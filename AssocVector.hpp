#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#include <algorithm>
#include <cassert>
#include <functional>
#include <cmath>

#define PRECONDITION( condition ) assert( condition );
#define POSTCONDITION( condition ) assert( condition );

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
    // destroy_range
    //
    template< typename _Ptr >
    inline
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
    inline
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
    inline
    void copyRange(
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
          , _Alloc allocator = std::allocator< _T >()
    )
    {
        Array< _T > result;

        result.setData( static_cast< _T * >( allocator.allocate( capacity ) ) );

        POSTCONDITION( result.getData() != 0 );

        result.setSize( 0 );
        result.setCapacity( capacity );

        util::construct_range( result.getData(), result.getData() + capacity );

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

        util::copyRange( origin.begin(), origin.end(), dest.begin() );

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
        util::destroy_range( array.begin(), array.end() );
        allocator.deallocate( array.getData(), array.capacity() );

        init( array );
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

        util::copyRange( array.begin(), array.end(), newArray.begin() );
        array::destroy( array, allocator );

        array.setSize( size );
        array.setCapacity( capacity );
        array.setData( newArray.getData() );
    }

    template< typename _T >
    inline
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
    inline
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
    inline
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
    inline
    void insert(
          Array< _T > & array
        , typename Array< _T >::iterator pos
        , _T const & t
    )
    {
        PRECONDITION( array.size() + 1 <= array.capacity() );

        if( pos != array.end() ){
            util::copyRange( pos, array.end(), pos + 1 );
        }

        * pos = t;

        array.setSize( array.size() + 1 );
    }

    template< typename _T >
    inline
    void pushBack(
          Array< _T > & array
        , _T const & t
    )
    {
        PRECONDITION( array.size() + 1 <= array.capacity() );

        array[ array.size() ] = t;
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

        util::copyRange( pos + 1, array.end(), pos );
        array.setSize( array.size() - 1 );
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

        StorageConstIterator currentInStorage = erased.front();
        StorageConstIterator const endInStorage = storage.end();

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
              * whereInsertInStorage = * currentInStorage;

              ++ whereInsertInStorage;
              ++ currentInStorage;
            }
        }

        POSTCONDITION( currentInErased == endInErased );

        storage.setSize( storage.size() - erased.size() );
    }

    template< typename _T >
    void mergeWithErased(
          array::Array< _T > & storage
        , array::Array< typename array::Array< _T >::const_iterator > const & erased
        , array::Array< _T > & newStorage
    )
    {
        PRECONDITION( storage.size() >= erased.size() );

        typedef typename array::Array< _T >::iterator StorageIterator;
        typedef typename array::Array< _T >::const_iterator StorageConstIterator;
        typedef typename array::Array< StorageConstIterator >::const_iterator ErasedConstIterator;

        StorageConstIterator currentInStorage = storage.begin();
        StorageConstIterator const endInStorage = storage.end();

        StorageIterator whereInsertInStorage = newStorage.begin();

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
              * whereInsertInStorage = * currentInStorage;

              ++ whereInsertInStorage;
              ++ currentInStorage;
            }
        }

        newStorage.setSize( storage.size() - erased.size() );
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

        storage.setSize( storage.size() + buffer.size() );
    }
}

namespace detail
{
    template< typename _Iterator >
    bool equal( _Iterator const & lhs, _Iterator const & rhs )
    {
        if(
               lhs.getStorage()->getData() != rhs.getStorage()->getData()
            || lhs.getBuffer()->getData() != rhs.getBuffer()->getData()
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
            , _cmp( cmp )
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

        AssocVectorIterator &
        operator=( AssocVectorIterator const & other )
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

                return _currentInBuffer;
            }

            if(
                   _currentInBuffer == _buffer->end()
                || _cmp( * _currentInStorage, * _currentInBuffer )
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
            , _cmp( cmp )
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

                return _currentInBuffer;
            }
            else if( _currentInBuffer == getBuffer()->rend() )
            {
                return _currentInStorage;
            }
            else if( _cmp( * _currentInStorage, * _currentInBuffer ) == false )
            {
                return _currentInStorage;
            }

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
    bool insert( value_type const & value );

    template< typename _Iterator >
    inline void insert( _Iterator begin, _Iterator end );

    //
    // find
    //
    iterator find( key_type const & k );
    const_iterator find( key_type const & k )const;

    inline std::size_t count( key_type const & k )const;

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
    void mergeStorageWithBuffer();
    void mergeStorageWithErased();
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
    AssocVector temp( other );
    temp.swap( * this );

    return * this;
}

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
bool
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( value_type const & value )
{
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

    array::pushBack( _storage, value_type_mutable( k, m ) );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::update( _Key const & k, _Mapped const & m )
{
    return update( _storage, k, m ) || update( _buffer, k, m );
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
        return end();
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

    {//erase from _buffer
        if( foundInStorage == _storage.end() )
        {
            typename _Storage::iterator const foundInBuffer = find( _buffer, k );

            if( foundInBuffer != _buffer.end() ){
                array::erase( _buffer, foundInBuffer );
            }

            return;
        }
    }

    {//erase from back
        if( foundInStorage + 1 == _storage.end() )
        {
            foundInStorage -> ~value_type_mutable();
            _storage.setSize( _storage.size() - 1 );

            return;
        }
    }

    {//erase from _storage
        array::insertSorted(
              _erased
            , typename _Storage::const_iterator( foundInStorage )
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
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::erase( iterator pos )
{
    {//erase from _buffer
        if( util::isBetween( _buffer.begin(), pos.base(), _buffer.end() ) ){
            return array::erase( _buffer, pos.base() );
        }
    }

    {//item not present in container
        if( util::isBetween( _storage.begin(), pos.base(), _storage.end() ) == false ){
            return;
        }
    }

    {//erase from back
        if( pos.base() + 1 == _storage.end() )
        {
            pos.base() -> ~value_type_mutable();
            _storage.setSize( _storage.size() - 1 );

            return;
        }
    }

    {//erase from _storage
        array::insertSorted(
              _erased
            , typename _Storage::const_iterator( pos.base() )
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

    _Storage newStorage = array::create< value_type_mutable >( storageCapacity, getAllocator( _storage ) );
    _Storage newBuffer = array::create< value_type_mutable >( bufferCapacity, getAllocator( _buffer ) );

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
    array::mergeWithErased( _storage, _erased );
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
