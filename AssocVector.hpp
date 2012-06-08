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

#define PRECONDITION( condition ) assert( condition );
#define POSTCONDITION( condition ) assert( condition );

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
    // isBetween
    //
    template<
          typename _T1
        , typename _T2
    >
    inline
    bool isBetween( _T1 const & begin, _T2 const & value, _T1 const & end )
    {
        PRECONDITION( begin <= end );

        return ( value < begin ) == false && ( end < value ) == false;
    }
}

namespace util
{
    //
    // destroyRange
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
            void destroy( _Ptr begin, _Ptr const end )
            {
                typedef typename std::iterator_traits< _Ptr >::value_type T;

                for( /*empty*/ ; begin != end ; ++ begin ){
                    begin -> T::~T();
                }
            }
        };
    }

    template< typename _Ptr >
    inline
    void destroyRange( _Ptr begin, _Ptr const end )
    {
        PRECONDITION( begin <= end );

        typedef typename std::iterator_traits< _Ptr >::value_type T;

        detail::DestroyRangeImpl< __has_trivial_destructor( T ) >::destroy( begin, end );
    }
}

namespace util
{
    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    _OutputPtr move_forward( _InputPtr first, _InputPtr last, _OutputPtr result )
    {
        for( /*empty*/ ; first != last ; ++ result , ++ first ){
            * result = AV_MOVE( * first );
        }

        return result;
    }

    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    _OutputPtr uninitialized_move_forward( _InputPtr first, _InputPtr last, _OutputPtr result )
    {
        for( /*empty*/ ; first != last ; ++result, ++first )
        {
            new ( static_cast< void * >(  result ) )
                typename std::iterator_traits< _OutputPtr >::value_type( AV_MOVE( * first ) );
        }

        return result;
    }

    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    _OutputPtr move_backward( _InputPtr first, _InputPtr last, _OutputPtr result )
    {
        while( first != last ){
            *( -- result ) = AV_MOVE( * ( -- last ) );
        }

        return result;
    }
}

namespace util
{
    //
    // moveRange
    //
    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    inline
    void moveRange(
          _InputPtr begin
        , _InputPtr end
        , _OutputPtr begin2
    )
    {
        if( begin < begin2 ){
            util::move_backward( begin, end, begin2 + ( end - begin ) );
        }
        else if( begin > begin2 ){
            util::move_forward( begin, end, begin2 );
        }
        else{
            // begin == begin2 -> do nothing
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
_Iterator last_less_equal( _Iterator first, _Iterator last, _T const & t, _Cmp cmp )
{
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

        bool const isEqual
            = cmp( * greaterEqual, t ) == false
            && cmp( t, * greaterEqual ) == false;

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
            _size = newSize;
        }

        std::size_t capacity()const{
            return _capacity;
        }

        std::size_t getCapacity()const{
            return capacity();
        }

        void setCapacity( std::size_t newCapacity ){
            _capacity = newCapacity;
        }

        value_type & front(){
            return _data[ 0 ];
        }

        value_type const & front()const{
            return _data[ 0 ];
        }

        value_type & back(){
            return _data[ _size - 1 ];
        }

        value_type const & back()const{
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
            return _data[ index ];
        }

        value_type const & operator[]( std::size_t index )const{
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
        , typename _Alloc
    >
    inline
    Array< _T > create(
            std::size_t capacity
          , _Alloc allocator
    )
    {
        PRECONDITION( capacity <= allocator.max_size() );

        Array< _T > result;

        void * const rawMemory = allocator.allocate( capacity );

        POSTCONDITION( rawMemory != 0 );

        result.setData( static_cast< _T * >( rawMemory ) );
        result.setSize( 0 );
        result.setCapacity( capacity );

        return result;
    }

    template<
          typename _T
        , typename _T2
        , typename _Alloc
    >
    inline
    void create(
          Array< _T > & dest
        , Array< _T2 > const & origin
        , _Alloc allocator
    )
    {
        PRECONDITION( dest.capacity() == 0 );
        PRECONDITION( dest.size() == 0 );
        PRECONDITION( dest.data() == 0 );

        dest = create< _T >( origin.capacity(), allocator );

        util::uninitialized_move_forward( origin.begin(), origin.end(), dest.begin() );

        dest.setSize( origin.size() );
        dest.setCapacity( origin.capacity() );
    }

    template< typename _T >
    inline
    void init( Array< _T > & array )
    {
        array.setData( 0 );
        array.setSize( 0 );
        array.setCapacity( 0 );
    }

    template<
          typename _T
        , typename _Alloc
    >
    inline
    void destroy(
          Array< _T > & array
        , _Alloc allocator
    )
    {
        util::destroyRange( array.begin(), array.end() );
        allocator.deallocate( array.getData(), array.capacity() );
    }

    template<
          typename _T
        , typename _Alloc
    >
    inline
    void
    reserve(
          Array< _T > & array
        , std::size_t capacity
        , _Alloc allocator
    )
    {
        std::size_t const size = array.size();

        if( capacity <= array.capacity() ){
            return;
        }

        Array< _T > newArray = array::create< _T >( capacity, allocator );

        util::uninitialized_move_forward( array.begin(), array.end(), newArray.begin() );
        array::destroy( array, allocator );

        array.setSize( size );
        array.setCapacity( capacity );
        array.setData( newArray.getData() );
    }

    template<
          typename _Iterator
        , typename _T
        , typename _Cmp
    >
    _Iterator
    findInSorted(
          _Iterator begin
        , _Iterator end
        , _T const & t
        , _Cmp cmp
    )
    {
        _Iterator const found = std::lower_bound( begin, end, t, cmp );

        if( found == end ){
            return end;
        }

        bool const isGreater = cmp( t, * found );

        if( isGreater ){
            return end;
        }

        return found;
    }

    template< typename _T >
    inline
    void insert(
          Array< _T > & array
        , typename Array< _T >::iterator pos
        , _T const & t
    )
    {
        PRECONDITION( array.size() + 1 <= array.capacity() );

        new ( static_cast< void * >( array.end() ) ) _T();

        if( pos != array.end() ){
            util::moveRange( pos, array.end(), pos + 1 );
        }

        * pos = t;

        array.setSize( array.size() + 1 );
    }

    template<
          typename _T
        , typename _Cmp
    >
    bool insertSorted(
          Array< _T > & array
        , _T const & t
        , _Cmp cmp
    )
    {
        PRECONDITION( array.size() + 1 <= array.capacity() );

        new ( static_cast< void * >( array.end() ) ) _T();

        typename Array< _T >::iterator const found = std::lower_bound(
              array.begin()
            , array.end()
            , t
            , cmp
        );

        if( found != array.end() )
        {
            bool const isEqual = cmp( t, * found ) == false;

            if( isEqual ){
                return false;
            }
        }

        insert( array, found, t );

        return true;
    }

    template< typename _T >
    inline
    void erase(
          Array< _T > & array
        , typename Array< _T >::iterator pos
    )
    {
        PRECONDITION( array.begin() <= pos );
        PRECONDITION( pos < array.end() );

        util::moveRange( pos + 1, array.end(), pos );
        array.setSize( array.size() - 1 );

        array.end() -> ~_T();
    }

    template< typename _T >
    void mergeWithErased(
          array::Array< _T > & storage
        , array::Array< typename array::Array< _T >::const_iterator > const & erased
    )
    {
        PRECONDITION( storage.size() >= erased.size() );

        if( erased.empty() ){
            return;
        }

        typedef typename array::Array< _T >::const_iterator StorageConstIterator;
        typedef typename array::Array< _T >::iterator StorageIterator;
        typedef typename array::Array< StorageConstIterator >::const_iterator ErasedConstIterator;

        StorageIterator currentInStorage = const_cast< StorageIterator >( erased.front() );
        StorageIterator const endInStorage = const_cast< StorageIterator >( storage.end() );

        StorageIterator whereInsertInStorage = const_cast< StorageIterator >( erased.front() );

        ErasedConstIterator currentInErased = erased.begin();
        ErasedConstIterator endInErased = erased.end();

        while( currentInStorage != endInStorage )
        {
            PRECONDITION( util::isBetween( storage.begin(), whereInsertInStorage, storage.end() ) );

            if(
                   currentInErased != endInErased
                && currentInStorage == * currentInErased
            )
            {
                ++ currentInStorage;
                ++ currentInErased;
            }
            else
            {
              _T & destination = * whereInsertInStorage;
              _T & source = * currentInStorage;

              destination = AV_MOVE( source );

              ++ whereInsertInStorage;
              ++ currentInStorage;
            }
        }

        POSTCONDITION( currentInErased == endInErased );

        storage.setSize( storage.size() - erased.size() );
    }

    template<
          typename _T
        , typename _Cmp
    >
    void merge(
          array::Array< _T > & storage
        , array::Array< _T > & buffer
        , _Cmp const & cmp = _Cmp()
    )
    {
        PRECONDITION( storage.size() + buffer.size() <= storage.capacity() );

        typedef typename array::Array< _T >::iterator Iterator;

        Iterator whereInsertInStorage = storage.begin() + storage.size() + buffer.size() - 1;

        Iterator currentInStorage = storage.begin() + storage.size() - 1;
        Iterator endInStorage = storage.begin() - 1;

        Iterator currentInBuffer = buffer.begin() + buffer.size() - 1;
        Iterator const endInBuffer = buffer.begin() - 1;

        std::size_t numberOfItemsToCreateByPlacementNew = buffer.size();

        while( currentInBuffer != endInBuffer )
        {
            if(
                   currentInStorage == endInStorage
                || cmp( * currentInStorage, * currentInBuffer )
            )
            {
                if( numberOfItemsToCreateByPlacementNew != 0 )
                {
                    new ( static_cast< void * >( whereInsertInStorage ) )
                        _T( AV_MOVE( * currentInBuffer ) );

                    numberOfItemsToCreateByPlacementNew -= 1;
                }
                else
                {
                    * whereInsertInStorage = AV_MOVE( * currentInBuffer );
                }

                -- currentInBuffer;
                -- whereInsertInStorage;
            }
            else
            {
                if( numberOfItemsToCreateByPlacementNew != 0 )
                {
                    new ( static_cast< void * >( whereInsertInStorage ) )
                        _T( AV_MOVE( * currentInStorage ) );

                    numberOfItemsToCreateByPlacementNew -= 1;
                }
                else
                {
                    * whereInsertInStorage = AV_MOVE( * currentInStorage );
                }

                -- currentInStorage;
                -- whereInsertInStorage;
            }
        }

        storage.setSize( storage.size() + buffer.size() );
    }
}

namespace util
{

template<
      typename _Iterator1
    , typename _Iterator2
    , typename _IteratorOutput
    , typename _Cmp
>
_IteratorOutput
move_merge_into_uninitialized(
      _Iterator1 begin1
    , _Iterator1 end1
    , _Iterator2 begin2
    , _Iterator2 end2
    , _IteratorOutput output
    , _Cmp cmp = _Cmp()
)
{
    while( begin1 != end1 && begin2 != end2 )
    {
        if( cmp( * begin1, * begin2 ) )
        {
            new ( static_cast< void * >( output ) )
                typename std::iterator_traits< _IteratorOutput >::value_type( AV_MOVE( * begin1 ) );

            ++ output;
            ++ begin1;
        }
        else{
            new ( static_cast< void * >( output ) )
                typename std::iterator_traits< _IteratorOutput >::value_type( AV_MOVE( * begin2 ) );

            ++ output;
            ++ begin2;
        }
    }

    if( begin1 == end1 ){
        return uninitialized_move_forward( begin2, end2, output );
    }

    if( begin2 == end2 ){
        return uninitialized_move_forward( begin1, end1, output );
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
            , _current( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorIterator( AssocVectorIterator< _Iter, _Container > const & other )
            : _container( other.getContainer() )

            , _currentInStorage( other.getCurrentInStorage() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _currentInErased( other.getCurrentInErased() )

            , _current( other.getCurrent() )
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
                _current = calculateCurrent();
            }
            else
            {
                _current = current;
            }
        }

        AssocVectorIterator &
        operator=( AssocVectorIterator const & other )
        {
            _container = other._container;

            _currentInStorage =  other._currentInStorage;
            _currentInBuffer = other._currentInBuffer;
            _currentInErased = other._currentInErased;

            _current = other._current;

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
            if( _current == 0 ){
                return * this;
            }
            else if( _current == _currentInStorage ){
                ++ _currentInStorage;
            }
            else{
                ++ _currentInBuffer;
            }

            _current = calculateCurrent();

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
            else if( _current == 0 )
            {
                -- _currentInStorage;
                -- _currentInBuffer;
            }
            else
            {
                if( _current == _currentInStorage ){
                    -- _currentInStorage;
                }
                else{
                    -- _currentInBuffer;
                }
            }

            _current = calculateCurrent();

            return * this;
        }

        AssocVectorIterator operator--( int )
        {
            AssocVectorIterator result( * this );

            ( * this ).operator--();


            return result;
        }

        reference operator*()const{
            PRECONDITION( _current != 0 );

            return * base();
        }

        pointer operator->()const{
            PRECONDITION( _current != 0 );

            return base();
        }

        pointer base()const{
            PRECONDITION( _current != 0 );

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

        // public for copy constructor only : Iterator -> ConstIterator
        _Container const * getContainer()const{ return _container; }

        pointer_mutable getCurrentInStorage()const{ return _currentInStorage; }
        pointer_mutable getCurrentInBuffer()const{ return _currentInBuffer; }
        typename _Container::_Erased::const_iterator getCurrentInErased()const{ return _currentInErased; }

        pointer_mutable getCurrent()const{ return _current; }

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

        AssocVectorReverseIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _container( 0 )

            , _currentInStorage( 0 )
            , _currentInBuffer( 0 )

            , _current( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorReverseIterator( AssocVectorReverseIterator< _Iter, _Container > const & other )
            : _container( other.getContainer() )

            , _currentInStorage( other.getCurrentInStorage() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _currentInErased( other.getCurrentInErased() )

            , _current( other.getCurrent() )
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
                _current = calculateCurrentReverse();
            }
            else{
                _current = current;
            }
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

        bool operator==( AssocVectorReverseIterator const & other )const{
            return equal( *this, other );
        }

        bool operator!=( AssocVectorReverseIterator const & other )const{
            return ! ( ( * this ) == other );
        }

        AssocVectorReverseIterator & operator++()
        {
            if( _current == 0 ){
                return * this;
            }
            else if( _current == _currentInStorage ){
                -- _currentInStorage;
            }
            else{
                -- _currentInBuffer;
            }

            _current = calculateCurrentReverse();

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
            else if( _current == 0 )
            {
                ++ _currentInStorage;
                ++ _currentInBuffer;
            }
            else
            {
                if( _current == _currentInStorage ){
                    ++ _currentInStorage;
                }
                else{
                    ++ _currentInBuffer;
                }
            }

            _current = calculateCurrentReverse();

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
                        reinterpret_cast< void const * >( _current )
                    )
                );
        }

        // public for copy constructor only : Iterator -> ConstIterator
        _Container const * getContainer()const{ return _container; }

        pointer_mutable getCurrentInStorage()const{ return _currentInStorage; }
        pointer_mutable getCurrentInBuffer()const{ return _currentInBuffer; }
        typename _Container::_Erased::const_iterator getCurrentInErased()const{ return _currentInErased; }

        pointer_mutable getCurrent()const{ return _current; }

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

        pointer_mutable _current;
    };

} // namespace detail

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp = std::less< _Key >
    , typename _Alloc = std::allocator< std::pair< _Key, _Mapped > >
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
    typedef _Alloc allocator_type;

    typedef util::CmpByFirst< value_type_mutable, _Cmp > value_compare;

    typedef detail::AssocVectorIterator< value_type_mutable *, AssocVector > iterator;
    typedef detail::AssocVectorIterator< value_type_mutable const *, AssocVector > const_iterator;

    typedef detail::AssocVectorReverseIterator< value_type_mutable *, AssocVector > reverse_iterator;
    typedef detail::AssocVectorReverseIterator< value_type_mutable const *, AssocVector > const_reverse_iterator;

    typedef array::Array< value_type_mutable > _Storage;

    typedef array::Array< typename _Storage::const_iterator > _Erased;

public:
    //
    // Memory Management
    //
    explicit AssocVector( _Cmp const & cmp = _Cmp(), _Alloc const & allocator = _Alloc() );

    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & other );

#ifdef AV_CXX11X_RVALUE_REFERENCE
    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Alloc > && other );
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

    //
    // _insert, faster, non STL compatible version on find
    //
    bool _insert( value_type const & value );

    template< typename _Iterator >
    inline void insert( _Iterator begin, _Iterator end );

    //
    // find
    //
    iterator find( key_type const & k );
    const_iterator find( key_type const & k )const;

    //
    // _find - faster, non STL compatible version of find
    //
    bool _find( key_type const & k )const;

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


    key_compare key_comp()const{ return _cmp; }
    value_compare value_comp()const{ return value_compare( _cmp ); }

    void merge();

private:
    //
    // find
    //
    typename _Storage::iterator find( _Storage & container, key_type const & k );
    typename _Storage::const_iterator find( _Storage const & container, key_type const & k )const;

    //
    // merge
    //
    void mergeStorageWithBuffer();
    void mergeStorageWithErased();
    void reserveStorageAndMergeWithBuffer( std::size_t storageCapacity );

    //
    // insert
    //
    void pushBack( key_type const & k, mapped_type const & m );

    std::pair< typename _Storage::iterator, bool >
    findOrInsertToBuffer( key_type const & k, mapped_type const & m );

    //
    // isErased
    //
    bool isErased( typename _Storage::const_iterator iterator )const;

    //
    // getAllocator (method specialization)
    //
    _Alloc getAllocator( _Storage const & ){ return _allocator; }

    typename _Alloc::template rebind< typename _Storage::const_iterator >::other
    getAllocator( _Erased const & )
    {return typename _Alloc::template rebind< typename _Storage::const_iterator >::other( _allocator );}

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

    _Alloc _allocator;
};

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool operator==(
      AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & lhs
    , AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & rhs
)
{
    if( lhs.size() != rhs.size() ){
        return false;
    }

    typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator begin = lhs.begin();
    typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator const end = lhs.end();

    typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator begin2 = rhs.begin();

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
    , typename _Alloc
>
bool operator!=(
      AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & lhs
    , AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & rhs
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
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::AssocVector(
      _Cmp const & cmp
    , _Alloc const & allocator
)
    : _cmp( cmp )
    , _allocator( allocator )
{
    array::init( _storage );
    array::init( _buffer );
    array::init( _erased );

    std::size_t const defaultSize = 2*(2*2);

    reserve( defaultSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::AssocVector(
    AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & other
)
    : _cmp( other._cmp )
    , _allocator( other._allocator )
{
    array::init( _storage );
    array::create< value_type_mutable >( _storage, other._storage, getAllocator( _storage ) );

    array::init( _buffer );
    array::create< value_type_mutable >( _buffer, other._buffer, getAllocator( _buffer ) );

    array::init( _erased );
    array::create< typename _Storage::const_iterator >( _erased, other._erased, getAllocator( _erased ) );
}

#ifdef AV_CXX11X_RVALUE_REFERENCE

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::AssocVector(
    AssocVector< _Key, _Mapped, _Cmp, _Alloc > && other
)
    : _storage( other._storage )
    , _buffer( other._buffer )
    , _erased( other._erased )
    , _cmp( other._cmp )
    , _allocator( other._allocator )
{
    array::init( other._storage );
    array::init( other._buffer );
    array::init( other._erased );
}

#endif

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::~AssocVector()
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
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::clear()
{
    util::destroyRange( _storage.begin(), _storage.end() );
    util::destroyRange( _buffer.begin(), _buffer.end() );

    _storage.setSize( 0 );
    _buffer.setSize( 0 );
    _erased.setSize( 0 );

}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc > &
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::operator=( AssocVector const & other )
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
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc > &
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::operator=( AssocVector && other )
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
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reserve( std::size_t newStorageCapacity )
{
    if( newStorageCapacity <= _storage.capacity() ){
        return;
    }

    if( _erased.empty() == false ){
        mergeStorageWithErased();
    }

    reserveStorageAndMergeWithBuffer( newStorageCapacity );

    array::reserve(
          _erased
        , calculateNewErasedCapacity( newStorageCapacity )
        , getAllocator( _erased )
    );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::begin()
{
    return iterator( this, _storage.begin(), _buffer.begin(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rbegin()
{
    return reverse_iterator( this, _storage.end() - 1, _buffer.end() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::begin()const
{
    return const_iterator( this, _storage.begin(), _buffer.begin(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rbegin()const
{
    return const_reverse_iterator( this, _storage.end() - 1, _buffer.end() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::end()
{
    return iterator( this, _storage.end(), _buffer.end(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rend()
{
    return reverse_iterator( this, _storage.begin() - 1, _buffer.begin() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::end()const
{
    return const_iterator( this, _storage.end(), _buffer.end(), 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rend()const
{
    return const_reverse_iterator( this, _storage.begin() - 1, _buffer.begin() - 1, 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::empty()const
{
    return size() == 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::size()const
{
    return _storage.size() + _buffer.size() - _erased.size();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::pair< typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator, bool >
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( value_type const & value )
{
    _Key const & k = value.first;
    _Mapped const & m = value.second;

    {//push back to storage
        if( _storage.empty() || _cmp( _storage.back().first, k ) )
        {
            pushBack( k, m );

            return std::make_pair(
                  iterator( this, _storage.end() - 1, _buffer.end(), _storage.end() - 1 )
                , true
            );
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
            typename _Storage::iterator const storageEnd1 = _storage.end();
            std::pair< typename _Storage::iterator, bool > const pair = findOrInsertToBuffer( k, m );
            typename _Storage::iterator const storageEnd2 = _storage.end();

            typename _Storage::iterator greaterEqualInStorage2 = 0;

            {// check if 'findOrInsertToBuffer' did reallocation
                if( storageEnd1 == storageEnd2 )
                {// no reallocation, 'greaterEqualInStorage' is still valid
                    greaterEqualInStorage2 = greaterEqualInStorage;
                }
                else
                {// reallocation done, search for 'greaterEqualInStorage' again
                    greaterEqualInStorage2 = std::lower_bound(
                          _storage.begin()
                        , _storage.end()
                        , k
                        , value_comp()
                    );
                }
            }

            POSTCONDITION( greaterEqualInStorage2 != 0 );

            return std::make_pair(
                  iterator( this, greaterEqualInStorage2, pair.first, pair.first )
                , pair.second
            );
        }
    }

    {// check if not erased
        typename _Erased::iterator const foundInErased = array::findInSorted(
              _erased.begin()
            , _erased.end()
            , greaterEqualInStorage
            , std::less< typename _Storage::const_iterator >()
        );

        {// item is in storage and is not marked as erased
            if( foundInErased == _erased.end() )
            {
                typename _Storage::iterator lessEqualInBuffer = util::last_less_equal(
                      _buffer.begin()
                    , _buffer.end()
                    , value_type_mutable( k, mapped_type() )
                    , value_comp()
                );

                return std::make_pair(
                      iterator( this, greaterEqualInStorage, lessEqualInBuffer, greaterEqualInStorage )
                    , false
                );
            }
        }

        {// item is in storage but is marked as erased
            array::erase( _erased, foundInErased );

            greaterEqualInStorage->second = m;

            typename _Storage::iterator lessEqualInBuffer = util::last_less_equal(
                  _buffer.begin()
                , _buffer.end()
                , value_type_mutable( k, mapped_type() )
                , value_comp()
            );

            return std::make_pair(
                  iterator( this, greaterEqualInStorage, lessEqualInBuffer, greaterEqualInStorage )
                , true
            );
        }
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_insert( value_type const & value )
{
    _Key const & k = value.first;
    _Mapped const & m = value.second;

    {//push back to storage
        if( _storage.empty() || _cmp( _storage.back().first, k ) )
        {
            pushBack( k, m );
            return true;
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
        if( notPresentInStorage ){
            return findOrInsertToBuffer( k, m ).second;
        }
    }

    {// check if not erased
        typename _Erased::iterator const foundInErased = array::findInSorted(
              _erased.begin()
            , _erased.end()
            , greaterEqualInStorage
            , std::less< typename _Storage::const_iterator >()
        );

        {// item is in storage and is not marked as erased
            if( foundInErased == _erased.end() )
            {
                return false;
            }
        }

        {// item is in storage but is marked as erased
            array::erase( _erased, foundInErased );

            greaterEqualInStorage->second = m;

            return true;
        }
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
template<
    typename _Iterator
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( _Iterator const begin, _Iterator const end )
{
    for( _Iterator current = begin ; current != end ; ++ current ){
        insert( * current );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::pushBack( _Key const & k, _Mapped const & m )
{
    PRECONDITION(
        (
               _storage.size() == 0
            || _cmp( _storage.back().first, k )
        )
    );

    if( _storage.size() == _storage.capacity() ){
        reserve( calculateNewStorageCapacity( _storage.capacity() ) );
    }

    {//push back
        new ( _storage.end() ) value_type_mutable( k, m );

        _storage.setSize( _storage.size() + 1 );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::isErased(
    typename AssocVector::_Storage::const_iterator iterator
)const
{
    typename _Erased::const_iterator const foundInErased = array::findInSorted(
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
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::find( _Key const & k )
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
            if( isErased( greaterEqualInStorage ) ){
                return end();
            }

            typename _Storage::iterator const greaterEqualInBuffer = std::lower_bound(
                  _buffer.begin()
                , _buffer.end()
                , k
                , value_comp()
            );

            return iterator(
                  this
                , greaterEqualInStorage
                , greaterEqualInBuffer
                , 0
            );
        }
    }

    {// check in buffer
        typename _Storage::iterator const foundInBuffer = find( _buffer, k );

        if( foundInBuffer == _buffer.end() ){
            return end();
        }

        return iterator(
              this
            , greaterEqualInStorage
            , foundInBuffer
            , 0
        );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_find( _Key const & k )const
{
    typename _Storage::const_iterator const foundInStorage = find( _storage, k );

    {// item is in storage, check in erased
        if( foundInStorage != _storage.end() )
        {
            if( isErased( foundInStorage ) ){
                return false;
            }

            return true;
        }
    }

    {// check in buffer
        typename _Storage::const_iterator const foundInBuffer = find( _buffer, k );

        if( foundInBuffer == _buffer.end() ){
            return false;
        }

        return true;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::find( _Key const & k )const
{
    typename _Storage::const_iterator const greaterEqualInStorage = std::lower_bound(
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
            if( isErased( greaterEqualInStorage ) ){
                return end();
            }

            typename _Storage::const_iterator const greaterEqualInBuffer = std::lower_bound(
                  _buffer.begin()
                , _buffer.end()
                , k
                , value_comp()
            );

            return const_iterator(
                  this
                , greaterEqualInStorage
                , greaterEqualInBuffer
                , 0
            );
        }
    }

    {// check in buffer
        typename _Storage::const_iterator const foundInBuffer = find( _buffer, k );

        if( foundInBuffer == _buffer.end() ){
            return end();
        }

        return const_iterator(
              this
            , greaterEqualInStorage
            , foundInBuffer
            , 0
        );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::merge()
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
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reference
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::operator[]( key_type const & k )
{
    {//push back to storage
        if( _storage.empty() || _cmp( _storage.back().first, k ) ){
            return pushBack( k, mapped_type() ), _storage.back().second;
        }
    }

    typename _Storage::iterator const foundInStorage = find( _storage, k );

    {//find or insert to buffer
        if( foundInStorage == _storage.end() ){
            return findOrInsertToBuffer( k, mapped_type() ).first->second;
        }
    }

    typename _Erased::iterator const foundInErased = array::findInSorted(
          _erased.begin()
        , _erased.end()
        , foundInStorage
        , std::less< typename _Storage::const_iterator >()
    );

    {// item is already in storage
        if( foundInErased == _erased.end() ){
            return foundInStorage->second;
        }
    }

    {// item is in storage but is also marked as erased
        array::erase( _erased, foundInErased );

        foundInStorage->second = mapped_type();

        return foundInStorage->second;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::count( key_type const & k )const{
    return _find( k ) ? 1 : 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::erase( key_type const & k )
{
    typename _Storage::iterator const foundInStorage = find( _storage, k );

    {//erase from _buffer
        if( foundInStorage == _storage.end() )
        {
            typename _Storage::iterator const foundInBuffer = find( _buffer, k );

            if( foundInBuffer == _buffer.end() )
            {
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
        if( foundInStorage + 1 == _storage.end() )
        {
            getAllocator( _storage ).destroy( foundInStorage );

            _storage.setSize( _storage.size() - 1 );

            if( _erased.back() == foundInStorage ){
                _erased.setSize( _erased.size() - 1 );

                return 0;
            }
            else{
                return 1;
            }
        }
    }

    {//erase from _storage
        bool const result = array::insertSorted(
              _erased
            , typename _Storage::const_iterator( foundInStorage )
            , std::less< typename _Storage::const_iterator >()
        );

        if( _erased.full() ){
            mergeStorageWithErased();
        }

        return result;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::erase( iterator pos )
{
    // iterator::base converts  : pair< T1, T2 > *       -> pair< T1 const, T2 > *
    // revert real iterator type: pair< T1 const, T2 > * -> pair< T1, T2 > *
    value_type_mutable * const posBase = reinterpret_cast< value_type_mutable * >( pos.base() );

    {//erase from _buffer
        if( util::isBetween( _buffer.begin(), posBase, _buffer.end() ) ){
            return array::erase( _buffer, posBase );
        }
    }

    {//item not present in container
        if( util::isBetween( _storage.begin(), posBase, _storage.end() ) == false ){
            return;
        }
    }

    {//erase from back
        if( posBase + 1 == _storage.end() )
        {
            getAllocator( _storage ).destroy( posBase );

            _storage.setSize( _storage.size() - 1 );

            if( _erased.back() == posBase ){
                _erased.setSize( _erased.size() - 1 );
            }
            
            return;
        }
    }
    
    {//erase from _storage
        array::insertSorted(
              _erased
            , typename _Storage::const_iterator( posBase )
            , std::less< typename _Storage::const_iterator >()
        );

        if( _erased.full() ){
            mergeStorageWithErased();
        }
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::swap(
    AssocVector< _Key, _Mapped, _Cmp, _Alloc > & other
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
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::find(
      _Storage & container
    , key_type const & k
)
{
    return array::findInSorted( container.begin(), container.end(), k, value_comp() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reserveStorageAndMergeWithBuffer(
    std::size_t storageCapacity
)
{
    PRECONDITION( _erased.empty() )

    std::size_t const bufferCapacity = calculateNewBufferCapacity( storageCapacity );

    _Storage newStorage = array::create< value_type_mutable >( storageCapacity, getAllocator( _storage ) );
    _Storage newBuffer = array::create< value_type_mutable >( bufferCapacity, getAllocator( _buffer ) );

    util::move_merge_into_uninitialized(
          _storage.begin()
        , _storage.end()
        , _buffer.begin()
        , _buffer.end()
        , newStorage.begin()
        , value_comp()
    );

    {
        std::size_t const size = _storage.size();

        array::destroy( _storage, getAllocator( _storage ) );

        _storage.setData( newStorage.getData() );
        _storage.setSize( size + _buffer.size() );
        _storage.setCapacity( storageCapacity );
    }

    {
        array::destroy( _buffer, getAllocator( _buffer ) );

        _buffer.setData( newBuffer.getData() );
        _buffer.setSize( 0 );
        _buffer.setCapacity( bufferCapacity );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::mergeStorageWithBuffer()
{
    array::merge( _storage, _buffer, value_comp() );

    util::destroyRange( _buffer.begin(), _buffer.end() );

    _buffer.setSize( 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::mergeStorageWithErased()
{
    typename _Storage::iterator const end = _storage.end();

    array::mergeWithErased( _storage, _erased );

    util::destroyRange( _storage.end(), end );

    _erased.setSize( 0 );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::find(
      _Storage const & container
    , key_type const & k
)const
{
    return array::findInSorted( container.begin(), container.end(), value_type_mutable( k, mapped_type() ), value_comp() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::pair< typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator, bool >
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findOrInsertToBuffer(
      _Key const & k
    , _Mapped const & m
)
{
    typename _Storage::iterator const greaterEqual =
        std::lower_bound(
              _buffer.begin()
            , _buffer.end()
            , k
            , value_comp()
        );

    if( greaterEqual != _buffer.end() )
    {
        bool const isEqual = _cmp( k, greaterEqual->first ) == false;

        if( isEqual ){
            return std::make_pair( greaterEqual, false );
        }
    }

    if( _buffer.full() )
    {
        merge();

        PRECONDITION( _buffer.empty() );

        array::insert( _buffer, _buffer.begin(), value_type_mutable( k, m ) );

        return std::make_pair( _buffer.begin(), true );
    }
    else
    {
        array::insert( _buffer, greaterEqual, value_type_mutable( k, m ) );

        return std::make_pair( greaterEqual, true );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::calculateNewBufferCapacity(
    std::size_t storageSize
)
{
    return static_cast< std::size_t >( sqrt( storageSize ));
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::calculateNewErasedCapacity(
    std::size_t storageSize
)
{
    return calculateNewBufferCapacity( storageSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::calculateNewStorageCapacity(
    std::size_t storageSize
)
{
    return 2 * storageSize;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::dump()const
{
    std::cout << "storage" << std::endl;
    for( int i = 0 ; i < _storage.size() ; ++ i )
        std::cout << " (" << _storage[i].first << "," << _storage[i].second << ")";

    std::cout << std::endl << "buffer" << std::endl;
    for( int i = 0 ; i < _buffer.size() ; ++ i )
        std::cout << " (" << _buffer[i].first << "," << _buffer[i].second << ")";

    std::cout << std::endl << "erased" << std::endl;
    for( int i = 0 ; i < _erased.size() ; ++ i )
        std::cout << " (" << (*_erased[i]).first << "," << (*_erased[i]).second << ")";

    std::cout << std::endl;
}

#endif
