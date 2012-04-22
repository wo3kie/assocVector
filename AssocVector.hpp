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
}

namespace util
{
    template< typename _T, typename _Cmp >
    void mergeInplace(
          util::Array< _T > & storage
        , util::Array< _T > & buffer
        , _Cmp const & cmp = _Cmp()
    )
    {//todo: simplify -> backward_merge

        assert( storage.size + buffer.size <= storage.capacity );

        if( buffer.size == 0 ){
            return;
        }

        if( storage.size == 0 || cmp( *( storage.end() - 1 ), * buffer.data ) )
        {
            util::copyRange( buffer.begin(), buffer.end(), storage.end() );

            storage.size += buffer.size;
            buffer.size = 0;

            return;
        }

        typename util::Array< _T >::iterator storageTerminator = storage.end();

        while( buffer.size != 0 )
        {
            _T const & item = buffer.back();

            typename util::Array< _T >::iterator greaterEqual
                = std::lower_bound(
                      storage.begin()
                    , storageTerminator
                    , item
                    , cmp
                );

            if( greaterEqual == storage.begin() )
            {
                util::copyRange(
                      storage.begin()
                    , storageTerminator
                    , storage.begin() + buffer.size
                );

                util::copyRange(
                      buffer.begin()
                    , buffer.end()
                    , storage.begin()
                );

                storage.size += buffer.size;
                buffer.size = 0;

                return;
            }
            else if( greaterEqual == storageTerminator )
            {
                greaterEqual[ buffer.size - 1 ] = item;

                storage.size += 1;
                buffer.size -= 1;
            }
            else{
                util::copyRange(
                      greaterEqual
                    , storageTerminator
                    , greaterEqual + buffer.size
                );

                greaterEqual[ buffer.size - 1 ] = item;

                storage.size += 1;
                buffer.size -= 1;

                storageTerminator = greaterEqual;
            }
        }
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
    typedef std::pair< _Key, _Mapped > value_type_private;

public:
    typedef _Key key_type;
    typedef _Mapped mapped_type;

    typedef std::pair< _Key const, _Mapped > value_type;
    typedef mapped_type & reference;

    typedef _Cmp key_compare;
    typedef _Alloc allocator_type;

    typedef util::CmpByFirst< value_type_private, _Cmp > value_compare;

    typedef detail::AssocVectorIterator< value_type_private *, AssocVector > iterator;
    typedef detail::AssocVectorIterator< value_type_private const *, AssocVector > const_iterator;

    typedef detail::AssocVectorReverseIterator< value_type_private *, AssocVector > reverse_iterator;
    typedef detail::AssocVectorReverseIterator< value_type_private const *, AssocVector > const_reverse_iterator;

    typedef util::Array< value_type_private > _Storage;

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

    typename _Storage::iterator findImpl( _Storage & container, key_type const & k );
    typename _Storage::const_iterator findImpl( _Storage const & container, key_type const & k )const;

    inline typename _Storage::iterator findInStorage( key_type const & k );
    inline typename _Storage::const_iterator findInStorage( key_type const & k )const;

    inline typename _Storage::iterator findInBuffer( key_type const & k );
    inline typename _Storage::const_iterator findInBuffer( key_type const & k )const;

    bool checkForUpdateStorage( key_type const & k, mapped_type const & m );
    bool checkForUpdateBuffer( key_type const & k, mapped_type const & m );

    bool findOrInsertToBuffer( key_type const & k, mapped_type const & m );

    void mergeImpl();

    void eraseImpl( _Storage & container, typename _Storage::iterator pos );

    void destroyAndFreeMemory( _Storage & storage );
    void destroy( _Storage & storage );
    typename _Storage::iterator allocateAndConstructMemory( std::size_t capacity );

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
        _storage.data = allocateAndConstructMemory( _storage.capacity );

        util::copyRange( other._storage.begin(), other._storage.end(), _storage.begin() );
    }

    {// init buffer
        _buffer.capacity = other._buffer.capacity;
        _buffer.size = other._buffer.size;
        _buffer.data = allocateAndConstructMemory( _buffer.capacity );

        util::copyRange( other._buffer.begin(), other._buffer.end(), _buffer.begin() );
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
    newStorageCapacity = std::max< std::size_t >( 8, newStorageCapacity );

    if( newStorageCapacity <= _storage.capacity ){
        return;
    }

    {// reserve storage
        typename _Storage::iterator newStorage = allocateAndConstructMemory( newStorageCapacity );
        util::copyRange( _storage.begin(), _storage.end(), newStorage );

        destroyAndFreeMemory( _storage );

        _storage.capacity = newStorageCapacity;
        _storage.data = newStorage;
    }

    {// reserve buffer
        std::size_t const newBufferCapacity = calculateNewBufferCapacity( newStorageCapacity );

        typename _Storage::iterator newMemory = allocateAndConstructMemory( newBufferCapacity );
        util::copyRange( _buffer.begin(), _buffer.end(), newMemory );

        destroyAndFreeMemory( _buffer );

        _buffer.capacity = newBufferCapacity;
        _buffer.data = newMemory;
    }
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
    return _storage.size == 0 && _buffer.size == 0;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
std::size_t AssocVector< _Key, _Mapped, _Cmp, _Alloc >::size()const{
    return _storage.size + _buffer.size;
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

    if( findInStorage( k ) != _storage.end() ){
        return false;
    }

    return findOrInsertToBuffer( k, m );
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
        std::size_t const newStorageCapacity = calculateNewStorageCapacity( _storage.capacity );

        typename _Storage::iterator newMemoryBuffer = allocateAndConstructMemory( newStorageCapacity );
        util::copyRange( _storage.begin(), _storage.end(), newMemoryBuffer );

        destroyAndFreeMemory( _storage );

        _storage.data = newMemoryBuffer;
        _storage.capacity = newStorageCapacity;
    }

    _storage.data[ _storage.size ] = value_type_private( k, m );

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
    if( checkForUpdateStorage( k, m ) ){
        return true;
    }

    return checkForUpdateBuffer( k, m );
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
    typename _Storage::iterator const foundInStorage = findInStorage( k );

    if( foundInStorage != _storage.end() ){
        return iterator(
              _storage
            , foundInStorage
            , _buffer
            , _buffer.end()
            , value_comp()
        );
    }

    typename _Storage::iterator const foundInBuffer = findInBuffer( k );

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
    typename _Storage::const_iterator const foundInStorage = findInStorage( k );

    if( foundInStorage != _storage.end() ){
        return const_iterator(
              _storage
            , foundInStorage
            , _buffer
            , _buffer.end()
            , value_comp()
        );
    }

    typename _Storage::const_iterator const foundInBuffer = findInBuffer( k );

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
        mergeImpl();
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
{// !! not optimized at all !!
    iterator found = find( k );

    if( found != end() ){
        return found->second;
    }

    insert( value_type( k, mapped_type() ) );

    return find( k )->second;
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
    typename _Storage::iterator const foundInStorage = findInStorage( k );

    if( foundInStorage != _storage.end() ){
        eraseImpl( _storage, foundInStorage );
    }

    typename _Storage::iterator const foundInBuffer = findInBuffer( k );

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
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findImpl( _Storage & container, key_type const & k )
{
    typename _Storage::iterator const greaterEqual
        = std::lower_bound(
              container.begin()
            , container.end()
            , k
            , value_comp()
        );

    if( greaterEqual == container.end() ){
        return greaterEqual;
    }
    else if( _cmp( k, greaterEqual->first ) == false ){
        return greaterEqual;
    }
    else{
        return container.end();
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findImpl(
      _Storage const & container
    , key_type const & k
)const
{
    typename _Storage::const_iterator const greaterEqual
        = std::lower_bound(
              container.begin()
            , container.end()
            , k
            , value_comp()
        );

    if( greaterEqual == container.end() ){
        return greaterEqual;
    }
    else if( _cmp( k, greaterEqual->first ) == false ){
        return greaterEqual;
    }
    else{
        return container.end();
    }
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findInStorage( _Key const & k ){
    return findImpl( _storage, k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findInStorage( _Key const & k )const{
    return findImpl( _storage, k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findInBuffer( _Key const & k ){
    return findImpl( _buffer, k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::const_iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findInBuffer( _Key const & k )const{
    return findImpl( _buffer, k );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::checkForUpdateStorage(
      _Key const & k
    , _Mapped const & m
)
{
    typename _Storage::iterator const greaterEqualInStorage = findInStorage( k );

    if( greaterEqualInStorage == _storage.end() ){
        return false;
    }

    greaterEqualInStorage->second = m;

    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::checkForUpdateBuffer(
      _Key const & k
    , _Mapped const & m
)
{
    typename _Storage::iterator const greaterEqualInBuffer = findInStorage( k );

    if( greaterEqualInBuffer == _buffer.end() ){
        return false;
    }

    greaterEqualInBuffer->second = m;
    return true;
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
bool AssocVector< _Key, _Mapped, _Cmp, _Alloc >::findOrInsertToBuffer(
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

    if( greaterEqualInBuffer == _buffer.end() )
    {
        _buffer.data[ _buffer.size ] = value_type( k, m );
        _buffer.size += 1;
    }
    else if( _cmp( k, greaterEqualInBuffer->first ) == false ){
        return false;
    }
    else
    {
        util::copyRange(
              greaterEqualInBuffer
            , _buffer.end()
            , greaterEqualInBuffer + 1
        );

        * greaterEqualInBuffer =value_type_private( k, m );

        _buffer.size += 1;
    }

    if( _buffer.full() ){
        merge();
    }

    return true;
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
    return static_cast< std::size_t >( 1.5 * storageSize );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::mergeImpl()
{
    std::size_t const newStorageCapacity = calculateNewStorageCapacity( _storage.capacity );

    _Storage newStorage;
    newStorage.data = allocateAndConstructMemory( newStorageCapacity );
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

        _buffer.data = allocateAndConstructMemory( newBufferCapacity );
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
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::destroy( _Storage & storage ){
    util::destroy_range( storage.begin(), storage.end() );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
void AssocVector< _Key, _Mapped, _Cmp, _Alloc >::destroyAndFreeMemory( _Storage & storage )
{
    util::destroy_range( storage.begin(), storage.end() );
    _allocator.deallocate( storage.data, storage.capacity );
}

template<
      typename _Key
    , typename _Mapped
    , typename _Cmp
    , typename _Alloc
>
typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator
AssocVector< _Key, _Mapped, _Cmp, _Alloc >::allocateAndConstructMemory( std::size_t capacity )
{
    typename AssocVector< _Key, _Mapped, _Cmp, _Alloc >::_Storage::iterator result
        = static_cast< typename AssocVector< _Key, _Mapped, _Cmp, _Alloc > ::_Storage::iterator >
            ( _allocator.allocate( capacity ) );

    util::construct_range( result, result + capacity );

    return result;
}

#endif
