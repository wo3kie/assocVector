/*
 * Copyright (C) 2012 £ukasz Czerwiñski
 *
 * GitHub: https://github.com/wo3kie/AssocVector
 * Website: http://www.lukaszczerwinski.pl/assoc_vector.en.html
 *
 * Distributed under the BSD Software License (see file license)
 */

#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#ifndef __GXX_EXPERIMENTAL_CXX0X__
    #error C++11 is required to run this code, please use AssocVector 1.0.x instead.
#endif

// includes.begin

#include <algorithm>
#include <functional>
#include <type_traits>

#include <cassert>
#include <cmath>

// includes.end

// configuration.begin

#if ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 8 )
    #define AV_HAS_TRIVIAL_DESTRUCTOR( type ) std::is_trivially_destructible< type >::value
#else
    #define AV_HAS_TRIVIAL_DESTRUCTOR( type ) __has_trivial_destructor( type )
#endif

// configuration.end

#ifndef NDEBUG
    #define AV_DEBUG
#endif

#ifdef AV_DEBUG
    #define AV_PRECONDITION( condition )    if( (bool)( condition ) == false ){int*i=0;*i=0;}
    #define AV_CHECK( condition )           if( (bool)( condition ) == false ){int*i=0;*i=0;}
    #define AV_POSTCONDITION( condition )   if( (bool)( condition ) == false ){int*i=0;*i=0;}
    #define AV_ERROR()                      if( true )                        {int*i=0;*i=0;}
#else
    #define AV_PRECONDITION( condition )    (void)( 0 );
    #define AV_CHECK( condition )           (void)( 0 );
    #define AV_POSTCONDITION( condition )   (void)( 0 );
    #define AV_ERROR()                      (void)( 0 );
#endif

namespace util
{
    //
    // CmpByFirst
    //
    template< typename _Pair, typename _Cmp >
    struct CmpByFirst
    {
        CmpByFirst( _Cmp const & cmp = _Cmp() )
            : _cmp( cmp )
        {
        }

        bool operator()( _Pair const & lhs, _Pair const & rhs )const
        {
            return _cmp( lhs.first, rhs.first );
        }

        bool operator()( _Pair const & pair, typename _Pair::first_type const & value )const
        {
            return _cmp( pair.first, value );
        }

        bool operator()( typename _Pair::first_type const & value, _Pair const & pair )const
        {
            return _cmp( value, pair.first );
        }

    private:
        _Cmp _cmp;
    };
}

namespace util
{
    //
    // is_between
    //
    template<
          typename _T1
        , typename _T2
    >
    inline
    bool
    is_between( _T1 const & first, _T2 const & value, _T1 const & last )
    {
        AV_PRECONDITION( first <= last );

        return ( value < first ) == false && ( last < value ) == false;
    }
}

namespace util
{
    //
    // destroy_range
    //
    namespace detail
    {
        template< bool _HasTrivialDestructor >
        struct DestroyRangeImpl
        {
        };

        template<>
        struct DestroyRangeImpl< true >
        {
            template< typename _Ptr >
            static
            void destroy( _Ptr, _Ptr )
            {
            }
        };

        template<>
        struct DestroyRangeImpl< false >
        {
            template< typename _Ptr >
            static
            void destroy( _Ptr first, _Ptr const last )
            {
                AV_PRECONDITION( first <= last );

                typedef typename std::iterator_traits< _Ptr >::value_type T;

                for( /*empty*/ ; first != last ; ++ first )
                {
                    AV_CHECK( first != 0 );

                    first -> T::~T();
                }
            }
        };
    }

    template< typename _Ptr >
    inline
    void
    destroy_range( _Ptr first, _Ptr const last )
    {
        typedef typename std::iterator_traits< _Ptr >::value_type T;

        detail::DestroyRangeImpl< AV_HAS_TRIVIAL_DESTRUCTOR( T ) >::destroy( first, last );
    }
}

namespace util
{
    template<
          typename _InputIterator
        , typename _OutputIterator
    >
    _OutputIterator
    move_forward( _InputIterator first, _InputIterator last, _OutputIterator output )
    {
        AV_PRECONDITION( first <= last );

        for( /*empty*/ ; first != last ; ++ output , ++ first ){
            * output = std::move( * first );
        }

        return output;
    }

    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    _OutputPtr
    uninitialized_move_forward( _InputPtr first, _InputPtr last, _OutputPtr output )
    {
        AV_PRECONDITION( first <= last );

        for( /*empty*/ ; first != last ; ++output, ++first )
        {
            AV_CHECK( first != 0 );
            AV_CHECK( output != 0 );

            new ( static_cast< void * >(  output ) )
                typename std::iterator_traits< _OutputPtr >::value_type( std::move( * first ) );
        }

        return output;
    }

    template<
          typename _InputIterator
        , typename _OutputIterator
    >
    _OutputIterator
    move_backward( _InputIterator first, _InputIterator last, _OutputIterator output )
    {
        AV_PRECONDITION( first <= last );

        while( first != last )
        {
            -- output;
            -- last;

            * output = std::move( * last );
        }

        return output;
    }
}

namespace util
{
    //
    // move
    //
    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    inline
    void
    move(
          _InputPtr first
        , _InputPtr last
        , _OutputPtr first2
    )
    {
        if( first < first2 ){
            util::move_backward( first, last, first2 + ( last - first ) );
        }
        else if( first > first2 ){
            util::move_forward( first, last, first2 );
        }
        else{
            // first == first2 -> do nothing
        }
    }
}

namespace util
{

template<
      typename _Iterator
    , typename _T
    , typename _Cmp
>
_Iterator
last_less_equal( _Iterator first, _Iterator last, _T const & t, _Cmp cmp )
{
    AV_PRECONDITION( first <= last );

    if( first == last ){
        return last;
    }

    _Iterator greaterEqual  = std::lower_bound( first, last, t, cmp );

    if( greaterEqual != last )
    {
        // lower_bound returns first greater_than/equal_to but we need last less_than

        bool const isEqual = cmp( t, * greaterEqual ) == false;

        if( isEqual )
        {
            // that is OK, request item was found
            return greaterEqual;
        }
    }

    if( greaterEqual == first )
    {
      // requested item does not exist in container

      // 6 8 10 13 17 19 20 21 22 24 end
      // ^ lower_bound( 1 ):
      // requested:                  ^

      return last;
    }
    else
    {
        // we need to go one item backward

        // 6 8 10 13 17 19 20 21 22 24
        // lower_bound( 23 ):       ^
        // requested:            ^

        return -- greaterEqual;
    }
}

}

namespace util
{

template<
      typename _InputIterator
    , typename _Cmp
>
bool is_sorted(
      _InputIterator first
    , _InputIterator const last
    , _Cmp const & cmp
)
{
    if( std::distance( first, last ) < 2 ){
        return true;
    }

    _InputIterator second = first;

    for( ++ second ; second != last ; ++ first, ++ second )
    {
        if( cmp( * first, * second ) == false ){
            return false;
        }
    }

    return true;
}

}

namespace util
{

template<
      typename _InputIterator1
    , typename _InputIterator2
    , typename _Cmp
>
bool has_intersection(
      _InputIterator1 first1
    , _InputIterator1 const last1
    , _InputIterator2 first2
    , _InputIterator2 const last2
    , _Cmp const & cmp
)
{
    while( first1 != last1 && first2 != last2 )
    {
        if( cmp( * first1, * first2 ) ){
            ++first1;
        }
        else if( cmp( * first2, * first1 ) ){
            ++first2;
        }
        else{
            return true;
        }
    }

    return false;
}

}

namespace array
{
    //
    // I need much more control over Array inside AssocVector than std::vector offers
    //

    // C' style array with some useful methods and functions

    //
    // Array
    //
    template< typename _T >
    struct Array
    {
        typedef _T value_type;

        typedef _T * iterator;
        typedef _T const * const_iterator;

        iterator begin(){
            return _data;
        }

        const_iterator begin()const
        {
            return _data;
        }

        iterator end()
        {
            return _data + _size;
        }

        const_iterator end()const
        {
            return _data + _size;
        }

        bool empty()const
        {
            return _size == 0;
        }

        bool full()const
        {
            return _size == _capacity;
        }

        std::size_t size()const
        {
            return _size;
        }

        std::size_t getSize()const
        {
            return size();
        }

        void setSize( std::size_t newSize )
        {
            AV_PRECONDITION( _data != 0 || newSize == 0 );

            _size = newSize;
        }

        std::size_t capacity()const
        {
            return _capacity;
        }

        std::size_t getCapacity()const
        {
            return capacity();
        }

        void setCapacity( std::size_t newCapacity )
        {
            AV_PRECONDITION( _data != 0 || newCapacity == 0 );

            _capacity = newCapacity;
        }

        value_type & front()
        {
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ 0 ];
        }

        value_type const & front()const
        {
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ 0 ];
        }

        value_type & back()
        {
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ _size - 1 ];
        }

        value_type const & back()const
        {
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ _size - 1 ];
        }

        _T * const data()const
        {
            return _data;
        }

        _T * const getData()const
        {
            return data();
        }

        _T * data()
        {
            return _data;
        }

        _T * getData()
        {
            return data();
        }

        void setData( _T * t )
        {
            _data = t;
        }

        value_type & operator[]( std::size_t index )
        {
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( index < size() );

            return _data[ index ];
        }

        value_type const & operator[]( std::size_t index )const
        {
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( index < size() );

            return _data[ index ];
        }

    private:
        _T * _data;

        std::size_t _capacity;
        std::size_t _size;
    };
}

namespace array
{
    template<
          typename _T
        , typename _Allocator
    >
    inline
    Array< _T >
    create(
            std::size_t capacity
          , _Allocator allocator
    )
    {
        AV_PRECONDITION( capacity <= allocator.max_size() );

        Array< _T > result;

        void * const rawMemory = allocator.allocate( capacity );

        AV_CHECK( rawMemory != 0 );

        result.setData( static_cast< _T * >( rawMemory ) );
        result.setSize( 0 );
        result.setCapacity( capacity );

        return result;
    }

    template<
          typename _T
        , typename _T2
        , typename _Allocator
    >
    inline
    void
    create(
          Array< _T > & object
        , Array< _T2 > const & other
        , _Allocator allocator
    )
    {
        AV_PRECONDITION( object.capacity() == 0 );
        AV_PRECONDITION( object.size() == 0 );
        AV_PRECONDITION( object.data() == 0 );

        object = create< _T >( other.capacity(), allocator );

        util::uninitialized_move_forward( other.begin(), other.end(), object.begin() );

        object.setSize( other.size() );
        object.setCapacity( other.capacity() );
    }

    template< typename _T >
    inline
    void
    reset( Array< _T > & array )
    {
        array.setData( 0 );
        array.setSize( 0 );
        array.setCapacity( 0 );
    }

    template<
          typename _T
        , typename _Allocator
    >
    inline
    void
    destroy_deallocate(
          Array< _T > & array
        , _Allocator allocator
    )
    {
        util::destroy_range( array.begin(), array.end() );
        allocator.deallocate( array.getData(), array.capacity() );
    }

    template<
          typename _T
        , typename _Allocator
    >
    inline
    void
    reserve(
          Array< _T > & array
        , std::size_t capacity
        , _Allocator allocator
    )
    {
        AV_PRECONDITION( capacity <= allocator.max_size() );

        if( capacity <= array.capacity() ){
            return;
        }

        Array< _T > newArray = array::create< _T >( capacity, allocator );

        util::uninitialized_move_forward( array.begin(), array.end(), newArray.begin() );

        array::destroy_deallocate( array, allocator );

        array.setData( newArray.getData() );
        array.setCapacity( newArray.getCapacity() );
    }

    template<
          typename _Iterator
        , typename _T
        , typename _Cmp
    >
    _Iterator
    find_in_sorted(
          _Iterator first
        , _Iterator last
        , _T const & t
        , _Cmp cmp
    )
    {
        AV_PRECONDITION( first <= last );

        _Iterator const greaterEqual = std::lower_bound( first, last, t, cmp );

        if( greaterEqual == last ){
            return last;
        }

        bool const isEqual = cmp( t, * greaterEqual ) == false;

        if( isEqual ){
            return greaterEqual;
        }

        return last;
    }

    template< typename _T >
    inline
    void
    insert(
          Array< _T > & array
        , typename Array< _T >::iterator pos
        , _T const & t
    )
    {
        AV_PRECONDITION( array.size() + 1 <= array.capacity() );
        AV_PRECONDITION( util::is_between( array.begin(), pos, array.end() ) );
        AV_PRECONDITION( array.end() != 0 );

        new ( static_cast< void * >( array.end() ) ) _T();

        if( pos != array.end() ){
            util::move( pos, array.end(), pos + 1 );
        }

        * pos = t;

        array.setSize( array.size() + 1 );
    }

    template<
          typename _T
        , typename _Cmp
    >
    std::pair< typename Array< _T >::iterator, bool >
    insert_in_sorted(
          Array< _T > & array
        , _T const & t
        , _Cmp cmp
    )
    {
        AV_PRECONDITION( array.size() + 1 <= array.capacity() );

        typename Array< _T >::iterator const greaterEqual
            = std::lower_bound( array.begin(), array.end(), t, cmp );

        if( greaterEqual != array.end() )
        {
            bool const isEqual = cmp( t, * greaterEqual ) == false;

            if( isEqual ){
                return std::make_pair( greaterEqual, false );
            }
        }

        insert( array, greaterEqual, t );

        return std::make_pair( greaterEqual, true );
    }

    template< typename _T >
    inline
    void
    erase(
          Array< _T > & array
        , typename Array< _T >::iterator pos
    )
    {
        AV_PRECONDITION( array.begin() <= pos );
        AV_PRECONDITION( pos < array.end() );

        util::move( pos + 1, array.end(), pos );
        ( array.end() - 1 )-> ~_T();
        array.setSize( array.size() - 1 );
    }

    template< typename _T >
    void
    erase_removed(
          array::Array< _T > & storage
        , array::Array< typename array::Array< _T >::const_iterator > const & erased
    )
    {
        AV_PRECONDITION( storage.size() >= erased.size() );

        if( erased.empty() ){
            return;
        }

        typedef typename array::Array< _T >::const_iterator StorageConstIterator;
        typedef typename array::Array< _T >::iterator StorageIterator;
        typedef typename array::Array< StorageConstIterator >::const_iterator ErasedConstIterator;

        StorageIterator currentInStorage = const_cast< StorageIterator >( erased.front() );
        AV_CHECK( util::is_between( storage.begin(), currentInStorage, storage.end() ) );

        StorageIterator const endInStorage = storage.end();

        StorageIterator whereInsertInStorage = const_cast< StorageIterator >( erased.front() );
        AV_CHECK( util::is_between( storage.begin(), whereInsertInStorage, storage.end() ) );

        ErasedConstIterator currentInErased = erased.begin();
        ErasedConstIterator const endInErased = erased.end();

        while( currentInStorage != endInStorage )
        {
            AV_CHECK( util::is_between( storage.begin(), whereInsertInStorage, storage.end() ) );

            if(
                   currentInErased != endInErased
                && currentInStorage == ( * currentInErased )
            )
            {
                ++ currentInStorage;
                ++ currentInErased;
            }
            else
            {
              ( * whereInsertInStorage ) = std::move( * currentInStorage );

              ++ whereInsertInStorage;
              ++ currentInStorage;
            }
        }

        AV_POSTCONDITION( currentInErased == endInErased );

        storage.setSize( storage.size() - erased.size() );
    }

    template<
          typename _T
        , typename _Cmp
    >
    void
    move_merge(
          array::Array< _T > & storage
        , array::Array< _T > & buffer
        , _Cmp const & cmp = _Cmp()
    )
    {
        AV_PRECONDITION( storage.size() + buffer.size() <= storage.capacity() );

        typedef typename array::Array< _T >::iterator Iterator;

        Iterator rWhereInsertInStorage = storage.begin() + storage.size() + buffer.size() - 1;

        Iterator rCurrentInStorage = storage.begin() + storage.size() - 1;
        Iterator const rEndInStorage = storage.begin() - 1;

        Iterator rCurrentInBuffer = buffer.begin() + buffer.size() - 1;
        Iterator const rEndInBuffer = buffer.begin() - 1;

        std::size_t numberOfItemsToCreateByPlacementNew = buffer.size();

        while(
               rCurrentInBuffer != rEndInBuffer
            && numberOfItemsToCreateByPlacementNew != 0
        )
        {
            AV_CHECK( rWhereInsertInStorage != 0 );
            AV_CHECK( rCurrentInStorage != 0 );
            AV_CHECK( rCurrentInBuffer != 0 );

            if(
                   rCurrentInStorage == rEndInStorage
                || cmp( * rCurrentInStorage, * rCurrentInBuffer )
            )
            {
                new ( static_cast< void * >( rWhereInsertInStorage ) )
                    _T( std::move( * rCurrentInBuffer ) );

                -- rCurrentInBuffer;
            }
            else
            {
                new ( static_cast< void * >( rWhereInsertInStorage ) )
                    _T( std::move( * rCurrentInStorage ) );

                -- rCurrentInStorage;
            }

            -- numberOfItemsToCreateByPlacementNew;
            -- rWhereInsertInStorage;
        }

        AV_CHECK( numberOfItemsToCreateByPlacementNew == 0 );

        while( rCurrentInBuffer != rEndInBuffer )
        {
            AV_CHECK( rWhereInsertInStorage != 0 );
            AV_CHECK( rCurrentInStorage != 0 );
            AV_CHECK( rCurrentInBuffer != 0 );

            if(
                   rCurrentInStorage == rEndInStorage
                || cmp( * rCurrentInStorage, * rCurrentInBuffer )
            )
            {
                * rWhereInsertInStorage = std::move( * rCurrentInBuffer );

                -- rCurrentInBuffer;
            }
            else
            {
                * rWhereInsertInStorage = std::move( * rCurrentInStorage );

                -- rCurrentInStorage;
            }

            -- rWhereInsertInStorage;
        }

        storage.setSize( storage.size() + buffer.size() );
    }
}

namespace util
{

template<
      typename _InputPtr1
    , typename _InputPtr2
    , typename _OutputPtr
    , typename _Cmp
>
_OutputPtr
move_merge_into_uninitialized(
      _InputPtr1 first1
    , _InputPtr1 last1
    , _InputPtr2 first2
    , _InputPtr2 last2
    , _OutputPtr output
    , _Cmp cmp = _Cmp()
)
{
    AV_PRECONDITION( first1 <= last1 );
    AV_PRECONDITION( first2 <= last2 );

    while( first1 != last1 && first2 != last2 )
    {
        AV_CHECK( first1 != 0 );
        AV_CHECK( first2 != 0 );
        AV_CHECK( output != 0 );

        if( cmp( * first1, * first2 ) )
        {
            new ( static_cast< void * >( output ) )
                typename std::iterator_traits< _OutputPtr >::value_type( std::move( * first1 ) );

            ++ output;
            ++ first1;
        }
        else
        {
            new ( static_cast< void * >( output ) )
                typename std::iterator_traits< _OutputPtr >::value_type( std::move( * first2 ) );

            ++ output;
            ++ first2;
        }
    }

    if( first1 == last1 ){
        return uninitialized_move_forward( first2, last2, output );
    }

    if( first2 == last2 ){
        return uninitialized_move_forward( first1, last1, output );
    }

    return output;
}

}

namespace detail
{
    template< typename _Iterator >
    bool equal( _Iterator const & lhs, _Iterator const & rhs )
    {
        if( lhs.getContainer() != rhs.getContainer() ){
            return false;
        }

        // for empty container returns that begin == end
        // despite on fact they are not
        if( lhs.getContainer()->empty() ){
            return true;
        }

        return lhs.getCurrent() == rhs.getCurrent();
    }

    //
    // AssocVectorLazyIterator
    //

    template<
          typename _Iterator
        , typename _Container
    >
    struct AssocVectorLazyIterator
    {
    private:
        typedef typename std::iterator_traits< _Iterator >::pointer pointer_mutable;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename std::iterator_traits< _Iterator >::value_type  value_type;
        typedef typename std::iterator_traits< _Iterator >::difference_type difference_type;

        // make key const
        typedef std::pair<
              typename value_type::first_type const
            , typename value_type::second_type
        > & reference;

        // make key const
        typedef std::pair<
              typename value_type::first_type const
            , typename value_type::second_type
        > * pointer;

    private:
        struct _CurrentInErased
        {
            _CurrentInErased( typename _Container::_Erased::const_iterator current )
                : _current( current )
            {
            }

            _CurrentInErased( _CurrentInErased const & other )
                : _current( other._current )
            {
            }

            _CurrentInErased & operator=( _CurrentInErased const & other )
            {
                _current = other._current;

                return * this;
            }

            _CurrentInErased & operator=( typename _Container::_Erased::const_iterator current )
            {
                _current = current;

                return * this;
            }

            bool is_end( _Container const * container )const
            {
                return _current == container->erased().end();
            }

            bool is_not_end( _Container const * container )const
            {
                return ! is_end( container );
            }

            bool is_begin( _Container const * container )const
            {
                return _current == container->erased().begin();
            }

            bool is_not_begin( _Container const * container )const
            {
                return ! is_begin( container );
            }

            void increment( _Container const * container )
            {
                AV_PRECONDITION( is_not_end( container ) );

                ++ _current;
            }

            void try_increment( _Container const * container )
            {
                if( is_end( container ) ){
                    return;
                }

                increment( container );
            }

            void decrement( _Container const * container )
            {
                AV_PRECONDITION( is_not_begin( container ) );

                -- _current;
            }

            void try_decrement( _Container const * container )
            {
                if( is_begin( container ) ){
                    return;
                }

                decrement( container );
            }

            bool validate( _Container const * container )const
            {
                bool const result
                    = util::is_between(
                          container->erased().begin()
                        , _current
                        , container->erased().end()
                    );

                if( result ){
                    return true;
                }

                AV_ERROR();

                return false;
            }

            typename _Container::_Erased::value_type
            get( _Container const * container )const
            {
                AV_PRECONDITION( _current );
                AV_PRECONDITION( is_not_end( container ) );
                AV_PRECONDITION( validate( container ) );

                return * _current;
            }

            typename _Container::_Erased::const_iterator data()const
            {
                return _current;
            }

            operator bool()const
            {
                return _current != 0;
            }

        private:
            typename _Container::_Erased::const_iterator _current;
        };

        struct _CurrentInBuffer
        {
            _CurrentInBuffer( pointer_mutable current )
                : _current( current )
            {
            }

            _CurrentInBuffer( _CurrentInBuffer const & other )
                : _current( other._current )
            {
            }

            _CurrentInBuffer & operator=( _CurrentInBuffer const & other )
            {
                _current = other._current;

                return * this;
            }

            _CurrentInBuffer & operator=( pointer_mutable current )
            {
                _current = current;

                return * this;
            }

            bool is_begin( _Container const * container )const
            {
                return _current == container->buffer().begin();
            }

            bool is_not_begin( _Container const * container )const
            {
                return ! is_begin( container );
            }

            bool is_end( _Container const * container )const
            {
                return _current == container->buffer().end();
            }

            bool is_not_end( _Container const * container )const
            {
                return ! is_end( container );
            }

            void increment( _Container const * container )
            {
                AV_PRECONDITION( is_not_end( container ) );

                ++ _current;
            }

            void try_increment( _Container const * container )
            {
                if( is_end( container ) ){
                    return;
                }

                increment( container );
            }

            void decrement( _Container const * container )
            {
                AV_PRECONDITION( is_not_begin( container ) );

                -- _current;
            }

            void try_decrement( _Container const * container )
            {
                if( is_begin( container ) ){
                    return;
                }

                decrement( container );
            }

            bool validate( _Container const * container )const
            {
                bool const result
                    = util::is_between(
                          container->buffer().begin()
                        , _current
                        , container->buffer().end()
                    );

                if( result ){
                    return true;
                }

                AV_ERROR();

                return false;
            }

            typename _Container::_Storage::value_type
            get( _Container const * container )const
            {
                AV_PRECONDITION( _current );
                AV_PRECONDITION( is_not_end( container ) );
                AV_PRECONDITION( validate( container ) );

                return * _current;
            }

            operator bool()const
            {
                return _current != 0;
            }

            pointer_mutable data()const
            {
                return _current;
            }
        private:
            pointer_mutable _current;
        };

        struct _CurrentInStorage
        {
            // begin is always set at first not erased item, marked with ^
            // end is always set at the end of container, marked with    $

            // Case 1: no item erased
            // storage: ^1 3 5 7$
            // erased :

            // Case 2: item erased from front
            // storage: 1 ^3 5 7$
            // erased : 1

            // Case 3: item erased from back
            // storage: ^1 3 5 7$
            // erased : 7

            // Case 4: item erased from front and back
            // storage: 1 ^3 5 7$
            // erased : 1 7

            _CurrentInStorage( pointer_mutable current )
                : _dir( 1 )
                , _current( current )
            {
            }

            _CurrentInStorage( _CurrentInStorage const & other )
                : _dir( other._dir )
                , _current( other._current )
            {
            }

            _CurrentInStorage & operator=( _CurrentInStorage const & other )
            {
                _dir = other._dir;
                _current = other._current;

                return * this;
            }

            bool operator==( _CurrentInStorage const & other )const
            {
                return _current == other._current;
            }

            bool operator!=( _CurrentInStorage const & other )const
            {
                return _current != other._current;
            }

            bool operator==( typename _Container::_Erased::value_type inErased )const
            {
                return _current == inErased;
            }

            bool operator!=( typename _Container::_Erased::value_type inErased )const
            {
                return _current != inErased;
            }

            bool is_begin( _Container const * container )const
            {
                _CurrentInStorage currentInStorage = const_cast< pointer_mutable >( container->storage().begin() );
                _CurrentInErased currentInErased = container->erased().begin();

                currentInStorage.setOnNotErased( currentInErased, container );

                return data() == currentInStorage.data();
            }

            bool is_not_begin( _Container const * container )const
            {
                return ! is_begin( container );
            }

            bool _is_begin( _Container const * container )const
            {
                return _current == container->storage().begin();
            }

            bool _is_not_begin( _Container const * container )const
            {
                return ! _is_begin( container );
            }

            bool is_end( _Container const * container )const
            {
                return _current == container->storage().end();
            }

            bool is_not_end( _Container const * container )const
            {
                return ! is_end( container );
            }

            void
            increment(
                  _CurrentInErased & currentInErased
                , _Container const * container
            )
            {
                AV_PRECONDITION( is_not_end( container ) );

                increment( container );

                if( _dir == -1 )
                {
                    _dir = 1;
                    currentInErased.try_increment( container );
                }

                setOnNotErased( currentInErased, container );
            }

            void
            try_increment(
                  _CurrentInErased & currentInErased
                , _Container const * container
            )
            {
                if( is_end( container ) ){
                    return;
                }

                increment( currentInErased, container );
            }

            void
            decrement(
                  _CurrentInErased & currentInErased
                , _Container const * container
            )
            {
                AV_PRECONDITION( is_not_begin( container ) );

                decrement( container );

                if( _dir == 1 )
                {
                    _dir = -1;
                    currentInErased.try_decrement( container );
                }

                setOnNotErasedBackward( currentInErased, container );
            }

            void
            try_decrement(
                  _CurrentInErased & currentInErased
                , _Container const * container
            )
            {
                if( _is_begin( container ) ){
                    return;
                }

                decrement( currentInErased, container );
            }

            void
            setOnNotErased(
                  _CurrentInErased & currentInErased
                , _Container const * container
            )
            {
                if( is_end( container ) )
                {
                    if( !currentInErased )
                    {
                        currentInErased = container->erased().end();
                    }

                    return;
                }

                if( !currentInErased )
                {
                    currentInErased = std::lower_bound(
                          container->erased().begin()
                        , container->erased().end()
                        , data()
                        , std::less< typename _Container::_Storage::const_iterator >()
                    );
                }

                if( _dir == -1 )
                {
                    _dir = 1;
                    currentInErased.try_increment( container );
                }

                while(
                       is_not_end( container )
                    && currentInErased.is_not_end( container )
                    && data() == currentInErased.get( container )
                )
                {
                    increment( container );
                    currentInErased.increment( container );
                }

                AV_POSTCONDITION( currentInErased );

                AV_POSTCONDITION(
                       (
                             is_end( container )
                          && currentInErased.is_end( container )
                       )
                    || currentInErased.is_end( container )
                    || *this != currentInErased.get( container )
                );
            }

            void
            setOnNotErasedBackward(
                  _CurrentInErased & currentInErased
                , _Container const * container
            )
            {
                AV_CHECK( is_not_end( container ) );

                if( _dir == 1 )
                {
                    _dir = -1;
                    currentInErased.try_decrement( container );
                }

                while(
                       is_not_begin( container )
                    && currentInErased.is_not_end( container )
                    && data() == currentInErased.get( container )
                )
                {
                    decrement( container );
                    currentInErased.try_decrement( container );
                }

                AV_POSTCONDITION( validate( container ) );

                AV_POSTCONDITION(
                       currentInErased.is_end( container )
                    || *this != currentInErased.get( container )
                );
            }

            operator bool()const
            {
                return _current != 0;
            }

            bool validate( _Container const * container )const
            {
                bool const result = util::is_between(
                          container->storage().begin()
                        , _current
                        , container->storage().end()
                    );

                if( result ){
                    return true;
                }

                AV_ERROR();

                return false;
            }

            typename _Container::_Storage::value_type
            get( _Container const * container )const
            {
                AV_PRECONDITION( _current );
                AV_PRECONDITION( is_not_end( container ) );

                return * _current;
            }

            pointer_mutable data()const
            {
                return _current;
            }

        private:
            void increment( _Container const * container )
            {
                AV_PRECONDITION( is_not_end( container ) );

                ++ _current;
            }

            void decrement( _Container const * container )
            {
                AV_PRECONDITION( is_not_begin( container ) );

                -- _current;
            }

        private:
            int _dir;

            pointer_mutable _current;
        };

        struct _Current
        {
            _Current( pointer_mutable current )
                : _current( current )
             {
             }

             _Current( _CurrentInStorage const & inStorage )
                : _current( inStorage.data() )
            {
            }

             _Current( _CurrentInBuffer const & inBuffer )
                : _current( inBuffer.data() )
            {
            }

            _Current & operator=( _Current const & other )
            {
                _current = other._current;

                return * this;
            }

            _Current & operator=( _CurrentInStorage const & inStorage )
            {
                _current = inStorage.data();

                return * this;
            }

            _Current & operator=( _CurrentInBuffer const & inBuffer )
            {
                _current = inBuffer.data();

                return * this;
            }

            bool operator==( _Current const & other )const
            {
                return _current == other._current;
            }

            bool operator==( _CurrentInStorage const & inStorage )const
            {
                return _current == inStorage.data();
            }

            bool operator==( _CurrentInBuffer const & inBuffer )const
            {
                return _current == inBuffer.data();
            }

            bool validate(
                  _CurrentInStorage currentInStorage
                , _CurrentInBuffer currentInBuffer
                , _Container const * container
            )const
            {
                AV_PRECONDITION( currentInStorage || currentInBuffer );
                AV_PRECONDITION( container != 0 );

                if( !currentInStorage )
                {
                    if( _current == currentInBuffer.data() ){
                        return true;
                    }

                    AV_ERROR();

                    return false;
                }

                if( !currentInBuffer )
                {
                    if( _current == currentInStorage.data() ){
                        return true;
                    }

                    AV_ERROR();

                    return false;
                }

                // if 'setLower' does not work 'validateCurrent' does not work as well :O(
                bool const result
                    = _current == getLower( currentInStorage, currentInBuffer, container ).data();

                if( result ){
                    return true;
                }

                AV_ERROR();

                return false;
            }

            void setLower(
                  _CurrentInStorage currentInStorage
                , _CurrentInBuffer currentInBuffer
                , _Container const * container
            )
            {
                _current = getLower( currentInStorage, currentInBuffer, container ).data();
            }

            _Current getLower(
                  _CurrentInStorage currentInStorage
                , _CurrentInBuffer currentInBuffer
                , _Container const * container
            )const
            {
                AV_CHECK( currentInStorage );
                AV_CHECK( currentInBuffer );

                if( currentInStorage.is_end( container ) )
                {
                    if( currentInBuffer.is_end( container ) ){
                        return _Current( 0 );
                    }
                    else{
                        return _Current( currentInBuffer );
                    }
                }
                else
                {
                    if( currentInBuffer.is_end( container ) ){
                        return _Current( currentInStorage );
                    }
                    else
                    {
                        if( container->value_comp()(
                                currentInStorage.get( container )
                              , currentInBuffer.get( container )
                            )
                        ){
                            return _Current( currentInStorage );
                        }
                        else{
                            return _Current( currentInBuffer );
                        }
                    }
                }
            }

            operator bool()const
            {
                return _current != 0;
            }

            typename _Container::_Storage::value_type
            get( _Container const * container )const
            {
                return * _current;
            }

            pointer_mutable data()const
            {
                return _current;
            }

        private:
            pointer_mutable _current;
        };

    public:
        AssocVectorLazyIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _container( 0 )

            , _currentInStorage( 0 )
            , _currentInBuffer( 0 )
            , _currentInErased( 0 )

            , _current( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorLazyIterator( AssocVectorLazyIterator< _Iter, _Container > const & other )
            : _container( other.getContainer() )

            , _currentInStorage( other.getCurrentInStorage() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _currentInErased( other.getCurrentInErased() )

            , _current( other.getCurrent() )
        {
        }

        AssocVectorLazyIterator(
              _Container const * container
            , pointer_mutable currentInStorage
            , pointer_mutable currentInBuffer
            , typename _Container::_Erased::const_iterator currentInErased
            , pointer_mutable current
        )
            : _container( container )

            , _currentInStorage( currentInStorage )
            , _currentInBuffer( currentInBuffer )
            , _currentInErased( currentInErased )

            , _current( current )
        {
            AV_PRECONDITION( container != 0 );
            AV_PRECONDITION( validate() );

            if( _currentInStorage && _currentInBuffer && !_currentInErased && !_current )
            {
                // not found in storage, insert to buffer
                // erase from buffer + find in storage
                // lower_bound
                // upper_bound

                // _currentInStorage <- fix against '!_currentInErased'
                _currentInStorage.setOnNotErased( _currentInErased, _container );

                // _current <- get it right now
                _current.setLower( _currentInStorage, _currentInBuffer, _container );
            }
            else
            if( _currentInStorage && _currentInBuffer && !_currentInErased && _current )
            {
                // not found in storage, found in buffer
                // not found in storage, inserted to buffer
                // erased from storage's back

                // _currentInStorage <- fix against '!_currentInErased'
                _currentInStorage.setOnNotErased( _currentInErased, _container );
            }
            else
            if( _currentInStorage && _currentInBuffer && _currentInErased && !_current )
            {
                // begin iterator
                // end iterator
                // erase from storage, not merged + find in buffer
                // erase from storage, merged + find in buffer + find in storage

                // _currentInStorage <- check against _currentInErased
                _currentInStorage.setOnNotErased( _currentInErased, _container );

                // _current <- get it right now
                _current.setLower( _currentInStorage, _currentInBuffer, _container );
            }

            AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );
            AV_POSTCONDITION( _currentInStorage || _currentInBuffer );
            AV_POSTCONDITION( _currentInErased );
            AV_POSTCONDITION( _container != 0 );
        }

        AssocVectorLazyIterator &
        operator=( AssocVectorLazyIterator const & other )
        {
            _container = other._container;

            _currentInStorage =  other._currentInStorage;
            _currentInBuffer = other._currentInBuffer;
            _currentInErased = other._currentInErased;

            _current = other._current;

            return * this;
        }

        bool operator==( AssocVectorLazyIterator const & other )const
        {
            resolveLazyValues();

            AV_PRECONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            return equal( *this, other );
        }

        bool operator!=( AssocVectorLazyIterator const & other )const
        {
            resolveLazyValues();

            AV_PRECONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            return ! ( ( * this ) == other );
        }

        AssocVectorLazyIterator & operator++()
        {
            resolveLazyValues();

            AV_PRECONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            if( _current == _Current( 0 ) ){
                return * this;
            }
            else if( _current == _currentInStorage ){
                _currentInStorage.try_increment( _currentInErased, _container );
            }
            else if( _current == _currentInBuffer ){
                _currentInBuffer.try_increment( _container );
            }
            else{
                AV_ERROR();
            }

            _current.setLower( _currentInStorage, _currentInBuffer, _container );

            AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            return * this;
        }

        AssocVectorLazyIterator operator++( int )
        {
            AssocVectorLazyIterator result( * this );

            ( * this ).operator++();

            return result;
        }

        AssocVectorLazyIterator & operator--()
        {
            resolveLazyValues();

            AV_PRECONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            if(
                   _currentInStorage.is_begin( _container )
                && _currentInBuffer.is_begin( _container )
            )
            {
                AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

                return * this;
            }

            if( _currentInStorage.is_begin( _container ) )
            {
                _currentInBuffer.decrement( _container );

                _current = _currentInBuffer;

                AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

                return * this;
            }

            if( _currentInBuffer.is_begin( _container ) )
            {
                _currentInStorage.decrement( _currentInErased, _container );

                _current = _currentInStorage;

                AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

                return * this;
            }

            _CurrentInStorage currentInStorage = _currentInStorage;
            _CurrentInBuffer currentInBuffer = _currentInBuffer;

            _CurrentInErased currentInErased = _currentInErased;

            currentInStorage.decrement( currentInErased, _container );
            currentInBuffer.decrement( _container );

            if(
                _container->value_comp()(
                      currentInStorage.get( _container )
                    , currentInBuffer.get( _container )
                )
            )
            {
                _currentInBuffer = currentInBuffer;

                _current = _currentInBuffer;
            }
            else
            {
                _currentInStorage = currentInStorage;
                _currentInErased = currentInErased;

                _current = _currentInStorage;
            }

            AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            return * this;
        }

        AssocVectorLazyIterator operator--( int )
        {
            AssocVectorLazyIterator result( * this );

            ( * this ).operator--();

            return result;
        }

        reference operator*()const
        {
            return * base();
        }

        pointer operator->()const
        {
            return base();
        }

        pointer base()const
        {
            AV_PRECONDITION( _current );
            AV_PRECONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            // make key const
            // pair< T1, T2 > * -> pair< T1 const, T2 > *
            //return reinterpret_cast< pointer >( _current );

            return
                reinterpret_cast< pointer >(
                    const_cast< void * >(
                        reinterpret_cast< void const * >( _current.data() )
                    )
                );
        }

        // public for copy constructor only : Iterator -> ConstIterator
        _Container const * getContainer()const{ return _container; }

        pointer_mutable getCurrentInStorage()const{ return _currentInStorage.data(); }
        pointer_mutable getCurrentInBuffer()const{ return _currentInBuffer.data(); }
        typename _Container::_Erased::const_iterator getCurrentInErased()const{ return _currentInErased.data(); }

        pointer_mutable getCurrent()const{ return _current.data(); }

    private:
        /*const function*/
        static
        void
        resolveLazyCurrentInBuffer(
              _CurrentInBuffer & currentInBuffer
            , _Current const current
            , _Container const * container
        )
        {
            if( currentInBuffer ){
                return;
            }

            currentInBuffer = const_cast< pointer_mutable >(
                std::lower_bound(
                      container->buffer().begin()
                    , container->buffer().end()
                    , current.get( container )
                    , container->value_comp()
                )
            );
        }

        static
        void
        resolveLazyCurrentInStorage(
              _CurrentInStorage & currentInStorage
            , _CurrentInErased & currentInErased
            , _Current const current
            , _Container const * container
        )
        {
            if( currentInStorage ){
                return;
            }

            currentInStorage = const_cast< pointer_mutable >(
                std::lower_bound(
                      container->storage().begin()
                    , container->storage().end()
                    , current.get( container )
                    , container->value_comp()
                )
            );

            currentInStorage.setOnNotErased( currentInErased, container );
        }

        void
        resolveLazyValues()const
        {
          resolveLazyCurrentInBuffer( _currentInBuffer, _current, _container );
          resolveLazyCurrentInStorage( _currentInStorage, _currentInErased, _current, _container );
        }

        /*pure function*/
        bool
        validate()const
        {
            if( !_currentInStorage && _currentInBuffer && _currentInErased && _current )
            {
                // not found in storage, inserted to buffer, buffer merged to storage

                AV_CHECK( _currentInBuffer.validate( _container ) );
                AV_CHECK( _currentInErased.is_end( _container ) );
                AV_CHECK( _current == _currentInBuffer );

                // _currentInStorage <- lazy in operator++/operator--/operator==/operator!=
            }
            else
            if( _currentInStorage && !_currentInBuffer && _currentInErased && !_current )
            {
                AV_ERROR();

                return false;
            }
            else
            if( _currentInStorage && !_currentInBuffer && _currentInErased && _current )
            {
                // found in storage, not found in erased

                AV_CHECK( _currentInStorage.validate( _container ) );
                AV_CHECK( std::binary_search( _container->erased().begin(), _container->erased().end(), _currentInStorage.data() ) == false );
                AV_CHECK( _currentInErased.validate( _container ) );
                AV_CHECK( _current == _currentInStorage );

                // _currentInBuffer <- lazy in operator++/operator--/operator==/operator!=
            }
            else
            if( _currentInStorage && _currentInBuffer && !_currentInErased && !_current )
            {
                // not found in storage, insert to buffer
                // erase from buffer + find in storage
                // lower_bound
                // upper_bound

                AV_CHECK( _currentInStorage.validate( _container ) );
                AV_CHECK( _currentInBuffer.validate( _container ) );

                // _currentInStorage <- fix against '!_currentInErased'
                // _current <- get it right now
            }
            else
            if( _currentInStorage && _currentInBuffer && !_currentInErased && _current )
            {
                // not found in storage, found in buffer
                // not found in storage, inserted to buffer
                // erased from storage's back

                AV_CHECK( _currentInStorage.validate( _container ) );
                AV_CHECK( _currentInBuffer.validate( _container ) );
                AV_CHECK( _current == _currentInBuffer );

                // _currentInStorage <- fix against '!_currentInErased'
                // _currentInErased <- get it right now
            }
            else
            if( _currentInStorage && _currentInBuffer && _currentInErased && !_current )
            {
                // begin iterator
                // end iterator
                // erase from storage, not merged + find in buffer
                // erase from storage, merged + find in buffer + find in storage

                AV_CHECK( _currentInStorage.validate( _container ) );
                AV_CHECK( _currentInBuffer.validate( _container ) );
                AV_CHECK( _currentInErased.validate( _container ) );

                // _currentInStorage <- check against _currentInErased
                // _current <- get it right now
            }
            else
            {
                AV_ERROR();

                return false;
            }

            return true;
        }

    private:
        _Container const * _container;

        // mutable since lazy values in operator==()const / operator!=()const
        mutable _CurrentInStorage _currentInStorage;
        mutable _CurrentInBuffer _currentInBuffer;
        mutable _CurrentInErased _currentInErased;

        _Current _current;
    };

    template<
          typename _Iterator
        , typename _Container
    >
    std::ostream &
    operator<<(
        std::ostream & out
        , AssocVectorLazyIterator< _Iterator, _Container > const & iter
    )
    {
        out << "S: " << iter.getCurrentInStorage();

        if( iter.getCurrentInStorage() == 0 ){
            out << " (null)";
        }
        else if( iter.getContainer()->storage().end() == iter.getCurrentInStorage() ){
            out << " (end)";
        }
        else{
            out << " " << * iter.getCurrentInStorage();
        }

        out << "\nB: " << iter.getCurrentInBuffer();

        if( iter.getCurrentInBuffer() == 0 ){
            out << " (null)";
        }
        else if( iter.getContainer()->buffer().end() == iter.getCurrentInBuffer() ){
            out << " (end)";
        }
        else{
            out << " " << * iter.getCurrentInBuffer();
        }

        out << "\nE: " << iter.getCurrentInErased();

        if( iter.getCurrentInErased() == 0 ){
            out << " (null)";
        }
        else if( iter.getContainer()->erased().end() == iter.getCurrentInErased() ){
            out << " (end)";
        }
        else{
            AV_CHECK( * iter.getCurrentInErased() );

            out << " " << * * iter.getCurrentInErased();
        }

        out << "\nC: " << iter.getCurrent();

        if( iter.getCurrent() == 0 ){
            out << " (null)";
        }
        else{
            out << " " << * iter.getCurrent();
        }

        std::flush( out );

        return out;
    }

    //
    // _AssocVectorIterator, simplified version of AssocVectorLazyIterator, works with _find and _end
    //
    template<
          typename _Iterator
        , typename _Container
    >
    struct _AssocVectorIterator
    {
    private:
        typedef typename std::iterator_traits< _Iterator >::pointer pointer_mutable;

    public:
        typedef typename std::iterator_traits< _Iterator >::value_type  value_type;

        // make key const
        typedef std::pair<
              typename value_type::first_type const
            , typename value_type::second_type
        > & reference;

        // make key const
        typedef std::pair<
              typename value_type::first_type const
            , typename value_type::second_type
        > * pointer;

    public:
        _AssocVectorIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _current( 0 )
        {
        }

        template< typename _Iter >
        _AssocVectorIterator( _AssocVectorIterator< _Iter, _Container > const & other )
            : _current( other.getCurrent() )
        {
        }

        _AssocVectorIterator( pointer_mutable current )
            : _current( current )
        {
        }

        _AssocVectorIterator &
        operator=( _AssocVectorIterator const & other )
        {
            _current = other._current;

            return * this;
        }

        bool operator==( _AssocVectorIterator const & other )const
        {
            return _current == other.getCurrent();
        }

        bool operator!=( _AssocVectorIterator const & other )const
        {
            return ! ( ( * this ) == other );
        }

        reference operator*()const
        {
            AV_PRECONDITION( _current != 0 );

            return * base();
        }

        pointer operator->()const
        {
            AV_PRECONDITION( _current != 0 );

            return base();
        }

        pointer base()const
        {
            AV_PRECONDITION( _current != 0 );

            // make key const
            // pair< T1, T2 > * -> pair< T1 const, T2 > *
            //return reinterpret_cast< pointer >( _current );

            return
                reinterpret_cast< pointer >(
                    const_cast< void * >(
                        reinterpret_cast< void const * >( _current )
                    )
                );
        }

        operator bool()const
        {
            return _current != 0;
        }

        // public for copy constructor only : Iterator -> ConstIterator
        pointer_mutable getCurrent()const
        {
            return _current;
        }

    private:
        pointer_mutable _current;
    };

    //
    // AssocVectorReverseIterator
    //
    template<
          typename _Iterator
        , typename _Container
    >
    struct AssocVectorReverseIterator
    {
    private:
        typedef typename std::iterator_traits< _Iterator >::pointer pointer_mutable;

    public:
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename std::iterator_traits< _Iterator >::value_type  value_type;
        typedef typename std::iterator_traits< _Iterator >::difference_type difference_type;

        // make key const
        typedef std::pair<
              typename value_type::first_type const
            , typename value_type::second_type
        > & reference;

        // make key const
        typedef std::pair<
              typename value_type::first_type const
            , typename value_type::second_type
        > * pointer;


    private:
        struct _RCurrentInErased
        {
            _RCurrentInErased( typename _Container::_Erased::const_iterator current )
                : _current( current )
            {
            }

            _RCurrentInErased( _RCurrentInErased const & other )
                : _current( other._current )
            {
            }

            _RCurrentInErased & operator=( typename _Container::_Erased::const_iterator current )
            {
                _current = current;

                return * this;
            }

            bool is_rbegin( _Container const * container )const
            {
                return _current == container->erased().end() - 1;
            }

            bool is_not_rbegin( _Container const * container )const
            {
                return ! is_rbegin( container );
            }

            bool is_rend( _Container const * container )const
            {
                return _current == container->erased().begin() - 1;
            }

            bool is_not_rend( _Container const * container )const
            {
                return ! is_rend( container );
            }

            void increment( _Container const * container )
            {
                AV_PRECONDITION( is_not_rend( container ) );

                -- _current; // reverse iterator
            }

            void try_increment( _Container const * container )
            {
                if( is_rend( container ) ){
                    return;
                }

                increment( container );
            }

            void decrement( _Container const * container )
            {
                AV_PRECONDITION( is_not_rbegin( container ) );

                ++ _current; // reverse iterator
            }

            void try_decrement( _Container const * container )
            {
                if( is_rbegin( container ) ){
                    return;
                }

                decrement( container );
            }

            typename _Container::_Erased::value_type
            get( _Container const * container )const
            {
                AV_PRECONDITION( _current );
                AV_PRECONDITION( is_not_rend( container ) );

                return * _current;
            }

            operator bool()const
            {
                return _current != 0;
            }

            typename _Container::_Erased::const_iterator
            data()const
            {
                return _current;
            }

        private:
            typename _Container::_Erased::const_iterator _current;
        };

        struct _RCurrentInBuffer
        {
            _RCurrentInBuffer( pointer_mutable current )
                : _current( current )
            {
            }

            _RCurrentInBuffer( _RCurrentInBuffer const & other )
                : _current( other._current )
            {
            }

            _RCurrentInBuffer & operator=( _RCurrentInBuffer const & other )
            {
                _current = other._current;

                return * this;
            }

            _RCurrentInBuffer & operator=( pointer_mutable current )
            {
                _current = current;

                return * this;
            }

            bool is_rbegin( _Container const * container )const
            {
                return _current == container->buffer().end() - 1;
            }

            bool is_not_rbegin( _Container const * container )const
            {
                return ! is_rbegin( container );
            }

            bool is_rend( _Container const * container )const
            {
                return _current == container->buffer().begin() - 1;
            }

            bool is_not_rend( _Container const * container )const
            {
                return ! is_rend( container );
            }

            void increment( _Container const * container )
            {
                AV_PRECONDITION( is_not_rend( container ) );

                -- _current; // reverse iterator
            }

            void try_increment( _Container const * container )
            {
                if( is_rend( container ) ){
                    return;
                }

                increment( container );
            }

            void decrement( _Container const * container )
            {
                AV_PRECONDITION( is_not_rbegin( container ) );

                ++ _current;
            }

            void try_decrement( _Container const * container )
            {
                if( is_rbegin( container ) ){
                    return;
                }

                decrement( container );
            }

            typename _Container::_Storage::value_type
            get( _Container const * container )const
            {
                AV_PRECONDITION( _current );
                AV_PRECONDITION( is_not_rend( container ) );

                return * _current;
            }

            operator bool()const
            {
                return _current != 0;
            }

            pointer_mutable data()const
            {
                return _current;
            }

        private:
            pointer_mutable _current;
        };

        struct _RCurrentInStorage
        {
            // rbegin is always set at last not erased item, marked with          ^
            // rend is always set one item before front of container, marked with $
            // _ item before a container, not a valid item of a container

            // Case 1: no item erased
            // storage: $_ 1 3 5 ^7
            // erased :

            // Case 2: item erased from front
            // storage: $_ 1 3 5 ^7
            // erased : 1

            // Case 3: item erased from back
            // storage: $_ 1 3 ^5 7
            // erased : 7

            // Case 4: item erased from front and back
            // storage: $_ 1 3 ^5 7
            // erased : 1 7

            _RCurrentInStorage( pointer_mutable current )
                : _dir( 1 )
                , _current( current )
            {
            }

            _RCurrentInStorage( _RCurrentInBuffer const & other )
                : _current( other._current )
            {
            }

            _RCurrentInStorage & operator=( _RCurrentInStorage const & other )
            {
                _dir = other._dir;
                _current = other._current;

                return * this;
            }

            _RCurrentInStorage & operator=( pointer_mutable current )
            {
                _current = current;

                return * this;
            }

            bool is_rbegin( _Container const * container )const
            {
                _RCurrentInStorage currentInStorage = const_cast< pointer_mutable >( container->storage().end() - 1 );
                _RCurrentInErased currentInErased = container->erased().end() - 1;

                currentInStorage.setOnNotErased( currentInErased, container );

                return data() == currentInStorage.data();
            }

            bool is_not_rbegin( _Container const * container )const
            {
                return ! is_rbegin( container );
            }

            bool is_rend( _Container const * container )const
            {
                return _current == container->storage().begin() - 1;
            }

            bool is_not_rend( _Container const * container )const
            {
                return ! is_rend( container );
            }

            void increment(
                  _RCurrentInErased & currentInErased
                , _Container const * container
            )
            {
                AV_PRECONDITION( is_not_rend( container ) );

                increment( container );

                if( _dir == -1 )
                {
                    _dir = 1;
                    currentInErased.try_increment( container );
                }

                setOnNotErased( currentInErased, container );
            }

            void try_increment(
                  _RCurrentInErased & inErased
                , _Container const * container
            )
            {
                if( is_rend( container ) ){
                    return;
                }

                increment( inErased, container );
            }

            void decrement(
                  _RCurrentInErased & currentInErased
                , _Container const * container
            )
            {
                AV_PRECONDITION( is_not_rbegin( container ) );

                decrement( container );

                if( _dir == 1 )
                {
                    _dir = -1;
                    currentInErased.try_decrement( container );
                }

                setOnNotErasedBackward( currentInErased, container );
            }

            void try_decrement(
                  _RCurrentInErased & currentInErased
                , _Container const * container
            )
            {
                if( is_rbegin( container ) ){
                    return;
                }

                decrement( currentInErased, container );
            }

            void setOnNotErased(
                  _RCurrentInErased & currentInErased
                , _Container const * container
            )
            {
                if( is_rend( container ) )
                {
                    if( !currentInErased ){
                        currentInErased = container->erased().begin() - 1;
                    }

                    return;
                }

                if( !currentInErased )
                {
                    currentInErased = util::last_less_equal(
                          container->erased().begin()
                        , container->erased().end()
                        , data()
                        , std::less< typename _Container::_Storage::const_iterator >()
                    );
                }

                if( _dir == -1 )
                {
                    _dir = 1;
                    currentInErased.try_increment( container );
                }

                while(
                       is_not_rend( container )
                    && currentInErased.is_not_rend( container )
                    && data() == currentInErased.get( container )
                )
                {
                    increment( container );
                    currentInErased.increment( container );
                }
            }

            void setOnNotErasedBackward(
                  _RCurrentInErased & currentInErased
                , _Container const * container
            )
            {
                AV_CHECK( is_not_rend( container ) );

                if( _dir == 1 )
                {
                    _dir = -1;
                    currentInErased.try_decrement( container );
                }

                while(
                       is_not_rbegin( container )
                    && currentInErased.is_not_rend( container )
                    && data() == currentInErased.get( container )
                )
                {
                    decrement( container );
                    currentInErased.try_decrement( container );
                }
            }

            typename _Container::_Storage::value_type
            get( _Container const * container )const
            {
                AV_PRECONDITION( _current );
                AV_PRECONDITION( is_not_rend( container ) );

                return * _current;
            }

            operator bool()const
            {
                return _current != 0;
            }

            pointer_mutable data()const
            {
                return _current;
            }

        private:

            void increment( _Container const * container )
            {
                AV_PRECONDITION( is_not_rend( container ) );

                -- _current; // reverse iterator
            }

            void decrement( _Container const * container )
            {
                AV_PRECONDITION( is_not_rbegin( container ) );

                ++ _current;
            }

        private:
            int _dir;

            pointer_mutable _current;
        };

        struct _RCurrent
        {
            _RCurrent( pointer_mutable current )
                : _current( current )
            {
            }

            _RCurrent( _RCurrent const & other )
                : _current( other._current )
            {
            }

            _RCurrent( _RCurrentInBuffer const & inBuffer )
                : _current( inBuffer.data() )
            {
            }

            _RCurrent( _RCurrentInStorage const & inStorage )
                : _current( inStorage.data() )
            {
            }

            bool operator==( _RCurrent const & other )const
            {
                return _current == other._current;
            }

            bool operator!=( _RCurrent const & other )const
            {
                return ! this->operator==( other );
            }

            bool operator==( pointer_mutable other )const
            {
                return _current == other;
            }

            bool operator!=( pointer_mutable other )const
            {
                return ! this->operator==( other );
            }

            bool operator==( _RCurrentInBuffer const & inBuffer )const
            {
                return _current == inBuffer.data();
            }

            bool operator!=( _RCurrentInBuffer const & inBuffer )const
            {
                return ! this->operator!=( inBuffer );
            }

            bool operator==( _RCurrentInStorage const & inStorage )const
            {
                return _current == inStorage.data();
            }

            bool operator!=( _RCurrentInStorage const & inStorage )const
            {
                return ! this->operator!=( inStorage );
            }

            void setGreater(
                  _RCurrentInStorage const & currentInStorage
                , _RCurrentInBuffer const & currentInBuffer
                , _Container const * container
            )
            {
                _current = getGreater( currentInStorage, currentInBuffer, container ).data();
            }

            _RCurrent getGreater(
                  _RCurrentInStorage const & currentInStorage
                , _RCurrentInBuffer const & currentInBuffer
                , _Container const * container
            )const
            {
                AV_CHECK( currentInStorage );
                AV_CHECK( currentInBuffer );

                if( currentInStorage.is_rend( container ) )
                {
                    if( currentInBuffer.is_rend( container ) ){
                        return _RCurrent( 0 );
                    }
                    else{
                        return _RCurrent( currentInBuffer );
                    }
                }
                else
                {
                    if( currentInBuffer.is_rend( container ) ){
                      return _RCurrent( currentInStorage );
                    }
                    else
                    {
                      if( container->value_comp()(
                              currentInBuffer.get( container )
                            , currentInStorage.get( container )
                          )
                      ){
                          return _RCurrent( currentInStorage );
                      }
                      else{
                          return _RCurrent( currentInBuffer );
                      }
                    }
                }
            }

            pointer_mutable data()const
            {
                return _current;
            }

        private:
            pointer_mutable _current;
        };
    public:
        AssocVectorReverseIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _container( 0 )

            , _currentInStorage( 0 )
            , _currentInBuffer( 0 )
            , _currentInErased( 0 )

            , _current( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorReverseIterator( AssocVectorReverseIterator< _Iter, _Container > const & other )
            : _container( other.getContainer() )

            , _currentInStorage( other.getRCurrentInStorage() )
            , _currentInBuffer( other.getRCurrentInBuffer() )
            , _currentInErased( other.getRCurrentInErased() )

            , _current( other.getCurrent() )
        {
        }

        AssocVectorReverseIterator(
              _Container const * container
            , pointer_mutable currentInStorage
            , pointer_mutable currentInBuffer
            , typename _Container::_Erased::const_iterator currentInErased
            , pointer_mutable current
        )
            : _container( container )

            , _currentInStorage( currentInStorage )
            , _currentInBuffer( currentInBuffer )
            , _currentInErased( currentInErased )

            , _current( current )
        {
            _currentInStorage.setOnNotErased( _currentInErased, _container );

            _current.setGreater( _currentInStorage, _currentInBuffer, _container );
        }

        AssocVectorReverseIterator & operator=( AssocVectorReverseIterator const & other )
        {
            _container = other._container;

            _currentInStorage =  other._currentInStorage;
            _currentInBuffer = other._currentInBuffer;
            _currentInErased = other._currentInErased;

            _current = other._current;

            return * this;
        }

        bool operator==( AssocVectorReverseIterator const & other )const
        {
            return equal( *this, other );
        }

        bool operator!=( AssocVectorReverseIterator const & other )const
            {
            return ! ( ( * this ) == other );
        }

        AssocVectorReverseIterator & operator++()
        {
            if( _current == 0 ){
                return * this;
            }
            else if( _current == _currentInStorage ){
                _currentInStorage.increment( _currentInErased, _container );
            }
            else if( _current == _currentInBuffer ){
                _currentInBuffer.increment( _container );
            }
            else{
                AV_ERROR();
            }

            _current.setGreater( _currentInStorage, _currentInBuffer, _container );

            return * this;
        }

        AssocVectorReverseIterator operator++( int )
        {
            AssocVectorReverseIterator result( * this );

            ( * this ).operator++();

            return result;
        }

        AssocVectorReverseIterator & operator--()
        {
            if(
                   _currentInStorage.is_rbegin( _container )
                && _currentInBuffer.is_rbegin( _container )
            ){
                return * this;
            }

            if( _currentInStorage.is_rbegin( _container ) )
            {
                _currentInBuffer.decrement( _container );
                _current = _currentInBuffer;

                return * this;
            }

            if( _currentInBuffer.is_rbegin( _container ) )
            {
                _currentInStorage.decrement( _currentInErased, _container );
                _current = _currentInStorage;

                return * this;
            }

            _RCurrentInStorage currentInStorage = _currentInStorage;
            _RCurrentInBuffer currentInBuffer = _currentInBuffer;

            _RCurrentInErased currentInErased = _currentInErased;

            currentInStorage.decrement( currentInErased, _container );
            currentInBuffer.decrement( _container );

            if(
                _container->value_comp()(
                      currentInBuffer.get( _container )
                    , currentInStorage.get( _container )
                )
            )
            {
                _currentInBuffer = currentInBuffer;

                _current = _currentInBuffer;
            }
            else
            {
                _currentInStorage = currentInStorage;
                _currentInErased = currentInErased;

                _current = _currentInStorage;
            }

            //AV_POSTCONDITION( _current.validate( _currentInStorage, _currentInBuffer, _container ) );

            return * this;
        }

        AssocVectorReverseIterator operator--( int )
        {
            AssocVectorReverseIterator result( * this );

            ( * this ).operator--();

            return result;
        }

        reference operator*()const
        {
            return * base();
        }

        pointer operator->()const
        {
            return base();
        }

        pointer base()const
        {
            return
                reinterpret_cast< pointer >(
                    const_cast< void * >(
                        reinterpret_cast< void const * >( _current.data() )
                    )
                );
        }

        // public for copy constructor only : Iterator -> ConstIterator
        _Container const * getContainer()const{ return _container; }

        pointer_mutable getRCurrentInStorage()const{ return _currentInStorage.data(); }
        pointer_mutable getRCurrentInBuffer()const{ return _currentInBuffer.data(); }
        typename _Container::_Erased::const_iterator getRCurrentInErased()const{ return _currentInErased.data(); }

        pointer_mutable getCurrent()const{ return _current.data(); }

    private:
        _Container const * _container;

        _RCurrentInStorage _currentInStorage;
        _RCurrentInBuffer _currentInBuffer;
        _RCurrentInErased _currentInErased;

        _RCurrent _current;
    };

    template<
          typename _Iterator
        , typename _Container
    >
    std::ostream &
    operator<<(
          std::ostream & out
        , AssocVectorReverseIterator< _Iterator, _Container > const & iter
    )
    {
        out << "S: " << iter.getRCurrentInStorage();

        if( iter.getRCurrentInStorage() == 0 ){
            out << " (null)";
        }
        else if( iter.getRCurrentInStorage() == iter.getContainer()->storage().begin() - 1 ){
            out << " (rend)";
        }
        else{
            out << " " << * iter.getRCurrentInStorage();
        }

        out << "\nB: " << iter.getRCurrentInBuffer();

        if( iter.getRCurrentInBuffer() == 0 ){
                out << " (null)";
        }
        else if( iter.getRCurrentInBuffer() == iter.getContainer()->buffer().begin() - 1 ){
            out << " (rend)";
        }
        else{
            out << " " << * iter.getRCurrentInBuffer();
        }

        out << "\nE: " << iter.getRCurrentInErased();

        if( iter.getRCurrentInErased() == 0 ){
            out << " (null)";
        }
        else if( iter.getRCurrentInErased() == iter.getContainer()->erased().begin() - 1 ){
            out << " (rend)";
        }
        else{
            AV_CHECK( * iter.getRCurrentInErased() );

            out << " " << * * iter.getRCurrentInErased();
        }

        out << "\nC: " << iter.getCurrent();

        if( iter.getCurrent() == 0 ){
            out << " (null)";
        }
        else{
            out << " " << * iter.getCurrent();
        }

        return out;
    }

} // namespace detail

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp = std::less< _Key >
    , typename _Allocator = std::allocator< std::pair< _Key, _Mapped > >
>
struct AssocVector
{
private:
    typedef std::pair< _Key, _Mapped > value_type_mutable;
    typedef std::pair< _Key const, _Mapped > value_type_key_const;

public:
    typedef _Key key_type;
    typedef _Mapped mapped_type;

    typedef value_type_key_const value_type;
    typedef mapped_type & reference;

    typedef _Cmp key_compare;
    typedef _Allocator allocator_type;

    typedef util::CmpByFirst< value_type_mutable, _Cmp > value_compare;

    typedef detail::AssocVectorLazyIterator< value_type_mutable *, AssocVector > iterator;
    typedef detail::AssocVectorLazyIterator< value_type_mutable const *, AssocVector > const_iterator;

    typedef detail::AssocVectorReverseIterator< value_type_mutable *, AssocVector > reverse_iterator;
    typedef detail::AssocVectorReverseIterator< value_type_mutable const *, AssocVector > const_reverse_iterator;

    typedef array::Array< value_type_mutable > _Storage;

    typedef array::Array< typename _Storage::const_iterator > _Erased;

#ifdef AV_ENABLE_EXTENSIONS
    public:
#else
    private:
#endif

    //
    // extension, faster, non STL compatible version of iterator, working with _find end _end
    //
    typedef detail::_AssocVectorIterator< value_type_mutable *, AssocVector > _iterator;
    typedef detail::_AssocVectorIterator< value_type_mutable const *, AssocVector > _const_iterator;

private:
    struct _FindOrInsertToBufferResult
    {
        typename _Storage::iterator _inBuffer;
        bool _isInserted;
        bool _isReallocated;
    };

    struct _TryRemoveBackResult
    {
        bool _anyItemRemoved;
        bool _erasedItemRemoved;
    };

    struct _FindImplResult
    {
        typename _Storage::iterator _inStorage;
        typename _Storage::iterator _inBuffer;
        typename _Erased::iterator _inErased;
        typename _Storage::iterator _current;

        bool validate()const
        {
            return
                   ( _current == 0 && _inStorage == 0 && _inBuffer == 0 && _inErased == 0 )
                || _inStorage != 0;
        }
    };

    struct _InsertImplResult
    {
        bool _isInserted;

        typename _Storage::iterator _inStorage;
        typename _Storage::iterator _inBuffer;
        typename _Erased::iterator _inErased;
        typename _Storage::iterator _current;

        bool validate()const
        {
            if( _current == 0 )
            {
                AV_ERROR();

                return false;
            }

            if( _inStorage == 0 && ( _inBuffer == 0 || _inErased == 0 ) )
            {
                AV_ERROR();

                return false;
            }

            return true;
        }
    };

    struct _TryEraseFromStorageResult
    {
        typename _Erased::iterator _inErased;
        bool _isErased;
        bool _isMerged;
    };

public:
    //
    // constructor
    //
    explicit AssocVector( _Cmp const & cmp = _Cmp(), _Allocator const & allocator = _Allocator() );
    explicit AssocVector( _Allocator const & allocator );
    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & other );

    template< typename __InputIterator >
    AssocVector(
          __InputIterator first
        , __InputIterator last
        , _Cmp const & cmp = _Cmp()
        , _Allocator const & allocator = _Allocator()
    );

    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Allocator > && other );

    //
    // destructor
    //
    inline ~AssocVector();

    //
    // clear
    //
    inline void clear();

    //
    // operator=
    //
    AssocVector & operator=( AssocVector const & other );
    AssocVector & operator=( AssocVector && other );

    //
    // methods
    //
    void reserve( std::size_t newCapacity );
    void swap( AssocVector & other );

    //
    // iterators
    //
    inline iterator begin();
    inline const_iterator begin()const;

    inline reverse_iterator rbegin();
    inline const_reverse_iterator rbegin()const;

    inline iterator end();
    inline const_iterator end()const;

    inline reverse_iterator rend();
    inline const_reverse_iterator rend()const;

    //
    // size
    //
    inline bool empty()const;
    inline std::size_t size()const;

    //
    // insert
    //
    std::pair< iterator, bool > insert( value_type const & value );

    iterator insert( iterator hint, value_type const & value );

    template< typename _Iterator >
    inline void insert( _Iterator begin, _Iterator end );

    //
    // find
    //
    iterator find( key_type const & k );
    const_iterator find( key_type const & k )const;

    iterator lower_bound( key_type const & k );
    const_iterator lower_bound( key_type const & k )const;

    iterator upper_bound( key_type const & k );
    const_iterator upper_bound( key_type const & k )const;

    std::pair< iterator, iterator > equal_range( key_type const & k );
    std::pair< const_iterator, const_iterator > equal_range( key_type const & k )const;

    //
    // count
    //
    inline std::size_t count( key_type const & k )const;

    //
    // operator[]
    //
    reference operator[]( key_type const & k );

    //
    // erase
    //
    std::size_t erase( key_type const & k );
    iterator erase( iterator pos );

    //
    // observers
    //
    key_compare key_comp()const
    {
        return _cmp;
    }

    value_compare value_comp()const
    {
        return value_compare( _cmp );
    }

#ifdef AV_ENABLE_EXTENSIONS
    public:
#else
    private:
#endif

    //
    // extension, flatenize container, enforce merge of _storage with _erased and with _buffer
    //
    void merge();

    //
    // extension, faster, non STL compatible version of insert
    //
    bool _insert( value_type const & value );

    //
    // extension, faster, non STL compatible version of end, works with _find
    //
    inline _iterator _end();
    inline _const_iterator _end()const;

    //
    // extension, faster, non STL compatible version of find, works with _end
    //
    _iterator _find( key_type const & k );
    _const_iterator _find( key_type const & k )const;

    //
    // extension, faster, non STL compatible version of erase
    //
    bool _erase( iterator pos );

private:
    bool validateStorage()const;
    bool validateBuffer()const;
    bool validateErased()const;
    bool validate()const;

    //
    // merge
    //
    void mergeStorageWithBuffer();
    void mergeStorageWithErased();

    //
    // insert
    //
    bool tryPushBack( key_type const & k, mapped_type const & m );

    _FindOrInsertToBufferResult
    findOrInsertToBuffer( key_type const & k, mapped_type const & m );

    //
    // insertImpl, function does as little as needed but returns as much data as possible
    //
    _InsertImplResult
    insertImpl( value_type const & value );

    //
    // erase
    //
    _TryRemoveBackResult
    tryRemoveStorageBack( typename _Storage::iterator pos );

    //
    // tryEraseFromStorage
    //
    _TryEraseFromStorageResult
    tryEraseFromStorage( typename _Storage::iterator pos );

    //
    // isErased
    //
    bool isErased( typename _Storage::const_iterator iterator )const;

    //
    // findImpl, function does as little as needed but returns as much data as possible
    //
    _FindImplResult
    findImpl( key_type const & key );

    //
    // getAllocator (method specialization)
    //
    _Allocator getAllocator( _Storage const & ){ return _allocator; }

    typename _Allocator::template rebind< typename _Storage::const_iterator >::other
    getAllocator( _Erased const & )
    {return typename _Allocator::template rebind< typename _Storage::const_iterator >::other( _allocator );}

public: // public for unit tests only
    void dump( int width = -1 )const;

    std::size_t bufferSize()const{ return _buffer.size(); }
    std::size_t bufferCapacity()const{ return _buffer.capacity(); }
    _Storage const & storage()const{ return _storage; }

    std::size_t storageSize()const{ return _storage.size(); }
    std::size_t storageCapacity()const{ return _storage.capacity(); }
    _Storage const & buffer()const{ return _buffer; }

    std::size_t erasedSize()const{ return _erased.size(); }
    std::size_t erasedCapacity()const{ return _erased.capacity(); }
    _Erased const & erased()const{ return _erased; }

    static std::size_t calculateNewBufferCapacity( std::size_t storageSize );
    static std::size_t calculateNewErasedCapacity( std::size_t storageSize );
    static std::size_t calculateNewStorageCapacity( std::size_t storageSize );

private:
    _Storage _storage;
    _Storage _buffer;

    _Erased _erased;

    _Cmp _cmp;

    _Allocator _allocator;
};

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool operator==(
      AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & lhs
    , AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & rhs
)
{
    if( lhs.size() != rhs.size() ){
        return false;
    }

    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator begin = lhs.begin();
    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator const end = lhs.end();

    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator begin2 = rhs.begin();

    for( /*empty*/ ; begin != end ; ++ begin, ++ begin2 )
    {
        if( begin->first != begin2->first ){
            return false;
        }

        if( begin->second != begin2->second ){
            return false;
        }
    }

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool operator!=(
      AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & lhs
    , AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & rhs
)
{
    return ! ( lhs == rhs );
}

//
// Method Definitions
//

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::AssocVector(
      _Cmp const & cmp
    , _Allocator const & allocator
)
    : _cmp( cmp )
    , _allocator( allocator )
{
    array::reset( _storage );
    array::reset( _buffer );
    array::reset( _erased );

    std::size_t const defaultSize = 2*(2*2);

    reserve( defaultSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::AssocVector( _Allocator const & allocator )
    : _allocator( allocator )
{
    array::reset( _storage );
    array::reset( _buffer );
    array::reset( _erased );

    std::size_t const defaultSize = 2*(2*2);

    reserve( defaultSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::AssocVector(
    AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & other
)
    : _cmp( other._cmp )
    , _allocator( other._allocator )
{
    array::reset( _storage );
    array::create< value_type_mutable >( _storage, other._storage, getAllocator( _storage ) );

    array::reset( _buffer );
    array::create< value_type_mutable >( _buffer, other._buffer, getAllocator( _buffer ) );

    array::reset( _erased );
    array::create< typename _Storage::const_iterator >( _erased, other._erased, getAllocator( _erased ) );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
template<
    typename __InputIterator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::AssocVector(
      __InputIterator first
    , __InputIterator last
    , _Cmp const & cmp
    , _Allocator const & allocator
)
    : _cmp( cmp )
    , _allocator( allocator )
{
    array::reset( _storage );
    array::reset( _buffer );
    array::reset( _erased );

    std::size_t const defaultSize = 2*(2*2);

    reserve( defaultSize );

    for( /*empty*/ ; first != last ; ++ first ){
        insert( * first );
    }

    AV_POSTCONDITION( validate() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::AssocVector(
    AssocVector< _Key, _Mapped, _Cmp, _Allocator > && other
)
    : _storage( other._storage )
    , _buffer( other._buffer )
    , _erased( other._erased )
    , _cmp( other._cmp )
    , _allocator( other._allocator )
{
    array::reset( other._storage );
    array::reset( other._buffer );
    array::reset( other._erased );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::~AssocVector()
{
    clear();

    getAllocator( _storage ).deallocate( _storage.data(), _storage.getCapacity() );
    getAllocator( _buffer ).deallocate( _buffer.data(), _buffer.getCapacity() );
    getAllocator( _erased ).deallocate( _erased.data(), _erased.getCapacity() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::clear()
{
    util::destroy_range( _storage.begin(), _storage.end() );
    util::destroy_range( _buffer.begin(), _buffer.end() );

    _storage.setSize( 0 );
    _buffer.setSize( 0 );
    _erased.setSize( 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator > &
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::operator=( AssocVector const & other )
{
    AssocVector temp( other );
    temp.swap( * this );

    return * this;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator > &
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::operator=( AssocVector && other )
{
    AssocVector temp( std::move( other ) );
    temp.swap( * this );

    return * this;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::reserve( std::size_t newStorageCapacity )
{
    if( newStorageCapacity <= _storage.capacity() ){
        return;
    }

    {// _erased
        if( _erased.empty() == false ){
            mergeStorageWithErased();
        }

        array::reserve(
              _erased
            , calculateNewErasedCapacity( newStorageCapacity )
            , getAllocator( _erased )
        );

        AV_CHECK( _erased.empty() );
    }

    std::size_t const newBufferCapacity
        = calculateNewBufferCapacity( newStorageCapacity );

    _Storage newStorage
        = array::create< value_type_mutable >( newStorageCapacity, getAllocator( _storage ) );

    _Storage newBuffer
        = array::create< value_type_mutable >( newBufferCapacity, getAllocator( _buffer ) );

    util::move_merge_into_uninitialized(
          _storage.begin()
        , _storage.end()
        , _buffer.begin()
        , _buffer.end()
        , newStorage.begin()
        , value_comp()
    );

    {
        std::size_t const oldSize = _storage.size();

        array::destroy_deallocate( _storage, getAllocator( _storage ) );

        _storage.setData( newStorage.getData() );
        _storage.setSize( oldSize + _buffer.size() );
        _storage.setCapacity( newStorageCapacity );
    }

    {
        array::destroy_deallocate( _buffer, getAllocator( _buffer ) );

        _buffer.setData( newBuffer.getData() );
        _buffer.setSize( 0 );
        _buffer.setCapacity( newBufferCapacity );
    }

    AV_POSTCONDITION( _buffer.empty() );
    AV_POSTCONDITION( validate() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::begin()
{
    return iterator( this, _storage.begin(), _buffer.begin(), _erased.begin(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::rbegin()
{
    return reverse_iterator( this, _storage.end() - 1, _buffer.end() - 1, _erased.end() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::begin()const
{
    return const_iterator( this, _storage.begin(), _buffer.begin(), _erased.begin(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::rbegin()const
{
    return const_reverse_iterator( this, _storage.end() - 1, _buffer.end() - 1, _erased.end() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::end()
{
    return iterator( this, _storage.end(), _buffer.end(), _erased.end(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_end()
{
    return _iterator( 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::rend()
{
    return reverse_iterator( this, _storage.begin() - 1, _buffer.begin() - 1, _erased.begin() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::end()const
{
    return const_iterator( this, _storage.end(), _buffer.end(), _erased.end(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_end()const
{
    return _const_iterator( 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::rend()const
{
    return const_reverse_iterator( this, _storage.begin() - 1, _buffer.begin() - 1, _erased.begin() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::empty()const
{
    return size() == 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::size()const
{
    return _storage.size() + _buffer.size() - _erased.size();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::pair< typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator, bool >
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::insert( value_type const & value )
{
    _InsertImplResult const result = insertImpl( value );

    return std::make_pair(
          iterator(
              this
            , result._inStorage
            , result._inBuffer
            , result._inErased
            , result._current
          )
        , result._isInserted
    );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::insert(
      typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator hint
    , value_type const & value
)
{
    // todo: use hint
    (void)(hint);

    _InsertImplResult const result = insertImpl( value );

    return iterator(
          this
        , result._inStorage
        , result._inBuffer
        , result._inErased
        , result._current
    );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_insert( value_type const & value )
{
    return insertImpl( value )._isInserted;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_InsertImplResult
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::insertImpl( value_type const & value )
{
    _Key const & k = value.first;
    _Mapped const & m = value.second;

    {//push back to storage
        if( tryPushBack( k, m ) )
        {
            _InsertImplResult result;
            result._isInserted = true;
            result._inStorage = ( _storage.end() - 1 );
            result._inBuffer = 0;
            result._inErased = _erased.end();
            result._current = ( _storage.end() - 1 );

            AV_POSTCONDITION( result.validate() );
            AV_POSTCONDITION( validate() );

            return result;
        }
    }

    typename _Storage::iterator const greaterEqualInStorage
        = std::lower_bound( _storage.begin(), _storage.end(), k, value_comp() );

    bool const notPresentInStorage
        = greaterEqualInStorage == _storage.end()
        || key_comp()( k, greaterEqualInStorage->first );

    {//find or insert to buffer
        if( notPresentInStorage )
        {
            _FindOrInsertToBufferResult const findOrInsertToBufferResult = findOrInsertToBuffer( k, m );

            _InsertImplResult result;
            result._isInserted = findOrInsertToBufferResult._isInserted;

            if( findOrInsertToBufferResult._isReallocated )
            {
                result._inStorage = 0;
                result._inErased = _erased.end();
            }
            else
            {
                result._inStorage = greaterEqualInStorage;
                result._inErased = 0;
            }

            result._inBuffer = findOrInsertToBufferResult._inBuffer;
            result._current = findOrInsertToBufferResult._inBuffer;

            AV_POSTCONDITION( result.validate() );
            AV_POSTCONDITION( validate() );

            return result;
        }
    }

    {// check if not erased
        typename _Erased::iterator const greaterEqualInErased = std::lower_bound(
              _erased.begin()
            , _erased.end()
            , greaterEqualInStorage
            , std::less< typename _Storage::const_iterator >()
        );

        bool const itemNotMarkedAsErased
            = greaterEqualInErased == _erased.end()
            || std::less< typename _Storage::const_iterator >()
                    ( greaterEqualInStorage, * greaterEqualInErased );

        if( itemNotMarkedAsErased )
        {// item is in storage and is not marked as erased
            _InsertImplResult result;
            result._isInserted = false;
            result._inStorage = greaterEqualInStorage;
            result._inBuffer = 0;
            result._inErased = greaterEqualInErased;
            result._current = greaterEqualInStorage;

            AV_POSTCONDITION( result.validate() );
            AV_POSTCONDITION( validate() );

            return result;
        }
        else
        {// item is in storage but is marked as erased
            array::erase( _erased, greaterEqualInErased );

            greaterEqualInStorage->second = m;

            _InsertImplResult result;
            result._isInserted = true;
            result._inStorage = greaterEqualInStorage;
            result._inBuffer = 0;

            // greaterEqualInErased is after 'array::erase' but still valid
            result._inErased = greaterEqualInErased;

            result._current = greaterEqualInStorage;

            AV_POSTCONDITION( validate() );
            AV_POSTCONDITION( result.validate() );

            return result;
        }
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
template<
    typename _Iterator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::insert( _Iterator const begin, _Iterator const end )
{
    for( _Iterator current = begin ; current != end ; ++ current ){
        insert( * current );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::tryPushBack( _Key const & k, _Mapped const & m )
{
    bool pushBackToStorage = false;

    {// should be pushed back
        if( _storage.empty() )
        {
            if( _buffer.empty() ){
                pushBackToStorage = true;
            }
            else
            {
                if( _cmp( _buffer.back().first, k ) ){
                    pushBackToStorage = true;
                }
            }
        }
        else
        {
            if( _buffer.empty() )
            {
                if( _cmp( _storage.back().first, k ) ){
                    pushBackToStorage = true;
                }
            }
            else
            {
                if( _cmp( _storage.back().first, k ) && _cmp( _buffer.back().first,k ) ){
                    pushBackToStorage = true;
                }
            }
        }
    }

    if( pushBackToStorage == false ){
        return false;
    }

    if( _storage.size() == _storage.capacity() ){
        reserve( calculateNewStorageCapacity( _storage.capacity() ) );
    }

    {//push back
        new ( _storage.end() ) value_type_mutable( k, m );

        _storage.setSize( _storage.size() + 1 );
    }

    AV_POSTCONDITION( validate() );

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_TryRemoveBackResult
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::tryRemoveStorageBack(
    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_Storage::iterator pos
)
{
    if( pos + 1 != _storage.end() )
    {
        _TryRemoveBackResult result;
        result._anyItemRemoved = false;
        result._erasedItemRemoved = false;

        return result;
    }

    getAllocator( _storage ).destroy( pos );

    _storage.setSize( _storage.size() - 1 );

    if(
           _erased.empty() == false
        && _erased.back() == pos
    )
    {
        _erased.setSize( _erased.size() - 1 );

        _TryRemoveBackResult result;
        result._anyItemRemoved = true;
        result._erasedItemRemoved = true;

        AV_POSTCONDITION( validate() );

        return result;
    }

    _TryRemoveBackResult result;
    result._anyItemRemoved = true;
    result._erasedItemRemoved = false;

    AV_POSTCONDITION( validate() );

    return result;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_TryEraseFromStorageResult
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::tryEraseFromStorage(
    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_Storage::iterator pos
)
{
    std::pair< typename _Erased::iterator, bool > const insertInSortedResult
        = array::insert_in_sorted(
              _erased
            , typename _Storage::const_iterator( pos )
            , std::less< typename _Storage::const_iterator >()
        );

    if( _erased.full() )
    {
        mergeStorageWithErased();

        _TryEraseFromStorageResult result;
        result._inErased = _erased.end();
        result._isErased = true;
        result._isMerged = true;

        AV_POSTCONDITION( validate() );

        return result;
    }
    else
    {
        _TryEraseFromStorageResult result;
        result._inErased = insertInSortedResult.first;
        result._isErased = insertInSortedResult.second;
        result._isMerged = false;

        AV_POSTCONDITION( validate() );

        return result;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::isErased(
    typename AssocVector::_Storage::const_iterator iterator
)const
{
    typename _Erased::const_iterator const foundInErased = array::find_in_sorted(
          _erased.begin()
        , _erased.end()
        , iterator
        , std::less< typename _Storage::const_iterator >()
    );

    return foundInErased != _erased.end();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_FindImplResult
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::findImpl( _Key const & k )
{
    typename _Storage::iterator const greaterEqualInStorage
        = std::lower_bound( _storage.begin(), _storage.end(), k, value_comp() );

    bool const presentInStorage
        = greaterEqualInStorage != _storage.end()
        && key_comp()( k, greaterEqualInStorage->first ) == false;

    {// item is in storage, check in erased
        if( presentInStorage )
        {
            typename _Erased::iterator greaterEqualInErased = std::lower_bound(
                  _erased.begin()
                , _erased.end()
                , greaterEqualInStorage
                , std::less< typename _Storage::const_iterator >()
            );

            bool const itemNotMarkedAsErased
                = greaterEqualInErased == _erased.end()
                || std::less< typename _Storage::const_iterator >()( greaterEqualInStorage, * greaterEqualInErased );

            if( itemNotMarkedAsErased )
            {
                _FindImplResult result;
                result._inStorage = greaterEqualInStorage;
                result._inBuffer = 0;
                result._inErased = greaterEqualInErased;
                result._current = greaterEqualInStorage;

                AV_POSTCONDITION( result.validate() );

                return result;
            }
            else
            {
                _FindImplResult result;
                result._inStorage = 0;
                result._inBuffer = 0;
                result._inErased = 0;
                result._current = 0;

                AV_POSTCONDITION( result.validate() );

                return result;
            }
        }
    }

    {// check in buffer
        typename _Storage::iterator const greaterEqualInBuffer
            = std::lower_bound( _buffer.begin(), _buffer.end(), k, value_comp() );

        bool const presentInBuffer
            = greaterEqualInBuffer != _buffer.end()
            && key_comp()( k, greaterEqualInBuffer->first ) == false;

        if( presentInBuffer )
        {
            _FindImplResult result;
            result._inStorage = greaterEqualInStorage;
            result._inBuffer = greaterEqualInBuffer;
            result._inErased = 0;
            result._current = greaterEqualInBuffer;

            AV_POSTCONDITION( result.validate() );

            return result;
        }
        else
        {
            _FindImplResult result;
            result._inStorage = 0;
            result._inBuffer = 0;
            result._inErased = 0;
            result._current = 0;

            AV_POSTCONDITION( result.validate() );

            return result;
        }
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::find( _Key const & k )
{
    _FindImplResult const result = findImpl( k );

    if( result._current == 0 ){
        return end();
    }
    else
    {
        return iterator(
              this
            , result._inStorage
            , result._inBuffer
            , result._inErased
            , result._current
        );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::find( _Key const & k )const
{
    typedef AssocVector< _Key, _Mapped, _Cmp, _Allocator > * NonConstThis;

    return const_cast< NonConstThis >( this )->find( k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::lower_bound( _Key const & k )
{
    typename _Storage::iterator const greaterEqualInStorage
        = std::lower_bound( _storage.begin(), _storage.end(), k, value_comp() );

    typename _Storage::iterator const greaterEqualInBuffer
        = std::lower_bound( _buffer.begin(), _buffer.end(), k, value_comp() );

    return iterator( this, greaterEqualInStorage, greaterEqualInBuffer, 0, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::lower_bound( _Key const & k )const
{
    typedef AssocVector< _Key, _Mapped, _Cmp, _Allocator > * NonConstThis;

    return const_cast< NonConstThis >( this )->lower_bound( k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::upper_bound( _Key const & k )
{
    typename _Storage::iterator const greaterInStorage
        = std::upper_bound( _storage.begin(), _storage.end(), k, value_comp() );

    typename _Storage::iterator const greaterInBuffer
        = std::upper_bound( _buffer.begin(), _buffer.end(), k, value_comp() );

    return iterator( this, greaterInStorage, greaterInBuffer, 0, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::upper_bound( _Key const & k )const
{
    typedef AssocVector< _Key, _Mapped, _Cmp, _Allocator > * NonConstThis;

    return const_cast< NonConstThis >( this )->upper_bound( k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::pair<
      typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
    , typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::equal_range( _Key const & k )
{
    typename _Storage::iterator const greaterEqualInStorage
        = std::lower_bound( _storage.begin(), _storage.end(), k, value_comp() );

    bool const notPresentInStorage
        = greaterEqualInStorage == _storage.end()
        || key_comp()( k, greaterEqualInStorage->first );

    typename _Storage::iterator const greaterEqualInBuffer
        = std::lower_bound( _buffer.begin(), _buffer.end(), k, value_comp() );

    bool const notPresentInBuffer
        = greaterEqualInBuffer == _buffer.end()
        || key_comp()( k, greaterEqualInBuffer->first );

    if( notPresentInStorage == false )
    {
        return std::make_pair(
              iterator( this, greaterEqualInStorage, greaterEqualInBuffer, 0, 0 )
            , iterator( this, greaterEqualInStorage + 1, greaterEqualInBuffer, 0, 0 )
        );
    }

    if( notPresentInBuffer == false )
    {
        return std::make_pair(
              iterator( this, greaterEqualInStorage, greaterEqualInBuffer, 0, 0 )
            , iterator( this, greaterEqualInStorage, greaterEqualInBuffer + 1, 0, 0 )
        );
    }

    return std::make_pair(
          iterator( this, greaterEqualInStorage, greaterEqualInBuffer, 0, 0 )
        , iterator( this, greaterEqualInStorage, greaterEqualInBuffer, 0, 0 )
    );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::pair<
      typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
    , typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::const_iterator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::equal_range( _Key const & k )const
{
    typedef AssocVector< _Key, _Mapped, _Cmp, _Allocator > * NonConstThis;

    return const_cast< NonConstThis >( this )->equal_range( k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_find( _Key const & k )
{
    return _iterator( findImpl( k )._current );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_find( _Key const & k )const
{
    typedef AssocVector< _Key, _Mapped, _Cmp, _Allocator > * NonConstThis;

    return const_cast< NonConstThis >( this )->_find( k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::validateStorage()const
{
    if( util::is_sorted( _storage.begin(), _storage.end(), value_comp() ) == false )
    {
        AV_ERROR();

        return false;
    }

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::validateBuffer()const
{
    if( _buffer.empty() ){
        return true;
    }

    if( util::is_sorted( _buffer.begin(), _buffer.end(), value_comp() ) == false )
    {
        AV_ERROR();

        return false;
    }

    if(
        util::has_intersection(
              _buffer.begin()
            , _buffer.end()
            , _storage.begin()
            , _storage.end()
            , value_comp()
        )
    )
    {
        AV_ERROR();

        return false;
    }

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::validateErased()const
{
    if( _erased.empty() ){
        return true;
    }

    if(
        util::is_sorted(
              _erased.begin()
            , _erased.end()
            , std::less< typename _Storage::const_iterator >()
        ) == false
    )
    {
        AV_ERROR();

        return false;
    }

    AV_CHECK( _erased.front() >= _storage.begin() );
    AV_CHECK( _erased.back() < _storage.end() );

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::validate()const
{
    return validateStorage() && validateBuffer() && validateErased();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::merge()
{
    if( size() > _storage.capacity() )
    {
        reserve( calculateNewStorageCapacity( _storage.capacity() ) );

        return;
    }

    if( _erased.empty() == false ){
        mergeStorageWithErased();
    }

    mergeStorageWithBuffer();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::reference
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::operator[]( key_type const & k )
{
    return insert( value_type( k, mapped_type() ) ).first->second;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::count( key_type const & k )const
{
    return _find( k ) ? 1 : 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::erase( key_type const & k )
{
    typename _Storage::iterator const foundInStorage
        = array::find_in_sorted( _storage.begin(), _storage.end(), k, value_comp() );

    {//erase from _buffer
        if( foundInStorage == _storage.end() )
        {
            typename _Storage::iterator const foundInBuffer
                = array::find_in_sorted( _buffer.begin(), _buffer.end(), k, value_comp() );

            if( foundInBuffer == _buffer.end() )
            {
                AV_POSTCONDITION( validate() );

                return 0;
            }
            else
            {
                array::erase( _buffer, foundInBuffer );

                AV_POSTCONDITION( validate() );

                return 1;
            }
        }
    }

    {//erase from back
        _TryRemoveBackResult const result = tryRemoveStorageBack( foundInStorage );

        if( result._anyItemRemoved )
        {
            if( result._erasedItemRemoved )
            {
                AV_POSTCONDITION( validate() );

                return 0;
            }
            else
            {
                AV_POSTCONDITION( validate() );

                return 1;
            }
        }
    }

    {//erase from _storage
        bool const result = tryEraseFromStorage( foundInStorage )._isErased ? 1 : 0;

        AV_POSTCONDITION( validate() );

        return result;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::erase( iterator pos )
{
    if( pos == end() ){
        return end();
    }

    // iterator::base converts  : pair< T1, T2 > *       -> pair< T1 const, T2 > *
    // revert real iterator type: pair< T1 const, T2 > * -> pair< T1, T2 > *
    value_type_mutable * const posBase = reinterpret_cast< value_type_mutable * >( pos.base() );

    {//erase from _buffer
        if( util::is_between( _buffer.begin(), posBase, _buffer.end() ) )
        {
            _Key const key = pos->first;

            array::erase( _buffer, posBase );

            typename _Storage::iterator const greaterEqualInStorage
                = pos.getCurrentInStorage()
                ? pos.getCurrentInStorage()
                : std::lower_bound( _storage.begin(), _storage.end(), key, value_comp() );

            AV_POSTCONDITION( validate() );

            return iterator( this, greaterEqualInStorage, posBase, 0, 0 );
        }
    }

    {//item not present in container
        if( util::is_between( _storage.begin(), posBase, _storage.end() ) == false ){
            return end();
        }
    }

    {//erase from back
        _Key const key = pos->first;

        _TryRemoveBackResult const result = tryRemoveStorageBack( posBase );

        if( result._anyItemRemoved )
        {
            typename _Storage::iterator const greaterEqualInBuffer
                = pos.getCurrentInBuffer()
                ? pos.getCurrentInBuffer()
                : std::lower_bound( _buffer.begin(), _buffer.end(), key, value_comp() );

            if( greaterEqualInBuffer == _buffer.end() )
            {
                AV_POSTCONDITION( validate() );

                return end();
            }
            else
            {
                AV_POSTCONDITION( validate() );

                return iterator( this, _storage.end(), greaterEqualInBuffer, 0, 0 );
            }
        }
    }

    {//erase from _storage
        _Key const key = pos->first;

        _TryEraseFromStorageResult const result = tryEraseFromStorage( posBase );

        if( result._isErased == false ){
            return end();
        }

        typename _Storage::iterator const greaterEqualInBuffer
            = pos.getCurrentInBuffer()
            ? pos.getCurrentInBuffer()
            : std::lower_bound( _buffer.begin(), _buffer.end(), key, value_comp() );

        if( result._isMerged == false )
        {
            AV_POSTCONDITION( validate() );

            return iterator( this, posBase + 1, greaterEqualInBuffer, result._inErased + 1, 0 );
        }

        typename _Storage::iterator const greaterEqualInStorage
            = std::lower_bound( _storage.begin(), _storage.end(), key, value_comp() );

        AV_POSTCONDITION( validate() );

        return iterator( this, greaterEqualInStorage, greaterEqualInBuffer, _erased.end(), 0 );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_erase( iterator pos )
{
    // iterator::base converts  : pair< T1, T2 > *       -> pair< T1 const, T2 > *
    // revert real iterator type: pair< T1 const, T2 > * -> pair< T1, T2 > *
    value_type_mutable * const posBase = reinterpret_cast< value_type_mutable * >( pos.base() );

    {//erase from _buffer
        if( util::is_between( _buffer.begin(), posBase, _buffer.end() ) )
        {
            array::erase( _buffer, posBase );

            AV_POSTCONDITION( validate() );

            return true;
        }
    }

    {//item not present in container
        if( util::is_between( _storage.begin(), posBase, _storage.end() ) == false ){
            return false;
        }
    }

    {//erase from back
        _TryRemoveBackResult const result = tryRemoveStorageBack( posBase );

        if( result._anyItemRemoved )
        {
            AV_POSTCONDITION( validate() );

            return true;
        }
    }

    {//erase from _storage
        bool const result = tryEraseFromStorage( posBase )._isErased;

        AV_POSTCONDITION( validate() );

        return result;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::swap(
    AssocVector< _Key, _Mapped, _Cmp, _Allocator > & other
)
{
    std::swap( _storage, other._storage );
    std::swap( _buffer, other._buffer );
    std::swap( _erased, other._erased );

    std::swap( _cmp, other._cmp );

    std::swap( _allocator, other._allocator );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::mergeStorageWithBuffer()
{
    AV_PRECONDITION( _erased.empty() );

    array::move_merge( _storage, _buffer, value_comp() );

    util::destroy_range( _buffer.begin(), _buffer.end() );

    _buffer.setSize( 0 );

    AV_POSTCONDITION( _buffer.empty() );
    AV_POSTCONDITION( validateStorage() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::mergeStorageWithErased()
{
    typename _Storage::iterator const end = _storage.end();

    array::erase_removed( _storage, _erased );

    util::destroy_range( _storage.end(), end );

    _erased.setSize( 0 );

    AV_POSTCONDITION( _erased.empty() );
    AV_POSTCONDITION( validateStorage() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_FindOrInsertToBufferResult
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::findOrInsertToBuffer(
      _Key const & k
    , _Mapped const & m
)
{
    typename _Storage::iterator const greaterEqualInBuffer
        = std::lower_bound( _buffer.begin(), _buffer.end(), k, value_comp() );

    if( greaterEqualInBuffer != _buffer.end() )
    {
        bool const isEqual = _cmp( k, greaterEqualInBuffer->first ) == false;

        if( isEqual )
        {
            _FindOrInsertToBufferResult result;
            result._inBuffer = greaterEqualInBuffer;
            result._isInserted = false;
            result._isReallocated = false;

            return result;
        }
    }

    if( _buffer.full() )
    {
        merge();

        AV_CHECK( _buffer.empty() );

        array::insert( _buffer, _buffer.begin(), value_type_mutable( k, m ) );

        _FindOrInsertToBufferResult result;
        result._inBuffer = _buffer.begin();
        result._isInserted = true;
        result._isReallocated = true;

        AV_POSTCONDITION( validate() );

        return result;
    }
    else
    {
        array::insert( _buffer, greaterEqualInBuffer, value_type_mutable( k, m ) );

        _FindOrInsertToBufferResult result;
        result._inBuffer = greaterEqualInBuffer;
        result._isInserted = true;
        result._isReallocated = false;

        AV_POSTCONDITION( validate() );

        return result;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Allocator >::calculateNewBufferCapacity(
    std::size_t storageSize
)
{
    return static_cast< std::size_t >( 1.0 * sqrt( storageSize ));
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Allocator >::calculateNewErasedCapacity(
    std::size_t storageSize
)
{
    return calculateNewBufferCapacity( storageSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Allocator >::calculateNewStorageCapacity(
    std::size_t storageSize
)
{
    return 2 * storageSize;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void AssocVector< _Key, _Mapped, _Cmp, _Allocator >::dump( int width )const
{
    std::cout << "storage: ";
    for( unsigned i = 0 ; i < _storage.size() ; ++ i )
    {
        if( i > 0 && width > 0 && ( i % width == 0 ) ){
            std::cout << "\n         ";
        }

        std::cout << " (" << _storage[i].first << "," << _storage[i].second << ")";
    }

    std::cout << std::endl << "buffer:  ";
    for( unsigned i = 0 ; i < _buffer.size() ; ++ i )
    {
        if( i > 0 && width > 0 && ( i % width == 0 ) ){
            std::cout << "\n         ";
        }

        std::cout << " (" << _buffer[i].first << "," << _buffer[i].second << ")";
    }

    std::cout << std::endl << "erased:  ";
    for( unsigned i = 0 ; i < _erased.size() ; ++ i )
    {
        if( i > 0 && width > 0 && ( i % width == 0 ) ){
            std::cout << "\n         ";
        }

        std::cout << " (" << (*_erased[i]).first << "," << (*_erased[i]).second << ")";
    }

    std::cout << "." << std::endl;
}

#endif
