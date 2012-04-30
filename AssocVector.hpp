#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#include <algorithm>
#include <functional>
#include <cmath>

#define PRECONDITION( condition ) assert( condition );
#define POSTCONDITION( condition ) assert( condition );
#define INVARIANT( condition ) assert( condition );

namespace util
{
    template< typename _Iterator >
    void dump( _Iterator begin, _Iterator end, std::ostream & out = std::cout )
    {
        out << '[';

        if( begin != end )
        {
            _Iterator current = begin;

            out << '(' << current->first << ',' << current->second << ')';

            ++ current;

            for( ; current != end ; ++ current ){
                out << '(' << current->first << ',' << current->second << ')';
            }
        }

        out << ']';
    }
}

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
    bool isBetween( _T1 const & begin, _T2 const & value, _T1 const & end ){
        PRECONDITION( begin <= end );

        return ( value < begin ) == false && ( end < value ) == false;
    }
}

namespace util
{
    //
    // destroy_range
    //
    template< typename _Ptr >
    void destroy_range( _Ptr begin, _Ptr const end )
    {
        PRECONDITION( begin <= end );

        typedef typename std::iterator_traits< _Ptr >::value_type T;

        for( /*empty*/ ; begin != end ; ++ begin ){
            begin -> T::~T();
        }
    }

    //
    // construct_range
    //
    template< typename _Ptr >
    void construct_range( _Ptr begin, _Ptr const end )
    {
        PRECONDITION( begin <= end );

        typedef typename std::iterator_traits< _Ptr >::value_type T;

        for( /*empty*/ ; begin != end ; ++ begin ){
            new ( begin ) T();
        }
    }
}

namespace util
{
    //
    // copyRange
    //
    template<
          typename _InputPtr
        , typename _OutputPtr
    >
    void
    copyRange(
          _InputPtr begin
        , _InputPtr end
        , _OutputPtr begin2
    )
    {
        assert( begin <= end );

        if( begin < begin2 ){
            std::copy_backward( begin, end, begin2 + ( end - begin ) );
        }
        else if( begin > begin2 ){
            std::copy( begin, end, begin2 );
        }
        else{
            // begin == begin2 -> do not copy
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
    _Iterator greaterEqual
        = std::lower_bound(
              first
            , last
            , t
            , cmp
        );

    if( greaterEqual == last ){
        return last;
    }

    // lower_bound returns first greater_than/equal_to but we need last less_than
    bool const isEqual = cmp( * greaterEqual, t ) == false;

    if( isEqual ){
        // that is OK, request item was found
        return greaterEqual;
    }

    // we need to go one item backward

    // 6 8 10 13 17 19 20 21 22 24
    // lower_bound( 23 ):       ^
    // requested:            ^

    return -- greaterEqual;
}

}

namespace array
{
    //
    // Array
    //
    template< typename _T >
    struct Array
    {
        typedef _T value_type;

        typedef _T * iterator;
        typedef _T const * const_iterator;

        _T * _data;

        std::size_t _capacity;
        std::size_t _size;

        iterator begin(){
            return _data;
        }

        const_iterator begin()const{
            return _data;
        }

        iterator rbegin(){
            return _data + _size - 1;
        }

        const_iterator rbegin()const{
            return _data + _size - 1;
        }

        iterator end(){
            return _data + _size;
        }

        const_iterator end()const{
            return _data + _size;
        }

        iterator rend(){
            return _data - 1;
        }

        const_iterator rend()const{
            return _data - 1;
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

        std::size_t capacity()const{
            return _capacity;
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

        value_type & operator[]( std::size_t index ){
            return _data[ index ];
        }

        value_type const & operator[]( std::size_t index )const{
            return _data[ index ];
        }
    };

    template< typename _Alloc >
    Array< typename _Alloc::value_type >
    create(
            std::size_t capacity
          , _Alloc allocator
    )
    {
        Array< typename _Alloc::value_type > result;

        result._data
            = static_cast< typename _Alloc::value_type * >( allocator.allocate( capacity ) );

        INVARIANT( result._data != 0 );

        result._size = 0;
        result._capacity = capacity;

        util::construct_range( result._data, result._data + capacity );

        return result;
    }

    template<
          typename _T
        , typename _T2
        , typename _Alloc
    >
    void
    create(
          Array< _T > & dest
        , Array< _T2 > const & origin
        , _Alloc allocator
    )
    {
        PRECONDITION( dest._capacity == 0 );
        PRECONDITION( dest._size == 0 );
        PRECONDITION( dest._data == 0 );

        dest = create( origin.capacity(), allocator );

        util::copyRange( origin.begin(), origin.end(), dest.begin() );

        dest._size = origin.size();
        dest._capacity = origin.capacity();
    }

    template<
          typename _T
        , typename _Alloc
    >
    void
    destroy(
          Array< _T > & array
        , _Alloc allocator
    )
    {
        util::destroy_range( array.begin(), array.end() );
        allocator.deallocate( array._data, array.capacity() );

        array._data = 0;
        array._size = 0;
        array._capacity = 0;
    }

    template< typename _T >
    void
    init( Array< _T > & array )
    {
        array._data = 0;
        array._size = 0;
        array._capacity = 0;
    }

    template<
          typename _T
        , typename _Alloc
    >
    void
    reserve(
          Array< _T > & container
        , std::size_t capacity
        , _Alloc allocator
    )
    {
        if( capacity <= container.capacity() ){
            return;
        }

        std::size_t const size = container.size();

        Array< _T > newArray = array::create( capacity, allocator );

        util::copyRange( container.begin(), container.end(), newArray.begin() );

        array::destroy( container, allocator );

        container._size = size;
        container._capacity = capacity;
        container._data = newArray._data;
    }

    template< typename _T >
    void dump( Array< _T > const & array, std::ostream & out = std::cout )
    {
        dump( array.begin(), array.end(), out );
    }

    template<
          typename _Array
        , typename _Iterator
        , typename _T
        , typename _Cmp
    >
    _Iterator
    findInSortedImpl(
          _Array array
        , _T const t
        , _Cmp cmp
    )
    {
        _Iterator const found
            = std::lower_bound(
                  array.begin()
                , array.end()
                , t
                , cmp
            );

        if( found == array.end() ){
            return array.end();
        }

        bool const isGreater = cmp( t, * found );

        if( isGreater ){
            return array.end();
        }
        else{
            return found;
        }
    }

    template<
          typename _T
        , typename _Cmp
    >
    typename Array< _T >::iterator
    findInSorted(
          Array< _T > & array
        , _T const t
        , _Cmp cmp = std::less< _T >()
    )
    {
        return findInSortedImpl< Array< _T >, typename Array< _T >::iterator >( array, t, cmp );
    }

    template<
          typename _T
        , typename _Cmp
    >
    typename Array< _T >::const_iterator
    findInSorted(
          Array< _T > const & array
        , _T const t
        , _Cmp cmp = std::less< _T >()
    )
    {
        return findInSortedImpl< Array< _T > const, typename Array< _T >::const_iterator >( array, t, cmp );
    }

    template< typename _T >
    void insert(
          Array< _T > & array
        , typename Array< _T >::iterator pos
        , _T const & t
    )
    {
        PRECONDITION( array.size() + 1 <= array.capacity() );

        std::size_t const size= array.size();

        if( pos != array.end() ){
            util::copyRange( pos, array.end(), pos + 1 );
        }

        * pos = t;

        array._size += 1;
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

        std::size_t const size = array.size();

        typename Array< _T >::iterator const found
            = std::lower_bound(
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

        INVARIANT( size == array.size() );

        insert( array, found, t );

        return true;
    }

    template< typename _T >
    void erase( Array< _T > & array, typename Array< _T >::iterator pos )
    {
        PRECONDITION( array.begin() <= pos );
        PRECONDITION( pos < array.end() );

        std::size_t const size = array.size();

        util::copyRange( pos + 1, array.end(), pos );
        array._size -= 1;
    }

    template< typename _T >
    void mergeWithErased(
          array::Array< _T > & storage
        , array::Array< typename array::Array< _T >::const_iterator > & erased
    )
    {
        PRECONDITION( storage.size() >= erased.size() );

        if( erased.empty() ){
            return;
        }

        typedef typename array::Array< _T >::const_iterator StorageConstIterator;
        typedef typename array::Array< _T >::iterator StorageIterator;

        typedef typename array::Array<
            typename array::Array< _T >::const_iterator
        >::const_iterator ErasedConstIterator;

        StorageConstIterator currentInStorage = erased.front();
        INVARIANT( util::isBetween( storage.begin(), currentInStorage, storage.end() ) );

        StorageConstIterator const endInStorage = storage.end();

        StorageIterator whereInsertInStorage = const_cast< StorageIterator >( erased.front() );
        INVARIANT( util::isBetween( storage.begin(), whereInsertInStorage, storage.end() ) );

        ErasedConstIterator currentInErased = erased.begin();
        ErasedConstIterator endInErased = erased.end();

        while( currentInStorage != endInStorage )
        {
            INVARIANT( util::isBetween( storage.begin(), whereInsertInStorage, storage.end() ) );

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
              * whereInsertInStorage = * currentInStorage;

              ++ whereInsertInStorage;
              ++ currentInStorage;
            }
        }

        INVARIANT( currentInErased == endInErased );

        storage._size -= erased.size();
        erased._size = 0;
    }

    template< typename _T >
    void mergeWithErased(
          array::Array< _T > & storage
        , array::Array< typename array::Array< _T >::const_iterator > & erased
        , array::Array< _T > & newStorage
    )
    {
        PRECONDITION( storage.size() >= erased.size() );

        typedef typename array::Array< _T >::iterator StorageIterator;
        typedef typename array::Array< _T >::const_iterator StorageConstIterator;

        typedef typename array::Array<
            typename array::Array< _T >::const_iterator
        >::const_iterator ErasedConstIterator;

        StorageConstIterator currentInStorage = storage.begin();
        StorageConstIterator const endInStorage = storage.end();

        StorageIterator whereInsertInStorage = newStorage.begin();

        ErasedConstIterator currentInErased = erased.begin();
        ErasedConstIterator endInErased = erased.end();

        while( currentInStorage != endInStorage )
        {
            INVARIANT( util::isBetween( storage.begin(), whereInsertInStorage, storage.end() ) );

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
              * whereInsertInStorage = * currentInStorage;

              ++ whereInsertInStorage;
              ++ currentInStorage;
            }
        }

        newStorage._size = storage.size() - erased.size();

        erased._size = 0;
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

        std::size_t const storageSize = storage.size();
        std::size_t const bufferSize = buffer.size();

        typedef typename array::Array< _T >::iterator Iterator;

        Iterator whereInsertInStorage = storage.begin() + storage.size() + buffer.size() - 1;

        Iterator currentInStorage = storage.begin() + storage.size() - 1;
        Iterator endInStorage = storage.begin() - 1;

        Iterator currentInBuffer = buffer.begin() + buffer.size() - 1;
        Iterator const endInBuffer = buffer.begin() - 1;

        while( currentInBuffer != endInBuffer )
        {
            if(
                   currentInStorage == endInStorage
                || cmp( * currentInStorage, * currentInBuffer )
            )
            {
                * whereInsertInStorage = * currentInBuffer;

                -- currentInBuffer;
                -- whereInsertInStorage;
            }
            else
            {
                * whereInsertInStorage = * currentInStorage;

                -- currentInStorage;
                -- whereInsertInStorage;
            }
        }

        storage._size += buffer.size();
        buffer._size = 0;
    }
}

namespace detail
{
    template< typename _Iterator >
    bool
    equal( _Iterator const & lhs, _Iterator const & rhs )
    {
        if(
               lhs.getStorage()->_data != rhs.getStorage()->_data
            || lhs.getBuffer()->_data != rhs.getBuffer()->_data
        ){
            return false;
        }

        // for empty container returns that begin == end
        // despite on fact they are not
        if(
                lhs.getStorage()->size() == 0
             && rhs.getStorage()->size() == 0
             && lhs.getBuffer()->size() == 0
             && rhs.getBuffer()->size() == 0
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
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename std::iterator_traits< _Iterator >::value_type  value_type;
        typedef typename std::iterator_traits< _Iterator >::difference_type difference_type;
        typedef typename std::iterator_traits< _Iterator >::reference reference;
        typedef typename std::iterator_traits< _Iterator >::pointer   pointer;

        AssocVectorIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _storage( 0 )
            , _currentInStorage( 0 )
            , _buffer( 0 )
            , _currentInBuffer( 0 )
            , _erased( 0 )
            , _currentInErased( 0 )
            , _cmp( cmp )
            , _current( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorIterator( AssocVectorIterator< _Iter, _Container > const & other )
            : _storage( other.getStorage() )
            , _currentInStorage( other.getCurrentInStorage() )
            , _buffer( other.getBuffer() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _erased( other.getErased() )
            , _currentInErased( other.getCurrentInErased() )
            , _cmp( other.getCmp() )
            , _current( other.getCurrent() )
        {
        }

        AssocVectorIterator(
              typename _Container::_Storage const & storage
            , pointer currentInStorage
            , typename _Container::_Storage const & buffer
            , pointer currentInBuffer
            , typename _Container::_Erased const & erased
            , typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _storage( & storage )
            , _currentInStorage( currentInStorage )
            , _buffer( & buffer )
            , _currentInBuffer( currentInBuffer )
            , _erased( & erased )
            , _currentInErased( 0 )
            , _cmp( cmp )
            , _current( 0 )
        {
            _currentInErased
                = std::lower_bound(
                      _erased->begin()
                    , _erased->end()
                    , _currentInStorage
                    , std::less< typename _Container::_Storage::const_iterator >()
                );

            _current = calculateCurrent();
        }

        AssocVectorIterator & operator=( AssocVectorIterator const & other )
        {
            _storage = other._storage;
            _currentInStorage =  other._currentInStorage;
            _buffer = other._buffer;
            _currentInBuffer = other._currentInBuffer;
            _erased = other._erased;
            _currentInErased = other._currentInErased;
            _cmp = other._cmp;
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
            if( _currentInStorage == _storage->begin() && _currentInBuffer == _buffer->begin() ){
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

            return * _current;
        }

        pointer operator->()const{
            PRECONDITION( _current != 0 );

            return base();
        }

        pointer base()const{
            PRECONDITION( _current != 0 );

            return _current;
        }

        // public for copy constructor only : Iterator -> ConstIterator
        typename _Container::_Storage const * getStorage()const{ return _storage; }
        pointer getCurrentInStorage()const{ return _currentInStorage; }

        typename _Container::_Storage const * getBuffer()const{ return _buffer; }
        pointer getCurrentInBuffer()const{ return _currentInBuffer; }

        typename _Container::_Erased const * getErased()const{ return _erased; }
        typename _Container::_Erased::const_iterator getCurrentInErased()const{ return _currentInErased; }

        typename _Container::value_compare getCmp()const{ return _cmp; }
        pointer getCurrent()const{ return _current; }

    private:
        pointer
        calculateCurrent()
        {
           while(
                   _currentInErased != _erased->end()
                && _currentInStorage != _storage->end()
                && _currentInStorage == *_currentInErased
            )
            {
                ++ _currentInStorage;
                ++ _currentInErased;
            }

            if( _currentInStorage == _storage->end() )
            {
                if( _currentInBuffer == _buffer->end() ){
                    return 0;
                }

                POSTCONDITION( util::isBetween( _buffer->begin(), _currentInBuffer, _buffer->end() ) );
                return _currentInBuffer;
            }

            if(
                   _currentInBuffer == _buffer->end()
                || _cmp( * _currentInStorage, * _currentInBuffer )
            )
            {
                POSTCONDITION( util::isBetween( _storage->begin(), _currentInStorage, _storage->end() ) );
                return _currentInStorage;
            }
            else
            {
                POSTCONDITION( util::isBetween( _buffer->begin(), _currentInBuffer, _buffer->end() ) );
                return _currentInBuffer;
            }
        }

    private:
        typename _Container::_Storage const * _storage;
        pointer _currentInStorage;
        typename _Container::_Storage const * _buffer;
        pointer _currentInBuffer;

        typename _Container::_Erased const * _erased;
        typename _Container::_Erased::const_iterator _currentInErased;

        typename _Container::value_compare _cmp;

        pointer _current;
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
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef typename std::iterator_traits< _Iterator >::value_type  value_type;
        typedef typename std::iterator_traits< _Iterator >::difference_type difference_type;
        typedef typename std::iterator_traits< _Iterator >::reference reference;
        typedef typename std::iterator_traits< _Iterator >::pointer   pointer;

        AssocVectorReverseIterator(
            typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _storage( 0 )
            , _currentInStorage( 0 )
            , _buffer( 0 )
            , _currentInBuffer( 0 )
            , _erased( 0 )
            , _currentInErased( 0 )
            , _cmp( cmp )
            , _current( 0 )
        {
        }

        template< typename _Iter >
        AssocVectorReverseIterator( AssocVectorReverseIterator< _Iter, _Container > const & other )
            : _storage( other.getStorage() )
            , _currentInStorage( other.getCurrentInStorage() )
            , _buffer( other.getBuffer() )
            , _currentInBuffer( other.getCurrentInBuffer() )
            , _erased( other.getErased() )
            , _currentInErased( other.getCurrentInErased() )
            , _cmp( other.getCmp() )
            , _current( other.getCurrent() )
        {
        }

        AssocVectorReverseIterator(
              typename _Container::_Storage const & storage
            , pointer currentInStorage
            , typename _Container::_Storage const & buffer
            , pointer currentInBuffer
            , typename _Container::_Erased const & erased
            , typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _storage( & storage )
            , _currentInStorage( currentInStorage )
            , _buffer( & buffer )
            , _currentInBuffer( currentInBuffer )
            , _erased( & erased )
            , _currentInErased( 0 )
            , _cmp( cmp )
            , _current( 0 )
        {
            _currentInErased
                = util::last_less_equal(
                      _erased->begin()
                    , _erased->end()
                    , _currentInStorage
                    , std::less< typename _Container::_Storage::const_iterator >()
                );

            if( _currentInErased == _erased->end() ){
                -- _currentInErased;
            }

            _current = calculateCurrentReverse();
        }

        AssocVectorReverseIterator & operator=( AssocVectorReverseIterator const & other )
        {
            _storage = other._storage;
            _currentInStorage =  other._currentInStorage;
            _buffer = other._buffer;
            _currentInBuffer = other._currentInBuffer;
            _erased = other._erased;
            _currentInErased = other._currentInErased;
            _cmp = other._cmp;
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
            if( _currentInStorage == _storage->rbegin() && _currentInBuffer == _buffer->rbegin() ) {
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
            return * _current;
        }

        pointer operator->()const{
            return base();
        }

        pointer base()const{
            return _current;
        }

        // public for copy constructor only : Iterator -> ConstIterator
        typename _Container::_Storage const * getStorage()const{ return _storage; }
        pointer getCurrentInStorage()const{ return _currentInStorage; }
        typename _Container::_Storage const * getBuffer()const{ return _buffer; }
        pointer getCurrentInBuffer()const{ return _currentInBuffer; }
        typename _Container::_Erased const * getErased()const{ return _erased; }
        typename _Container::value_compare getCmp()const{ return _cmp; }
        pointer getCurrent()const{ return _current; }

    private:
        pointer
        calculateCurrentReverse()
        {
           while(
                   _currentInErased != _erased->rend()
                && _currentInStorage != _storage->rend()
                && _currentInStorage == *_currentInErased
            )
            {
                -- _currentInStorage;
                -- _currentInErased;
            }

            if( _currentInStorage == _storage->rend() )
            {
                if( _currentInBuffer == _buffer->rend() ){
                    return 0;
                }

                POSTCONDITION( util::isBetween( _buffer->begin(), _currentInBuffer, _buffer->end() ) );
                return _currentInBuffer;
            }
            else if( _currentInBuffer == getBuffer()->rend() )
            {
                POSTCONDITION( util::isBetween( _storage->begin(), _currentInStorage, _storage->end() ) );
                return _currentInStorage;
            }
            else if( _cmp( * _currentInStorage, * _currentInBuffer ) == false )
            {
                POSTCONDITION( util::isBetween( _storage->begin(), _currentInStorage, _storage->end() ) );
                return _currentInStorage;
            }

            POSTCONDITION( util::isBetween( _buffer->begin(), _currentInBuffer, _buffer->end() ) );
            return _currentInBuffer;
        }

    private:
        typename _Container::_Storage const * _storage;
        pointer _currentInStorage;
        typename _Container::_Storage const * _buffer;
        pointer _currentInBuffer;
        typename _Container::_Erased const * _erased;
        typename _Container::_Erased::const_iterator _currentInErased;

        typename _Container::value_compare _cmp;

        pointer _current;
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

    inline ~AssocVector();

    AssocVector & operator=( AssocVector const & other );

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
    inline bool insert( value_type const & value );

    template< typename _Iterator >
    inline void insert( _Iterator begin, _Iterator end );

    //
    // find
    //
    iterator find( key_type const & k );
    const_iterator find( key_type const & k )const;

    std::size_t count( key_type const & k )const;

    //
    // update
    //
    bool update( key_type const & k, mapped_type const & m );
    reference operator[]( key_type const & k );

    //
    // erase
    //
    void erase( key_type const & k );
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
    void reserveStorageAndMergeWithBuffer( std::size_t storageCapacity );

    //
    // insert
    //
    void pushBack( key_type const & k, mapped_type const & m );

    std::pair< bool, typename _Storage::iterator >
    findOrInsertToBuffer( key_type const & k, mapped_type const & m );

    //
    // update
    //
    bool update( _Storage & storage, key_type const & k, mapped_type const & m );

    bool isErased( typename _Storage::const_iterator iterator )const;

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

    for( /*empty*/ ; begin != end ; ++ begin, ++ begin2 ){
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
    array::create( _storage, other._storage, getAllocator( _storage ) );

    array::init( _buffer );
    array::create( _buffer, other._buffer, getAllocator( _buffer ) );

    array::init( _erased );
    array::create( _erased, other._erased, getAllocator( _erased ) );

    POSTCONDITION( operator==( *this, other ) );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::~AssocVector()
{
    array::destroy( _storage, getAllocator( _storage ) );
    array::destroy( _buffer, getAllocator( _buffer ) );
    array::destroy( _erased, getAllocator( _erased ) );
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
    AssocVector< _Key, _Mapped, _Cmp, _Alloc > temp( other );
    temp.swap( * this );

    POSTCONDITION( operator==( *this, other ) );

    return * this;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reserve( std::size_t newStorageCapacity )
{
    if( newStorageCapacity <= _storage.capacity() ){
        return;
    }

    if( _erased.empty() == false ){
        array::mergeWithErased( _storage, _erased, _storage );
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
    return iterator( _storage, _storage.begin(), _buffer, _buffer.begin(), _erased, _cmp );
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
    return reverse_iterator( _storage, _storage.rbegin(), _buffer, _buffer.rbegin(), _erased, _cmp );
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
    return const_iterator( _storage, _storage.begin(), _buffer, _buffer.begin(), _erased, _cmp );
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
    return const_reverse_iterator( _storage, _storage.rbegin(), _buffer, _buffer.rbegin(), _erased, _cmp );
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
    return iterator( _storage, _storage.end(), _buffer, _buffer.end(), _erased, _cmp );
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
    return reverse_iterator( _storage, _storage.rend(), _buffer, _buffer.rend(), _erased, _cmp );
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
    return const_iterator( _storage, _storage.end(), _buffer, _buffer.end(), _erased, _cmp );
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
    return const_reverse_iterator( _storage, _storage.rend(), _buffer, _buffer.rend(), _erased, _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::empty()const
{
    return size() == 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::size()const
{
    return _storage.size() + _buffer.size() - _erased.size();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( value_type const & value )
{
    std::size_t const size = this->size();

    _Key const & k = value.first;
    _Mapped const & m = value.second;

    if( _storage.empty() || _cmp( _storage.back().first, k ) ){
        return pushBack( k, m ), true;
    }

    if( _buffer.full() ){
        merge();
    }

    {//scope
        typename _Storage::iterator const found = find( _storage, k );

        if( found != _storage.end() ){
            return false;
        }
    }

    return findOrInsertToBuffer( k, m ).first;
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
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( _Iterator const begin, _Iterator const end )
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
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::pushBack( _Key const & k, _Mapped const & m )
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

    _storage._data[ _storage.size() ] = value_type_mutable( k, m );

    _storage._size += 1;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::update( _Key const & k, _Mapped const & m )
{
    return update( _storage, k, m ) || update( _buffer, k, m );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::isErased(
    typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::const_iterator iterator
)const
{
    typename _Erased::const_iterator const foundInErased
        = array::findInSorted(
              _erased
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
    std::size_t const size = this->size();

    typename _Storage::iterator const foundInStorage = find( _storage, k );

    if( foundInStorage != _storage.end() )
    {
        if( isErased( foundInStorage ) ){
            return end();
        }

        return iterator(
              _storage
            , foundInStorage
            , _buffer
            , _buffer.end()
            , _erased
            , value_comp()
        );
    }

    typename _Storage::iterator const foundInBuffer = find( _buffer, k );

    if( foundInBuffer == _buffer.end() ){
        end();
    }

    return iterator(
          _storage
        , _storage.end()
        , _buffer
        , foundInBuffer
        , _erased
        , value_comp()
    );
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
    typename _Storage::const_iterator const foundInStorage = find( _storage, k );

    if( foundInStorage != _storage.end() )
    {
        if( isErased( foundInStorage ) ){
            return end();
        }

        return const_iterator(
              _storage
            , foundInStorage
            , _buffer
            , _buffer.end()
            , _erased
            , value_comp()
        );
    }

    typename _Storage::const_iterator const foundInBuffer = find( _buffer, k );

    if( foundInBuffer != _buffer.end() ){
        return end();
    }

    return const_iterator(
          _storage
        , _storage.end()
        , _buffer
        , foundInBuffer
        , _erased
        , value_comp()
    );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::merge()
{
    if( _storage.size() + _buffer.size() - _erased.size() > _storage.capacity() ){
        return reserve( calculateNewStorageCapacity( _storage.capacity() ) );
    }

    if( _erased.empty() == false ){
        array::mergeWithErased( _storage, _erased );
    }

    array::merge( _storage, _buffer, value_comp() );
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
    if( _storage.empty() || _cmp( _storage.back().first, k ) ){
        return pushBack( k, mapped_type() ), _storage.back().second;
    }

    if( _buffer.full() ){
        merge();
    }

    {//scope
        typename _Storage::iterator const foundInStorage = find( _storage, k );

        if( foundInStorage != _storage.end() )
        {
            if( isErased( foundInStorage ) == false ){
                return foundInStorage->second;
            }
        }
    }

    return findOrInsertToBuffer( k, mapped_type() ).second->second;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::count( key_type const & k )const{
    return find( k ) == end() ? 0 : 1;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::erase( key_type const & k )
{
    typename _Storage::iterator const foundInStorage = find( _storage, k );

    if( foundInStorage == _storage.end() )
    {
        typename _Storage::iterator const foundInBuffer = find( _buffer, k );

        if( foundInBuffer != _buffer.end() ){
            array::erase( _buffer, foundInBuffer );
        }

        return;
    }

    if( foundInStorage + 1 == _storage.end() )
    {
        foundInStorage -> ~value_type_mutable();
        _storage._size -= 1;
    }
    else
    {
        array::insertSorted(
              _erased
            , typename _Storage::const_iterator( foundInStorage )
            , std::less< typename _Storage::const_iterator >()
        );

        if( _erased.full() ){
            array::mergeWithErased( _storage, _erased );
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
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::erase( iterator pos )
{
    std::size_t const size = this->size();

    if( util::isBetween( _buffer.begin(), pos.base(), _buffer.end() ) ){
        return array::erase( _buffer, pos.base() );
    }

    if( util::isBetween( _storage.begin(), pos.base(), _storage.end() ) )
    {
        if( pos.base() + 1 == _storage.end() )
        {
            pos.base() -> ~value_type_mutable();
            _storage._size -= 1;
        }
        else
        {
            array::insertSorted(
                  _erased
                , typename _Storage::const_iterator( pos.base() )
                , std::less< typename _Storage::const_iterator >()
            );

            if( _erased.full() ){
                array::mergeWithErased( _storage, _erased );
            }
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
    return array::findInSorted( container, value_type_mutable( k, mapped_type() ), value_comp() );
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

    std::size_t const bufferCapacity
        = calculateNewBufferCapacity( storageCapacity );

    _Storage newStorage = array::create( storageCapacity, getAllocator( _storage ) );
    _Storage newBuffer = array::create( bufferCapacity, getAllocator( _buffer ) );

    std::merge(
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

        _storage._data = newStorage._data;
        _storage._size = size + _buffer.size();
        _storage._capacity = storageCapacity;
    }

    {
        array::destroy( _buffer, getAllocator( _buffer ) );

        _buffer._data = newBuffer._data;
        _buffer._size = 0;
        _buffer._capacity = bufferCapacity;
    }
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
    return array::findInSorted( container, value_type_mutable( k, mapped_type() ), value_comp() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::update(
      _Storage & storage
    , _Key const & k
    , _Mapped const & m
)
{
    typename _Storage::iterator const found = find( k );

    if( found == storage.end() ){
        return false;
    }

    found->second = m;

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::pair< bool, typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator >
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
            return std::make_pair( false, greaterEqual );
        }
    }

    array::insert( _buffer, greaterEqual, value_type_mutable( k, m ) );

    return std::make_pair( true, greaterEqual );
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
