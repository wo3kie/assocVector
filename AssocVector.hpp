#ifndef ASSOC_VECTOR_HPP
#define ASSOC_VECTOR_HPP

#include <algorithm>
#include <functional>
#include <cmath>

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
    template< typename _Type >
    bool isBetween( _Type const & begin, _Type const & value, _Type const & end ){
        assert( begin <= end );

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
    //
    // Array
    //
    template< typename _T >
    struct Array
    {
        typedef _T value_type;

        typedef _T * iterator;
        typedef _T const * const_iterator;

        _T * data;

        std::size_t capacity;
        std::size_t size;

        iterator begin(){
            return data;
        }

        const_iterator begin()const{
            return data;
        }

        iterator rbegin(){
            return data + size - 1;
        }

        const_iterator rbegin()const{
            return data + size - 1;
        }

        iterator end(){
            return data + size;
        }

        const_iterator end()const{
            return data + size;
        }

        iterator rend(){
            return data - 1;
        }

        const_iterator rend()const{
            return data - 1;
        }

        bool empty()const{
            return size == 0;
        }

        bool full()const{
            return size == capacity;
        }

        value_type & front(){
            return data[ 0 ];
        }

        value_type const & front()const{
            return data[ 0 ];
        }

        value_type & back(){
            return data[ size - 1 ];
        }

        value_type const & back()const{
            return data[ size - 1 ];
        }

        value_type & operator[]( std::size_t index ){
            return data[ index ];
        }

        value_type const & operator[]( std::size_t index )const{
            return data[ index ];
        }
    };

    template<
          typename _Array
        , typename _Iterator
        , typename _T
        , typename _Cmp
    >
    _Iterator
    findInSortedImpl( _Array array, _T const t, _Cmp cmp )
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
    findInSorted( Array< _T > & array, _T const t, _Cmp cmp )
    {
        return
        findInSortedImpl<
              Array< _T >
            , typename Array< _T >::iterator
        >( array, t, cmp );
    }
    
    template<
          typename _T
        , typename _Cmp
    >
    typename Array< _T >::const_iterator
    findInSorted( Array< _T > const & array, _T const t, _Cmp cmp )
    {
        return
        findInSortedImpl<
              Array< _T > const
            , typename Array< _T >::const_iterator
        >( array, t, cmp );
    }
}

namespace util
{    
    template< typename _T, typename _Cmp >
    void mergeInplace(
          util::Array< _T > & storage
        , util::Array< _T > & buffer
        , _Cmp const & cmp = _Cmp()
    )
    {
        assert( storage.size + buffer.size <= storage.capacity );

        typedef typename util::Array< _T >::iterator Iterator;
        
        Iterator whereInsertInStorage = storage.begin() + storage.size + buffer.size - 1;
        
        Iterator currentInStorage = storage.begin() + storage.size - 1;
        Iterator endInStorage = storage.begin() - 1;

        Iterator currentInBuffer = buffer.begin() + buffer.size - 1;
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

        storage.size += buffer.size;
        buffer.size = 0;
    }
}

namespace detail
{
    template< typename _Iterator >
    bool
    equal( _Iterator const & lhs, _Iterator const & rhs )
    {
        if(
               lhs.getStorage()->data != rhs.getStorage()->data
            || lhs.getBuffer()->data != rhs.getBuffer()->data
        ){
            return false;
        }

        // for empty container returns that begin == end
        // despite on fact they are not
        if(
                lhs.getStorage()->size == 0
             && rhs.getStorage()->size == 0
             && lhs.getBuffer()->size == 0
             && rhs.getBuffer()->size == 0
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
            , _cmp( other.getCmp() )
            , _current( other.getCurrent() )
        {
        }

        AssocVectorIterator(
              typename _Container::_Storage const & storage
            , pointer currentInStorage
            , typename _Container::_Storage const & buffer
            , pointer currentInBuffer
            , typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _storage( & storage )
            , _currentInStorage( currentInStorage )
            , _buffer( & buffer )
            , _currentInBuffer( currentInBuffer )
            , _cmp( cmp )
            , _current( calculateCurrent() )
        {
        }

        AssocVectorIterator & operator=( AssocVectorIterator const & other )
        {
            _storage = other._storage;
            _currentInStorage =  other._currentInStorage;
            _buffer = other._buffer;
            _currentInBuffer = other._currentInBuffer;
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
        typename _Container::value_compare getCmp()const{ return _cmp; }
        pointer getCurrent()const{ return _current; }

    private:
        pointer
        calculateCurrent()
        {
            if( _currentInStorage == _storage->end() )
            {
                if( _currentInBuffer == _buffer->end() ){
                    return 0;
                }

                return _currentInBuffer;
            }

            if( _currentInBuffer == _buffer->end() ){
                return _currentInStorage;
            }
            else if( _cmp( * _currentInStorage, * _currentInBuffer ) ){
                return _currentInStorage;
            }
            else{
                return _currentInBuffer;
            }
        }

    private:
        typename _Container::_Storage const * _storage;
        pointer _currentInStorage;
        typename _Container::_Storage const * _buffer;
        pointer _currentInBuffer;

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
            , _cmp( other.getCmp() )
            , _current( other.getCurrent() )
        {
        }

        AssocVectorReverseIterator(
              typename _Container::_Storage const & storage
            , pointer currentInStorage
            , typename _Container::_Storage const & buffer
            , pointer currentInBuffer
            , typename _Container::value_compare const & cmp = typename _Container::value_compare()
        )
            : _storage( & storage )
            , _currentInStorage( currentInStorage )
            , _buffer( & buffer )
            , _currentInBuffer( currentInBuffer )
            , _cmp( cmp )
            , _current( calculateCurrentReverse() )
        {
        }

        AssocVectorReverseIterator & operator=( AssocVectorReverseIterator const & other )
        {
            _storage = other._storage;
            _currentInStorage =  other._currentInStorage;
            _buffer = other._buffer;
            _currentInBuffer = other._currentInBuffer;
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
        typename _Container::value_compare getCmp()const{ return _cmp; }
        pointer getCurrent()const{ return _current; }

    private:
        pointer
        calculateCurrentReverse()
        {
            if( _currentInStorage == _storage->rend() )
            {
                if( _currentInBuffer == _buffer->rend() ){
                    return 0;
                }

                return _currentInBuffer;
            }
            else if( _currentInBuffer == getBuffer()->rend() ){
                return _currentInStorage;
            }
            else if( _cmp( * _currentInStorage, * _currentInBuffer ) == false ){
                return _currentInStorage;
            }

            return _currentInBuffer;
        }

    private:
        typename _Container::_Storage const * _storage;
        pointer _currentInStorage;
        typename _Container::_Storage const * _buffer;
        pointer _currentInBuffer;

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

public:
    typedef _Key key_type;
    typedef _Mapped mapped_type;

    typedef std::pair< _Key const, _Mapped > value_type;
    typedef mapped_type & reference;

    typedef _Cmp key_compare;
    typedef _Alloc allocator_type;

    typedef util::CmpByFirst< value_type_mutable, _Cmp > value_compare;

    typedef detail::AssocVectorIterator< value_type_mutable *, AssocVector > iterator;
    typedef detail::AssocVectorIterator< value_type_mutable const *, AssocVector > const_iterator;

    typedef detail::AssocVectorReverseIterator< value_type_mutable *, AssocVector > reverse_iterator;
    typedef detail::AssocVectorReverseIterator< value_type_mutable const *, AssocVector > const_reverse_iterator;

    typedef util::Array< value_type_mutable > _Storage;
    
    typedef util::Array< typename _Storage::const_iterator > _Erased;

public:
    explicit AssocVector( _Cmp const & cmp = _Cmp(), _Alloc const & allocator = _Alloc() );
    AssocVector( AssocVector< _Key, _Mapped, _Cmp, _Alloc > const & other );

    inline ~AssocVector();

    AssocVector operator=( AssocVector const & other );

    void reserve( std::size_t newCapacity );

    inline iterator begin();
    inline const_iterator begin()const;

    inline reverse_iterator rbegin();
    inline const_reverse_iterator rbegin()const;

    inline iterator end();
    inline const_iterator end()const;

    inline reverse_iterator rend();
    inline const_reverse_iterator rend()const;

    inline bool empty()const;
    inline std::size_t size()const;

    inline bool insert( value_type const & value );

    template< typename _Iterator >
    inline void insert( _Iterator begin, _Iterator end );

    bool update( key_type const & k, mapped_type const & m );

    iterator find( key_type const & k );
    const_iterator find( key_type const & k )const;

    reference operator[]( key_type const & k );

    std::size_t count( key_type const & k )const;

    void erase( key_type const & k );
    void erase( iterator pos );

    void merge();

    key_compare key_comp()const{ return _cmp; }
    value_compare value_comp()const{ return value_compare( _cmp ); }

    void swap( AssocVector & other );

private:
    void push_back( key_type const & k, mapped_type const & m );

    typename _Storage::iterator find( _Storage & container, key_type const & k );
    typename _Storage::const_iterator find( _Storage const & container, key_type const & k )const;

    bool update( _Storage & storage, key_type const & k, mapped_type const & m );

    std::pair< bool, typename _Storage::iterator >
    findOrInsertToBuffer( key_type const & k, mapped_type const & m );

    template< typename __Container >
    void reserveImpl( __Container & container, std::size_t capacity );

    void createNewStorageAndMerge();
    
    void eraseImpl( _Storage & container, typename _Storage::iterator pos );

    template< typename __Container >
    void destroyAndFreeMemory( __Container & container );
    
    template< typename __Container >
    typename __Container::iterator allocateAndConstruct( std::size_t capacity );
    
        // method specialization
        _Alloc getAllocator( _Storage const & ){ return _allocator; }
    
        typename _Alloc::template rebind< typename _Storage::const_iterator >::other
        getAllocator( _Erased const & )
        {return typename _Alloc::template rebind< typename _Storage::const_iterator >::other( _allocator );}
    
public: // public for unit tests only
    std::size_t bufferSize()const{ return _buffer.size; }
    std::size_t bufferCapacity()const{ return _buffer.capacity; }
    std::size_t storageSize()const{ return _storage.size; }
    std::size_t storageCapacity()const{ return _storage.capacity; }

    static std::size_t calculateNewBufferCapacity( std::size_t storageSize );
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
    {// init storage
        _storage.data = 0;
        _storage.capacity = 0;
        _storage.size = 0;
    }

    {// init buffer
        _buffer.data = 0;
        _buffer.capacity = 0;
        _buffer.size = 0;
    }
    
    {// init erased
        _erased.data = 0;
        _erased.capacity = 0;
        _erased.size = 0;
    }

    std::size_t const defaultMinimalSize = 2*(2*2);

    reserve( defaultMinimalSize );
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
    {// init storage
        _storage.capacity = other._storage.capacity;
        _storage.size = other._storage.size;
        _storage.data = allocateAndConstruct< _Storage >( _storage.capacity );

        util::copyRange( other._storage.begin(), other._storage.end(), _storage.begin() );
    }

    {// init buffer
        _buffer.capacity = other._buffer.capacity;
        _buffer.size = other._buffer.size;
        _buffer.data = allocateAndConstruct< _Storage >( _buffer.capacity );

        util::copyRange( other._buffer.begin(), other._buffer.end(), _buffer.begin() );
    }
    
    {// init erased
        _erased.capacity = other._erased.capacity;
        _erased.size = other._erased.size;
        _erased.data = allocateAndConstruct< _Erased >( _erased.capacity );

        util::copyRange( other._erased.begin(), other._erased.end(), _erased.begin() );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::~AssocVector()
{
    {// free storage
        destroyAndFreeMemory( _storage );

        _storage.data = 0;
        _storage.capacity = 0;
        _storage.size = 0;
    }

    {// free buffer
        destroyAndFreeMemory( _buffer );

        _buffer.data = 0;
        _buffer.capacity = 0;
        _buffer.size = 0;
    }
    
    {// free erased
        destroyAndFreeMemory( _erased );

        _erased.data = 0;
        _erased.capacity = 0;
        _erased.size = 0;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
AssocVector< _Key, _Mapped, _Cmp, _Alloc >
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::operator=( AssocVector const & other )
{
    AssocVector< _Key, _Mapped, _Cmp, _Alloc > temp( other );
    temp.swap( * this );

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
    if( newStorageCapacity <= _storage.capacity ){
        return;
    }

    reserveImpl( _storage, newStorageCapacity );
    reserveImpl( _buffer, calculateNewBufferCapacity( newStorageCapacity ) );
    reserveImpl( _erased, calculateNewBufferCapacity( newStorageCapacity ) );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
template<
      typename __Container
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reserveImpl(
      __Container & container
    , std::size_t capacity
)
{
    typename __Container::iterator newMemory = allocateAndConstruct< __Container >( capacity );
    util::copyRange( container.begin(), container.end(), newMemory );

    destroyAndFreeMemory( container );

    container.capacity = capacity;
    container.data = newMemory;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::begin(){
    return iterator( _storage, _storage.begin(), _buffer, _buffer.begin(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rbegin(){
    return reverse_iterator( _storage, _storage.rbegin(), _buffer, _buffer.rbegin(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::begin()const{
    return const_iterator( _storage, _storage.begin(), _buffer, _buffer.begin(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rbegin()const{
    return const_reverse_iterator( _storage, _storage.rbegin(), _buffer, _buffer.rbegin(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::end(){
    return iterator( _storage, _storage.end(), _buffer, _buffer.end(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rend(){
    return reverse_iterator( _storage, _storage.rend(), _buffer, _buffer.rend(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::end()const{
    return const_iterator( _storage, _storage.end(), _buffer, _buffer.end(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::const_reverse_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::rend()const{
    return const_reverse_iterator( _storage, _storage.rend(), _buffer, _buffer.rend(), _cmp );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::empty()const{
    return size() == 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::size()const{
    return _storage.size + _buffer.size - _erased.size;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( value_type const & value )
{
    _Key const & k = value.first;
    _Mapped const & m = value.second;

    if( _storage.empty() || _cmp( _storage.back().first, k ) ){
        push_back( k, m );
        return true;
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
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::insert( _Iterator begin, _Iterator const end )
{
    for( /*empty*/ ; begin != end ; ++ begin ){
        insert( * begin );
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::push_back( _Key const & k, _Mapped const & m )
{
    assert(
        (
               _storage.size == 0
            || _cmp( _storage.back().first, k )
        )
    );

    if( _storage.size + 1 >= _storage.capacity ){
        reserveImpl( _storage, calculateNewStorageCapacity( _storage.capacity ) );
    }

    _storage.data[ _storage.size ] = value_type_mutable( k, m );

    _storage.size += 1;
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
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::find( _Key const & k )
{
    typename _Storage::iterator const foundInStorage = find( _storage, k );

    if( foundInStorage != _storage.end() ){
        typename _Erased::iterator const foundInErased
            = util::findInSorted(
                  _erased
                , typename _Storage::const_iterator( foundInStorage )
                , std::less< typename _Storage::const_iterator >()
            );
        
        if( foundInErased == _erased.end() )
        {
            return iterator(
                  _storage
                , foundInStorage
                , _buffer
                , _buffer.end()
                , value_comp()
            );
        }
    }

    typename _Storage::iterator const foundInBuffer = find( _buffer, k );

    if( foundInBuffer != _buffer.end() ){
        return iterator(
              _storage
            , _storage.end()
            , _buffer
            , foundInBuffer
            , value_comp()
        );
    }

    return end();
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

    if( foundInStorage != _storage.end() ){
        typename _Erased::const_iterator const foundInErased
            = util::findInSorted(
                  _erased
                , foundInStorage
                , std::less< typename _Storage::const_iterator >()
            );
        
        if( foundInErased == _erased.end() )
        {
            return const_iterator(
                  _storage
                , foundInStorage
                , _buffer
                , _buffer.end()
                , value_comp()
            );
        }
    }

    typename _Storage::const_iterator const foundInBuffer = find( _buffer, k );

    if( foundInBuffer != _buffer.end() ){
        return const_iterator(
              _storage
            , _storage.end()
            , _buffer
            , foundInBuffer
            , value_comp()
        );
    }

    return end();
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::merge()
{
    if( _storage.size + _buffer.size >= _storage.capacity ){
        createNewStorageAndMerge();
    }
    else{
        util::mergeInplace( _storage, _buffer, value_comp() );
    }
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
        push_back( k, mapped_type() );
        return _storage.back().second;
    }

    if( _buffer.full() ){
        merge();
    }
    
    {//scope
        typename _Storage::iterator const foundInStorage = find( _storage, k );
        
        if( foundInStorage != _storage.end() ){
            typename _Erased::const_iterator const foundInErased
                = util::findInSorted(
                      _erased
                    , typename _Storage::const_iterator( foundInStorage )
                    , std::less< typename _Storage::const_iterator >()
                );
            
            if( foundInErased == _erased.end() )
            {
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
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::eraseImpl( _Storage & container, typename _Storage::iterator pos )
{
    util::copyRange( pos + 1, container.end(), pos );
    container.size -= 1;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::erase( key_type const & k )
{//todo: optimise
    typename _Storage::iterator const foundInStorage = find( _storage, k );

    if( foundInStorage != _storage.end() ){
        eraseImpl( _storage, foundInStorage );
    }

    typename _Storage::iterator const foundInBuffer = find( _buffer, k );

    if( foundInBuffer != _buffer.end() ){
        eraseImpl( _buffer, foundInBuffer );
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
{//todo: optimise
    if( util::isBetween( _storage.begin(), pos.base(), _storage.end() ) ){
        eraseImpl( _storage, pos.base() );
    }

    if( util::isBetween( _buffer.begin(), pos.base(), _buffer.end() ) ){
        eraseImpl( _buffer, pos.base() );
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
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::find( _Storage & container, key_type const & k )
{
    return util::findInSorted( container, value_type_mutable( k, mapped_type() ), value_comp() );
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
    return util::findInSorted( container, value_type_mutable( k, mapped_type() ), value_comp() );
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
    typename _Storage::iterator const foundInStorage = find( storage, k );

    if( foundInStorage == storage.end() ){
        typename _Erased::const_iterator const foundInErased
            = util::findInSorted(
                  _erased
                , typename _Storage::const_iterator( foundInStorage )
                , std::less< typename _Storage::const_iterator >()
            );
        
        if( foundInErased == _erased.end() )
        {
            return false;
        }
    }

    foundInStorage->second = m;
    
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

    if( greaterEqual == _buffer.end() )
    {
        _buffer.data[ _buffer.size ] = value_type( k, m );

        _buffer.size += 1;
        
        return std::make_pair( true, _buffer.data + _buffer.size - 1 );
    }
    else
    {
        bool const isGreater = _cmp( k, greaterEqual->first );
        
        if( isGreater )
        {
            util::copyRange(
                  greaterEqual
                , _buffer.end()
                , greaterEqual + 1
            );

            * greaterEqual = value_type_mutable( k, m );

            _buffer.size += 1;
            
            return std::make_pair( true, greaterEqual );
        }
        else
        {
            return std::make_pair( false, greaterEqual );
        }        
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
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::calculateNewStorageCapacity(
    std::size_t storageSize
)
{
    return static_cast< std::size_t >( 2 * storageSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::createNewStorageAndMerge()
{
    std::size_t const newStorageCapacity = calculateNewStorageCapacity( _storage.capacity );

    _Storage newStorage;
    newStorage.data = allocateAndConstruct< _Storage >( newStorageCapacity );
    newStorage.capacity = newStorageCapacity;
    newStorage.size = _storage.size + _buffer.size;

    std::merge(
          _storage.begin()
        , _storage.end()
        , _buffer.begin()
        , _buffer.end()
        , newStorage.begin()
        , value_comp()
    );

    {// storage
        destroyAndFreeMemory( _storage );

        _storage.data = newStorage.data;
        _storage.capacity = newStorage.capacity;
        _storage.size = newStorage.size;
    }

    {// buffer
        std::size_t const newBufferCapacity = calculateNewBufferCapacity( newStorageCapacity );

        destroyAndFreeMemory( _buffer );

        _buffer.data = allocateAndConstruct< _Storage >( newBufferCapacity );
        _buffer.capacity = newBufferCapacity;
        _buffer.size = 0;
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
template<
      typename __Container
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::destroyAndFreeMemory( __Container & container )
{
    util::destroy_range( container.begin(), container.end() );
    getAllocator( container ).deallocate( container.data, container.capacity );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
template<
      typename __Container
>
typename __Container::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::allocateAndConstruct( std::size_t capacity )
{
    typename __Container::iterator result
        = static_cast< typename __Container::iterator >
            ( getAllocator( __Container() ).allocate( capacity ) );

    util::construct_range( result, result + capacity );

    return result;
}

#endif
