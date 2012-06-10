#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

// includes.begin

#include <algorithm>
#include <cassert>
#include <functional>
#include <cmath>

// includes.end

// configuration.begin

#if( _MSC_VER >= 1600 )
    #define AV_CXX11X_RVALUE_REFERENCE
#endif

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    #if ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 3 )
      #define AV_CXX11X_RVALUE_REFERENCE
    #endif
#endif

#ifdef AV_CXX11X_RVALUE_REFERENCE
  #define AV_MOVE std::move
#else
  #define AV_MOVE
#endif

// configuration.end

#define AV_PRECONDITION( condition ) assert( ( condition ) );
#define AV_CHECK( condition ) assert( ( condition ) );
#define AV_POSTCONDITION( condition ) assert( ( condition ) );

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

        bool operator()( _Pair const & lhs, _Pair const & rhs )const{
            return _cmp( lhs.first, rhs.first );
        }

        bool operator()( _Pair const & pair, typename _Pair::first_type const & value )const{
            return _cmp( pair.first, value );
        }

        bool operator()( typename _Pair::first_type const & value, _Pair const & pair )const{
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
                typedef typename std::iterator_traits< _Ptr >::value_type T;

                for( /*empty*/ ; first != last ; ++ first )
                {
                    AV_PRECONDITION( first != 0 );

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
        AV_PRECONDITION( first <= last );

        typedef typename std::iterator_traits< _Ptr >::value_type T;

        detail::DestroyRangeImpl< __has_trivial_destructor( T ) >::destroy( first, last );
    }
}

namespace util
{
    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    _OutputPtr
    move_forward( _InputPtr first, _InputPtr last, _OutputPtr output )
    {
        AV_PRECONDITION( first <= last );

        for( /*empty*/ ; first != last ; ++ output , ++ first ){
            * output = AV_MOVE( * first );
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
            AV_PRECONDITION( first != 0 );
            AV_PRECONDITION( output != 0 );

            new ( static_cast< void * >(  output ) )
                typename std::iterator_traits< _OutputPtr >::value_type( AV_MOVE( * first ) );
        }

        return output;
    }

    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    _OutputPtr
    move_backward( _InputPtr first, _InputPtr last, _OutputPtr output )
    {
        AV_PRECONDITION( first <= last );

        while( first != last ){
            -- output;
            -- last;

            * output = AV_MOVE( * last );
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
        AV_PRECONDITION( first <= last );

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

    _Iterator greaterEqual  = std::lower_bound(
          first
        , last
        , t
        , cmp
    );

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

        const_iterator begin()const{
            return _data;
        }

        iterator end(){
            return _data + _size;
        }

        const_iterator end()const{
            return _data + _size;
        }

        bool empty()const{
            return _size == 0;
        }

        bool full()const{
            return _size == _capacity;
        }

        std::size_t size()const{
            return _size;
        }

        std::size_t getSize()const{
            return size();
        }

        void setSize( std::size_t newSize ){
            AV_PRECONDITION( _data != 0 || newSize == 0 );

            _size = newSize;
        }

        std::size_t capacity()const{
            return _capacity;
        }

        std::size_t getCapacity()const{
            return capacity();
        }

        void setCapacity( std::size_t newCapacity ){
            AV_PRECONDITION( _data != 0 || newCapacity == 0 );

            _capacity = newCapacity;
        }

        value_type & front(){
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ 0 ];
        }

        value_type const & front()const{
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ 0 ];
        }

        value_type & back(){
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ _size - 1 ];
        }

        value_type const & back()const{
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( empty() == false );

            return _data[ _size - 1 ];
        }

        _T * const data()const{
            return _data;
        }

        _T * const getData()const{
            return data();
        }

        _T * data(){
            return _data;
        }

        _T * getData(){
            return data();
        }

        void setData( _T * t ){
            _data = t;
        }

        value_type & operator[]( std::size_t index ){
            AV_PRECONDITION( _data != 0 );
            AV_PRECONDITION( index < size() );

            return _data[ index ];
        }

        value_type const & operator[]( std::size_t index )const{
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

        AV_POSTCONDITION( rawMemory != 0 );

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

        std::size_t const size = array.size();

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
    bool
    insert_in_sorted(
          Array< _T > & array
        , _T const & t
        , _Cmp cmp
    )
    {
        AV_PRECONDITION( array.size() + 1 <= array.capacity() );

        typename Array< _T >::iterator const greaterEqual = std::lower_bound(
              array.begin()
            , array.end()
            , t
            , cmp
        );

        if( greaterEqual != array.end() )
        {
            bool const isEqual = cmp( t, * greaterEqual ) == false;

            if( isEqual ){
                return false;
            }
        }

        insert( array, greaterEqual, t );

        return true;
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
              ( * whereInsertInStorage ) = AV_MOVE( * currentInStorage );

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

        // todo: unroll loop
        while( rCurrentInBuffer != rEndInBuffer )
        {
            if(
                   rCurrentInStorage == rEndInStorage
                || cmp( * rCurrentInStorage, * rCurrentInBuffer )
            )
            {
                if( numberOfItemsToCreateByPlacementNew != 0 )
                {
                    new ( static_cast< void * >( rWhereInsertInStorage ) )
                        _T( AV_MOVE( * rCurrentInBuffer ) );

                    numberOfItemsToCreateByPlacementNew -= 1;
                }
                else
                {
                    * rWhereInsertInStorage = AV_MOVE( * rCurrentInBuffer );
                }

                -- rCurrentInBuffer;
                -- rWhereInsertInStorage;
            }
            else
            {
                if( numberOfItemsToCreateByPlacementNew != 0 )
                {
                    new ( static_cast< void * >( rWhereInsertInStorage ) )
                        _T( AV_MOVE( * rCurrentInStorage ) );

                    numberOfItemsToCreateByPlacementNew -= 1;
                }
                else
                {
                    * rWhereInsertInStorage = AV_MOVE( * rCurrentInStorage );
                }

                -- rCurrentInStorage;
                -- rWhereInsertInStorage;
            }
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
        AV_PRECONDITION( first1 != 0 );
        AV_PRECONDITION( first2 != 0 );
        AV_PRECONDITION( output != 0 );

        if( cmp( * first1, * first2 ) )
        {
            new ( static_cast< void * >( output ) )
                typename std::iterator_traits< _OutputPtr >::value_type( AV_MOVE( * first1 ) );

            ++ output;
            ++ first1;
        }
        else{
            new ( static_cast< void * >( output ) )
                typename std::iterator_traits< _OutputPtr >::value_type( AV_MOVE( * first2 ) );

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
        if(
               lhs.getContainer()->storage().getData() != rhs.getContainer()->storage().getData()
            || lhs.getContainer()->buffer().getData() != rhs.getContainer()->buffer().getData()
        ){
            return false;
        }

        // for empty container returns that begin == end
        // despite on fact they are not
        if(
                lhs.getContainer()->storage().size() == 0
             && rhs.getContainer()->storage().size() == 0
             && lhs.getContainer()->storage().size() == 0
             && rhs.getContainer()->storage().size() == 0
        ){
            return true;
        }

        return lhs.getCurrent() == rhs.getCurrent();
    }

    //
    // AssocVectorIterator
    //
    template<
          typename _Iterator
        , typename _Container
    >
    struct AssocVectorIterator
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

        AssocVectorIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _container( 0 )
            , _currentInStorage( 0 )
            , _currentInBuffer( 0 )
            , _currentPosition( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorIterator( AssocVectorIterator< _Iter, _Container > const & other )
            : _container( other.getContainer() )

            , _currentInStorage( other.getCurrentInStorage() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _currentInErased( other.getCurrentInErased() )

            , _currentPosition( other.getCurrent() )
        {
        }

        AssocVectorIterator(
              _Container const * container
            , pointer_mutable currentInStorage
            , pointer_mutable currentInBuffer
            , pointer_mutable current
        )
            : _container( container )

            , _currentInStorage( currentInStorage )
            , _currentInBuffer( currentInBuffer )
        {
            _currentInErased = std::lower_bound(
                  _container->erased().begin()
                , _container->erased().end()
                , _currentInStorage
                , std::less< typename _Container::_Storage::const_iterator >()
            );

            if( current == 0 )
            {
                _currentPosition = calculateCurrent();
            }
            else
            {
                _currentPosition = current;
            }
        }

        AssocVectorIterator &
        operator=( AssocVectorIterator const & other )
        {
            _container = other._container;

            _currentInStorage =  other._currentInStorage;
            _currentInBuffer = other._currentInBuffer;
            _currentInErased = other._currentInErased;

            _currentPosition = other._currentPosition;

            return * this;
        }

        bool operator==( AssocVectorIterator const & other )const{
            return equal( *this, other );
        }

        bool operator!=( AssocVectorIterator const & other )const{
            return ! ( ( * this ) == other );
        }

        AssocVectorIterator & operator++()
        {
            if( _currentPosition == 0 ){
                return * this;
            }
            else if( _currentPosition == _currentInStorage ){
                ++ _currentInStorage;
            }
            else{
                ++ _currentInBuffer;
            }

            _currentPosition = calculateCurrent();

            return * this;
        }

        AssocVectorIterator operator++( int )
        {
            AssocVectorIterator result( * this );

            ( * this ).operator++();

            return result;
        }

        AssocVectorIterator & operator--()
        {
            if(
                   _currentInStorage == _container->storage().begin()
                && _currentInBuffer == _container->buffer().begin()
            ){
                return * this;
            }
            else if( _currentPosition == 0 )
            {
                -- _currentInStorage;
                -- _currentInBuffer;
            }
            else
            {
                if( _currentPosition == _currentInStorage ){
                    -- _currentInStorage;
                }
                else{
                    -- _currentInBuffer;
                }
            }

            _currentPosition = calculateCurrent();

            return * this;
        }

        AssocVectorIterator operator--( int )
        {
            AssocVectorIterator result( * this );

            ( * this ).operator--();


            return result;
        }

        reference operator*()const{
            AV_PRECONDITION( _currentPosition != 0 );

            return * base();
        }

        pointer operator->()const{
            AV_PRECONDITION( _currentPosition != 0 );

            return base();
        }

        pointer base()const{
            AV_PRECONDITION( _currentPosition != 0 );

            // make key const
            // pair< T1, T2 > * -> pair< T1 const, T2 > *
            //return reinterpret_cast< pointer >( _currentPosition );

            return
                reinterpret_cast< pointer >(
                    const_cast< void * >(
                        reinterpret_cast< void const * >( _currentPosition )
                    )
                );
        }

        // public for copy constructor only : Iterator -> ConstIterator
        _Container const * getContainer()const{ return _container; }

        pointer_mutable getCurrentInStorage()const{ return _currentInStorage; }
        pointer_mutable getCurrentInBuffer()const{ return _currentInBuffer; }
        typename _Container::_Erased::const_iterator getCurrentInErased()const{ return _currentInErased; }

        pointer_mutable getCurrent()const{ return _currentPosition; }

    private:
        pointer_mutable
        calculateCurrent()
        {
           while(
                   _currentInErased != _container->erased().end()
                && _currentInStorage != _container->storage().end()
                && _currentInStorage == *_currentInErased
            )
            {
                ++ _currentInStorage;
                ++ _currentInErased;
            }

            if( _currentInStorage == _container->storage().end() )
            {
                if( _currentInBuffer == _container->buffer().end() ){
                    return 0;
                }

                return _currentInBuffer;
            }

            if(
                   _currentInBuffer == _container->buffer().end()
                || _container->value_comp()( * _currentInStorage, * _currentInBuffer )
            )
            {
                return _currentInStorage;
            }
            else
            {
                return _currentInBuffer;
            }
        }

    private:
        _Container const * _container;

        pointer_mutable _currentInStorage;
        pointer_mutable _currentInBuffer;

        typename _Container::_Erased::const_iterator _currentInErased;

        pointer_mutable _currentPosition;
    };

    //
    // _AssocVectorIterator, simplified version of AssocVectorIterator, works with _find and _end
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

        _AssocVectorIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _currentPosition( 0 )
        {
        }

        template< typename _Iter >
        _AssocVectorIterator( _AssocVectorIterator< _Iter, _Container > const & other )
            : _currentPosition( other.getCurrent() )
        {
        }

        _AssocVectorIterator( pointer_mutable current )
            : _currentPosition( current )
        {
        }

        _AssocVectorIterator &
        operator=( _AssocVectorIterator const & other )
        {
            _currentPosition = other._currentPosition;

            return * this;
        }

        bool operator==( _AssocVectorIterator const & other )const{
            return _currentPosition == other.getCurrent();
        }

        bool operator!=( _AssocVectorIterator const & other )const{
            return ! ( ( * this ) == other );
        }

        reference operator*()const{
            AV_PRECONDITION( _currentPosition != 0 );

            return * base();
        }

        pointer operator->()const{
            AV_PRECONDITION( _currentPosition != 0 );

            return base();
        }

        pointer base()const{
            AV_PRECONDITION( _currentPosition != 0 );

            // make key const
            // pair< T1, T2 > * -> pair< T1 const, T2 > *
            //return reinterpret_cast< pointer >( _currentPosition );

            return
                reinterpret_cast< pointer >(
                    const_cast< void * >(
                        reinterpret_cast< void const * >( _currentPosition )
                    )
                );
        }

        operator bool()const
        {
            return _currentPosition != 0;
        }

        // public for copy constructor only : Iterator -> ConstIterator
        pointer_mutable getCurrent()const{ return _currentPosition; }

    private:
        pointer_mutable _currentPosition;
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

        AssocVectorReverseIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _container( 0 )

            , _currentInStorage( 0 )
            , _currentInBuffer( 0 )

            , _currentPosition( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorReverseIterator( AssocVectorReverseIterator< _Iter, _Container > const & other )
            : _container( other.getContainer() )

            , _currentInStorage( other.getCurrentInStorage() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _currentInErased( other.getCurrentInErased() )

            , _currentPosition( other.getCurrent() )
        {
        }

        AssocVectorReverseIterator(
              _Container const * container
            , pointer_mutable currentInStorage
            , pointer_mutable currentInBuffer
            , pointer_mutable current
        )
            : _container( container )

            , _currentInStorage( currentInStorage )
            , _currentInBuffer( currentInBuffer )
        {
            _currentInErased  = util::last_less_equal(
                      _container->erased().begin()
                    , _container->erased().end()
                    , _currentInStorage
                    , std::less< typename _Container::_Storage::const_iterator >()
                );

            if( _currentInErased == _container->erased().end() ){
                _currentInErased = _container->erased().begin() - 1;
            }

            if( current == 0 ){
                _currentPosition = calculateCurrentReverse();
            }
            else{
                _currentPosition = current;
            }
        }

        AssocVectorReverseIterator & operator=( AssocVectorReverseIterator const & other )
        {
            _container = other._container;

            _currentInStorage =  other._currentInStorage;
            _currentInBuffer = other._currentInBuffer;
            _currentInErased = other._currentInErased;

            _currentPosition = other._currentPosition;

            return * this;
        }

        bool operator==( AssocVectorReverseIterator const & other )const{
            return equal( *this, other );
        }

        bool operator!=( AssocVectorReverseIterator const & other )const{
            return ! ( ( * this ) == other );
        }

        AssocVectorReverseIterator & operator++()
        {
            if( _currentPosition == 0 ){
                return * this;
            }
            else if( _currentPosition == _currentInStorage ){
                -- _currentInStorage;
            }
            else{
                -- _currentInBuffer;
            }

            _currentPosition = calculateCurrentReverse();

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
                   _currentInStorage == ( _container->storage().end() - 1 )
                && _currentInBuffer == ( _container->buffer().end() - 1 )
            ){
                return * this;
            }
            else if( _currentPosition == 0 )
            {
                ++ _currentInStorage;
                ++ _currentInBuffer;
            }
            else
            {
                if( _currentPosition == _currentInStorage ){
                    ++ _currentInStorage;
                }
                else{
                    ++ _currentInBuffer;
                }
            }

            _currentPosition = calculateCurrentReverse();

            return * this;
        }

        AssocVectorReverseIterator operator--( int )
        {
            AssocVectorReverseIterator result( * this );

            ( * this ).operator--();

            return result;
        }

        reference operator*()const{
            return * base();
        }

        pointer operator->()const{
            return base();
        }

        pointer base()const{
            return
                reinterpret_cast< pointer >(
                    const_cast< void * >(
                        reinterpret_cast< void const * >( _currentPosition )
                    )
                );
        }

        // public for copy constructor only : Iterator -> ConstIterator
        _Container const * getContainer()const{ return _container; }

        pointer_mutable getCurrentInStorage()const{ return _currentInStorage; }
        pointer_mutable getCurrentInBuffer()const{ return _currentInBuffer; }
        typename _Container::_Erased::const_iterator getCurrentInErased()const{ return _currentInErased; }

        pointer_mutable getCurrent()const{ return _currentPosition; }

    private:
        pointer_mutable
        calculateCurrentReverse()
        {
           while(
                   _currentInErased != ( _container->erased().begin() - 1 )
                && _currentInStorage != ( _container->storage().begin() - 1 )
                && _currentInStorage == * _currentInErased
            )
            {
                -- _currentInStorage;
                -- _currentInErased;
            }

            if( _currentInStorage == ( _container->storage().begin() - 1 ) )
            {
                if( _currentInBuffer == (_container->buffer().begin() - 1 ) ){
                    return 0;
                }

                return _currentInBuffer;
            }
            else if( _currentInBuffer == ( _container->buffer().begin() - 1 ) )
            {
                return _currentInStorage;
            }
            else if( _container->value_comp()( * _currentInStorage, * _currentInBuffer ) == false )
            {
                return _currentInStorage;
            }

            return _currentInBuffer;
        }

    private:
        _Container const * _container;

        pointer_mutable _currentInStorage;
        pointer_mutable _currentInBuffer;
        typename _Container::_Erased::const_iterator _currentInErased;

        pointer_mutable _currentPosition;
    };

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

    typedef detail::AssocVectorIterator< value_type_mutable *, AssocVector > iterator;
    typedef detail::AssocVectorIterator< value_type_mutable const *, AssocVector > const_iterator;

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
        typename _Storage::iterator _positionInBuffer;
        bool _isInserted;
        bool _isReallocated;
    };

    struct _TryToRemoveBackResult
    {
        bool _anyItemRemoved;
        bool _erasedItemRemoved;

    };

    struct _FindImplResult
    {
        typename _Storage::iterator _positionInStorage;
        typename _Storage::iterator _positionInBuffer;
        typename _Storage::iterator _currentPosition;
    };

    struct _InsertImplResult
    {
        bool _isInserted;

        typename _Storage::iterator _positionInStorage;
        typename _Storage::iterator _positionInBuffer;
        typename _Storage::iterator _currentPosition;
    };

public:
    //
    // Memory Management
    //
    explicit AssocVector( _Cmp const & cmp = _Cmp(), _Allocator const & allocator = _Allocator() );

    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Allocator > const & other );

#ifdef AV_CXX11X_RVALUE_REFERENCE
    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Allocator > && other );
#endif

    inline ~AssocVector();
    inline void clear();

    AssocVector & operator=( AssocVector const & other );

#ifdef AV_CXX11X_RVALUE_REFERENCE
    AssocVector & operator=( AssocVector && other );
#endif

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

    template< typename _Iterator >
    inline void insert( _Iterator begin, _Iterator end );

    //
    // find
    //
    iterator find( key_type const & k );
    const_iterator find( key_type const & k )const;

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
    void erase( iterator pos );

    //
    // observers
    //
    key_compare key_comp()const{ return _cmp; }
    value_compare value_comp()const{ return value_compare( _cmp ); }

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
    // extension, faster, non STL compatible version on insert
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

private:

    //
    // merge
    //
    void mergeStorageWithBuffer();
    void mergeStorageWithErased();

    //
    // insert
    //
    bool tryToPushBack( key_type const & k, mapped_type const & m );

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
    _TryToRemoveBackResult
    tryToRemoveStorageBack( typename _Storage::iterator pos );

    //
    // eraseFromStorage
    //
    bool eraseFromStorage( typename _Storage::iterator pos );

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
    void dump()const;

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
){
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

#ifdef AV_CXX11X_RVALUE_REFERENCE

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

#endif

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

#ifdef AV_CXX11X_RVALUE_REFERENCE

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
AssocVector< _Key, _Mapped, _Cmp, _Allocator > &
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::operator=( AssocVector && other )
{
    AssocVector temp( AV_MOVE( other ) );
    temp.swap( * this );

    return * this;
}

#endif

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

    AV_CHECK( _buffer.empty() );
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
    return iterator( this, _storage.begin(), _buffer.begin(), 0 );
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
    return reverse_iterator( this, _storage.end() - 1, _buffer.end() - 1, 0 );
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
    return const_iterator( this, _storage.begin(), _buffer.begin(), 0 );
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
    return const_reverse_iterator( this, _storage.end() - 1, _buffer.end() - 1, 0 );
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
    return iterator( this, _storage.end(), _buffer.end(), 0 );
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
    return reverse_iterator( this, _storage.begin() - 1, _buffer.begin() - 1, 0 );
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
    return const_iterator( this, _storage.end(), _buffer.end(), 0 );
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
    return const_reverse_iterator( this, _storage.begin() - 1, _buffer.begin() - 1, 0 );
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

    if( result._positionInStorage == 0 )
    {
        typename _Storage::iterator const greaterEqualInStorage = std::lower_bound(
              _storage.begin()
            , _storage.end()
            , value.first
            , value_comp()
        );

        return std::make_pair(
              iterator(
                  this
                , greaterEqualInStorage
                , result._positionInBuffer
                , result._currentPosition
              )
            , result._isInserted
        );
    }
    else if( result._positionInBuffer == 0 )
    {
        typename _Storage::iterator const greaterEqualInBuffer = std::lower_bound(
              _buffer.begin()
            , _buffer.end()
            , value.first
            , value_comp()
        );

        return std::make_pair(
              iterator(
                  this
                , result._positionInStorage
                , greaterEqualInBuffer
                , result._currentPosition
              )
            , result._isInserted
        );
    }
    else
    {
        return std::make_pair(
              iterator(
                  this
                , result._positionInStorage
                , result._positionInBuffer
                , result._currentPosition
              )
            , result._isInserted
        );
    }
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
        if( tryToPushBack( k, m ) )
        {
            _InsertImplResult result;
            result._isInserted = true;
            result._positionInStorage = ( _storage.end() - 1 );
            result._positionInBuffer = 0;
            result._currentPosition = ( _storage.end() - 1 );

            return result;
        }
    }

    typename _Storage::iterator const greaterEqualInStorage = std::lower_bound(
          _storage.begin()
        , _storage.end()
        , k
        , value_comp()
    );

    bool const notPresentInStorage
        = greaterEqualInStorage == _storage.end()
        || key_comp()( k, greaterEqualInStorage->first );

    {//find or insert to buffer
        if( notPresentInStorage )
        {
            _FindOrInsertToBufferResult const findOrInsertToBufferResult
                = findOrInsertToBuffer( k, m );

            _InsertImplResult result;
            result._isInserted = findOrInsertToBufferResult._isInserted;

            if( findOrInsertToBufferResult._isReallocated ){
                result._positionInStorage = 0;
            }
            else{
                result._positionInStorage = greaterEqualInStorage;
            }

            result._positionInBuffer = findOrInsertToBufferResult._positionInBuffer;
            result._currentPosition = findOrInsertToBufferResult._positionInBuffer;

            return result;
        }
    }

    {// check if not erased
        typename _Erased::iterator const foundInErased = array::find_in_sorted(
              _erased.begin()
            , _erased.end()
            , greaterEqualInStorage
            , std::less< typename _Storage::const_iterator >()
        );

        {// item is in storage and is not marked as erased
            if( foundInErased == _erased.end() )
            {
                _InsertImplResult result;
                result._isInserted = false;
                result._positionInStorage = greaterEqualInStorage;
                result._positionInBuffer = 0;
                result._currentPosition = greaterEqualInStorage;

                return result;
            }
        }

        {// item is in storage but is marked as erased
            array::erase( _erased, foundInErased );

            greaterEqualInStorage->second = m;

            _InsertImplResult result;
            result._isInserted = true;
            result._positionInStorage = greaterEqualInStorage;
            result._positionInBuffer = 0;
            result._currentPosition = greaterEqualInStorage;

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
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::tryToPushBack( _Key const & k, _Mapped const & m )
{
    bool pushBackToStorage = false;

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

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_TryToRemoveBackResult
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::tryToRemoveStorageBack(
    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_Storage::iterator pos
)
{
    if( pos + 1 != _storage.end() ){
        _TryToRemoveBackResult result;
        result._anyItemRemoved = false;
        result._erasedItemRemoved = false;

        return result;
    }

    getAllocator( _storage ).destroy( pos );

    _storage.setSize( _storage.size() - 1 );

    if(
           _erased.empty() == false
        && _erased.back() == pos
    ){
        _erased.setSize( _erased.size() - 1 );

        _TryToRemoveBackResult result;
        result._anyItemRemoved = true;
        result._erasedItemRemoved = true;

        return result;
    }

    _TryToRemoveBackResult result;
    result._anyItemRemoved = true;
    result._erasedItemRemoved = false;

    return result;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::eraseFromStorage(
    typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_Storage::iterator pos
)
{
    bool const result = array::insert_in_sorted(
          _erased
        , typename _Storage::const_iterator( pos )
        , std::less< typename _Storage::const_iterator >()
    );

    if( _erased.full() ){
        mergeStorageWithErased();
    }

    return result;
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
    typename _Storage::iterator const greaterEqualInStorage = std::lower_bound(
          _storage.begin()
        , _storage.end()
        , k
        , value_comp()
    );

    bool const presentInStorage
        = greaterEqualInStorage != _storage.end()
        && key_comp()( k, greaterEqualInStorage->first ) == false;

    {// item is in storage, check in erased
        if( presentInStorage )
        {
            if( isErased( greaterEqualInStorage ) )
            {
                _FindImplResult result;
                result._positionInStorage = 0;
                result._positionInBuffer = 0;
                result._currentPosition = 0;

                return result;
            }
            else
            {
                _FindImplResult result;
                result._positionInStorage = greaterEqualInStorage;
                result._positionInBuffer = 0;
                result._currentPosition = greaterEqualInStorage;

                return result;
            }
        }
    }

    {// check in buffer
        typename _Storage::iterator const greaterEqualInBuffer = std::lower_bound(
              _buffer.begin()
            , _buffer.end()
            , k
            , value_comp()
        );

        bool const presentInBuffer
            = greaterEqualInBuffer != _buffer.end()
            && key_comp()( k, greaterEqualInBuffer->first ) == false;

        if( presentInBuffer )
        {
            _FindImplResult result;
            result._positionInStorage = greaterEqualInStorage;
            result._positionInBuffer = greaterEqualInBuffer;
            result._currentPosition = greaterEqualInBuffer;

            return result;
        }
        else
        {
            _FindImplResult result;
            result._positionInStorage = 0;
            result._positionInBuffer = 0;
            result._currentPosition = 0;

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

    if( result._currentPosition == 0 ){
        return end();
    }
    else if( result._positionInBuffer == 0 )
    {
        typename _Storage::iterator const greaterEqualInBuffer = std::lower_bound(
              _buffer.begin()
            , _buffer.end()
            , k
            , value_comp()
        );

        return iterator(
              this
            , result._positionInStorage
            , greaterEqualInBuffer
            , result._currentPosition
        );
    }
    else
    {
        return iterator(
              this
            , result._positionInStorage
            , result._positionInBuffer
            , result._currentPosition
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
typename AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_iterator
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::_find( _Key const & k )
{
    return _iterator( findImpl( k )._currentPosition );
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
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::merge()
{
    if( size() > _storage.capacity() ){
        return reserve( calculateNewStorageCapacity( _storage.capacity() ) );
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
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::count( key_type const & k )const{
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

            if( foundInBuffer == _buffer.end() ){
                return 0;
            }
            else
            {
                array::erase( _buffer, foundInBuffer );

                return 1;
            }
        }
    }

    {//erase from back
        _TryToRemoveBackResult const result = tryToRemoveStorageBack( foundInStorage );

        if( result._anyItemRemoved )
        {
            if( result._erasedItemRemoved ){
                return 0;
            }
            else{
                return 1;
            }
        }
    }

    {//erase from _storage
        return eraseFromStorage( foundInStorage );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Allocator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Allocator >::erase( iterator pos )
{
    // iterator::base converts  : pair< T1, T2 > *       -> pair< T1 const, T2 > *
    // revert real iterator type: pair< T1 const, T2 > * -> pair< T1, T2 > *
    value_type_mutable * const posBase = reinterpret_cast< value_type_mutable * >( pos.base() );

    {//erase from _buffer
        if( util::is_between( _buffer.begin(), posBase, _buffer.end() ) ){
            return array::erase( _buffer, posBase );
        }
    }

    {//item not present in container
        if( util::is_between( _storage.begin(), posBase, _storage.end() ) == false ){
            return;
        }
    }

    {//erase from back
        _TryToRemoveBackResult const result = tryToRemoveStorageBack( posBase );

        if( result._anyItemRemoved ){
            return;
        }
    }

    {//erase from _storage
        eraseFromStorage( posBase );
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
    array::move_merge( _storage, _buffer, value_comp() );

    util::destroy_range( _buffer.begin(), _buffer.end() );

    _buffer.setSize( 0 );
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
    typename _Storage::iterator const greaterEqualInBuffer =
        std::lower_bound(
              _buffer.begin()
            , _buffer.end()
            , k
            , value_comp()
        );

    if( greaterEqualInBuffer != _buffer.end() )
    {
        bool const isEqual = _cmp( k, greaterEqualInBuffer->first ) == false;

        if( isEqual )
        {
            _FindOrInsertToBufferResult result;
            result._positionInBuffer = greaterEqualInBuffer;
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
        result._positionInBuffer = _buffer.begin();
        result._isInserted = true;
        result._isReallocated = true;

        return result;
    }
    else
    {
        array::insert( _buffer, greaterEqualInBuffer, value_type_mutable( k, m ) );

        _FindOrInsertToBufferResult result;
        result._positionInBuffer = greaterEqualInBuffer;
        result._isInserted = true;
        result._isReallocated = false;

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
    return static_cast< std::size_t >( sqrt( storageSize ));
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
void AssocVector< _Key, _Mapped, _Cmp, _Allocator >::dump()const
{
    std::cout << "storage: ";
    for( int i = 0 ; i < _storage.size() ; ++ i )
        std::cout << " (" << _storage[i].first << "," << _storage[i].second << ")";

    std::cout << std::endl << "buffer: ";
    for( int i = 0 ; i < _buffer.size() ; ++ i )
        std::cout << " (" << _buffer[i].first << "," << _buffer[i].second << ")";

    std::cout << std::endl << "erased: ";
    for( int i = 0 ; i < _erased.size() ; ++ i )
        std::cout << " (" << (*_erased[i]).first << "," << (*_erased[i]).second << ")";

    std::cout << std::endl;
}

#endif
