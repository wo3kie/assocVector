/*
 * Copyright (C) 2012 £ukasz Czerwiñski
 *
 * GitHub: https://github.com/wo3kie/AssocVector
 * Website: http://www.lukaszczerwinski.pl/assoc_vector.en.html
 *
 * Distributed under the BSD Software License (see file license)
 */

#ifndef __GXX_EXPERIMENTAL_CXX0X__
    #error C++11 is required to run this code, please use AssocVector 1.0.x instead.
#endif

#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define AV_ENABLE_EXTENSIONS
#include "AssocVector.hpp"

#ifdef AV_DEBUG
    #define _CORE_DUMP()\
        { int * core_dump = 0; * core_dump = 0; }

    #define _AV_ASSERT_CORE_DUMP( expression )\
        if( ( expression ) ){}else{ _CORE_DUMP() }

    #define _AV_ASSERT( expression )\
        assert( expression )

    #define AV_ASSERT( expression )\
        _AV_ASSERT_CORE_DUMP( expression )

    #define AV_ASSERT_EQUAL( actual, expected )\
        if( ( actual ) != ( expected ) ){ std::cout << "(" << actual << ") == " << expected << std::endl; AV_ASSERT( false );}

    #define AV_ASSERT_NOT_EQUAL( actual, expected )\
        if( ( actual ) == ( expected ) ){ std::cout << "(" << actual << ") != " << expected << std::endl; AV_ASSERT( false );}
#else
    #error Are you trying to run unit tests with assertions disabled ?
#endif

//
// dump
//
template< typename _Iterator >
void dump( _Iterator begin, _Iterator end, std::ostream & out = std::cout )
{
    out << "[";

    if( begin != end )
    {
        _Iterator current = begin;

        out << * current;

        for( ++ current ; current != end ; ++ current ){
            out << "," << *current ;
        }
    }

    out << "]";
}

template<
      typename _Iterator1
    , typename _Iterator2
>
void checkEqual(
      _Iterator1 first1
    , _Iterator1 const last1
    , _Iterator2 first2
    , _Iterator2 const last2
)
{
    AV_ASSERT_EQUAL( std::distance( first1, last1 ), std::distance( first2, last2 ) );
    AV_CHECK( std::equal( first1, last1, first2 ) );
}

template<
      typename _Iterator1
    , typename _Iterator2
>
void checkEqualBackward(
      _Iterator1 const first1
    , _Iterator1 last1
    , _Iterator2 const first2
    , _Iterator2 last2
)
{
    std::size_t const dist1 = std::distance( first1, last1 );
    std::size_t const dist2 = std::distance( first2, last2 );

    AV_ASSERT_EQUAL( dist1, dist2 );

    if( dist1 == 0 ){
        return;
    }

    do
    {
        -- last1;
        -- last2;

        AV_ASSERT_EQUAL( * last1, * last2 );
    }
    while( last1 != first1 );

    AV_ASSERT_EQUAL( * last1, * last2 );
}

//
// checkEqual
//
template<
      typename _T1
    , typename _T2
>
void checkEqual(
      AssocVector< _T1, _T2 > const & av
    , std::map< _T1, _T2 > const & map
)
{
    AV_ASSERT_EQUAL( av.empty(), map.empty() );
    AV_ASSERT_EQUAL( av.size(), map.size() );

    checkEqual( av.cbegin(), av.end(), map.cbegin(), map.cend() );
    checkEqual( av.crbegin(), av.rend(), map.rbegin(), map.rend() );

    checkEqualBackward( av.begin(), av.cend(), map.cbegin(), map.cend() );
    checkEqualBackward( av.rbegin(), av.crend(), map.rbegin(), map.rend() );
}

//
// Key
//
struct Key
{
    Key( int i = 0 )
    {
        ++ createdObjects;

        array.push_back( i );

        for( int i = 1 ; i < 4 ; ++ i ){
            array.push_back( rand() );
        }
    }

    Key( Key const & other )
        : array( other.array )
    {
        ++ createdObjects;
        ++ copies;
    }

    ~Key()
    {
        ++ destroyedObjects;
    }

    Key( Key && other )
        : array( std::move( other.array ) )
    {
        ++ createdObjects;
        ++ moves;
    }

    Key & operator=( Key const & other )
    {
        ++ copies;

        Key temp( other );
        this->swap( temp );

        return * this;
    }

    Key & operator=( Key && other )
    {
        ++ moves;

        array = std::move( other.array );

        return * this;
    }

    bool operator==( Key const & other )const
    {
        return array[0] == other.array[0];
    }

    bool operator!=( Key const & other )const
    {
        return ! operator==( other );
    }

    bool operator<( Key const & other )const
    {
        return array[0] < other.array[0];
    }

    void swap( Key & other )
    {
        array.swap( other.array );
    }

    std::vector< int > array;

    static unsigned createdObjects;
    static unsigned destroyedObjects;

    static unsigned copies;
    static unsigned moves;
};

unsigned Key::createdObjects = 0;

unsigned Key::destroyedObjects = 0;

unsigned Key::copies = 0;

unsigned Key::moves = 0;

std::ostream & operator<<( std::ostream & out, Key const & s3 )
{
    dump( s3.array.begin(), s3.array.end(), out );

    return out;
}

//
// Value
//
struct Value
{
    Value( int i = 0, std::string const & text = "" )
    {
        ++ createdObjects;

        array.push_back( i );
        this->text = text;

        for( int i = 1 ; i < 4 ; ++ i ){
            array.push_back( rand() );
        }
    }

    Value( Value const & other )
        : array( other.array )
        , text( other.text )
    {
        ++ createdObjects;
        ++ copies;
    }

    ~Value()
    {
        ++ destroyedObjects;
    }

    Value( Value && other )
        : array( std::move( other.array ) )
        , text( other.text )
    {
        ++ createdObjects;
        ++ moves;
    }

    Value & operator=( Value const & other )
    {
        ++ copies;

        Value temp( other );
        this->swap( temp );

        return * this;
    }

    Value & operator=( Value && other )
    {
        ++ moves;

        array = std::move( other.array );
        text = other.text;

        return * this;
    }

    bool operator==( Value const & other )const
    {
        return
               array[0] == other.array[0]
            && text == other.text;
    }

    bool operator!=( Value const & other )const
    {
        return ! operator==( other );
    }

    bool operator<( Value const & other )const
    {
        return array[0] < other.array[0];
    }

    void swap( Value & other )
    {
        array.swap( other.array );
    }

    std::vector< int > array;
    std::string text;

    static unsigned createdObjects;
    static unsigned destroyedObjects;

    static unsigned copies;
    static unsigned moves;
};

unsigned Value::createdObjects = 0;

unsigned Value::destroyedObjects = 0;

unsigned Value::copies = 0;

unsigned Value::moves = 0;

std::ostream & operator<<( std::ostream & out, Value const & s3 )
{
    dump( s3.array.begin(), s3.array.end(), out );

    return out;
}

//
// MyAllocator
//
template< typename _T >
struct MyAllocator
{
    typedef _T value_type;

    typedef _T * pointer;
    typedef _T const * const_pointer;

    typedef _T & reference;
    typedef _T const & const_reference;

    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    MyAllocator(){}

    MyAllocator( MyAllocator const & other )
        : _alloc( other._alloc )
    {
    }

    template< typename __T >
    MyAllocator( MyAllocator< __T > const & other )
        : _alloc( other._alloc )
    {
    }

    pointer allocate( unsigned n, const_pointer * hint = 0 )
    {
        totalMemory += n * sizeof( value_type );
        notFreedMemory += n * sizeof( value_type );

        return _alloc.allocate( n, hint );
    }

    void deallocate( pointer p, unsigned n )
    {
        notFreedMemory -= n * sizeof( value_type );

        _alloc.deallocate( p, n );
    }

    void construct( pointer p, value_type const & v )
    {
        _alloc.construct( p, v );
    }

    void construct( pointer p, value_type && v )
    {
        _alloc.construct( p, std::move( v ) );
    }

    void destroy( pointer p )
    {
        _alloc.destroy( p );
    }

    unsigned max_size()const throw()
    {
        return _alloc.max_size();
    }

    template< typename __T >
    struct rebind{ typedef MyAllocator< __T > other; };

    static int totalMemory;
    static int notFreedMemory;

    std::allocator< value_type > _alloc;
};

template< typename _T >
int MyAllocator< _T >::totalMemory = 0;

template< typename _T >
int MyAllocator< _T >::notFreedMemory = 0;


namespace std
{

template<
      typename _T1
    , typename _T2
>
std::ostream & operator<<( std::ostream & out, std::pair< _T1, _T2 > const & pair )
{
    return out << "(" << pair.first << "," << pair.second << ")";
}

}

//
// Range
//
template< typename _T = int >
struct Range
{
    Range( _T start, _T end, _T step = _T( 1 ) )
        : _start( start )
        , _end( end )
        , _step( step )
    {
    }

    _T start()const
    {
        return _start;
    }

    _T end()const
    {
        return _end;
    }

    _T step()const
    {
        return _step;
    }

private:
    _T _start;
    _T _end;
    _T _step;
};

//
// Unit Test Framework
//
template<
      typename _K
    , typename _M
>
struct TestCase
{
    TestCase( AssocVector< _K, _M > & av, std::map< _K, _M > & map )
        : _av( av )
        , _map( map )
    {
    }

    AssocVector< _K, _M > & av()
    {
        return _av;
    }

    AssocVector< _K, _M > const & av()const
    {
        return _av;
    }

    std::map< _K, _M > & map()
    {
        return _map;
    }

    std::map< _K, _M > const & map()const
    {
        return _map;
    }

private:
    AssocVector< _K, _M > & _av;
    std::map< _K, _M > & _map;
};

//
// Insert
//
template<
      typename _K
    , typename _M
>
struct Insert
{
    Insert( _K k, _M m )
        : _k( k )
        , _m( m )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > & av, std::map< __K, __M > & map )const
    {
        std::pair< typename AssocVector< __K, __M >::iterator, bool > inAV;
        std::pair< typename std::map< __K, __M >::iterator, bool > inMAP;

        {// do test
            inAV = av.insert( typename AssocVector< __K, __M >::value_type( _k, _m ) );
            inMAP = map.insert( typename std::map< __K, __M >::value_type( _k, _m ) );
        }

        {// validate
            AV_ASSERT_EQUAL( inAV.second, inMAP.second );

            checkEqual( av.begin(), inAV.first, map.begin(), inMAP.first );
            checkEqual( inAV.first, av.end(), inMAP.first, map.end() );

            checkEqual( av, map );
        }
    }

private:
    _K _k;
    _M _m;
};

template< typename _K >
Insert< _K, _K >
insert( _K k, _K m = _K() )
{
    return Insert< _K, _K >( k, m );
}

//
// InsertRange
//
template<
      typename _K
    , typename _M
>
struct InsertRange
{
    InsertRange( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > & av, std::map< __K, __M > & map )const
    {
        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            std::pair< typename AssocVector< __K, __M >::iterator, bool > inAV;
            std::pair< typename std::map< __K, __M >::iterator, bool > inMAP;

            {// do test
                inAV = av.insert( typename AssocVector< __K, __M >::value_type( k, k ) );
                inMAP = map.insert( typename std::map< __K, __M >::value_type( k, k ) );
            }

            {// validate
                AV_ASSERT_EQUAL( inAV.second, inMAP.second );

                checkEqual( av.begin(), inAV.first, map.begin(), inMAP.first );
                checkEqual( inAV.first, av.end(), inMAP.first, map.end() );

                checkEqual( av, map );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
InsertRange< _K, _K >
insert( Range< _K > const & range )
{
    return InsertRange< _K, _K >( range );
}

//
// EraseRange
//
template<
      typename _K
    , typename _M
>
struct EraseRange
{
    EraseRange( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > & av, std::map< __K, __M > & map )const
    {
        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            bool inAV;
            bool inMAP;

            {// do test
                inAV = av.erase( k );
                inMAP = map.erase( k );
            }

            {// validate
                AV_ASSERT_EQUAL( inAV, inMAP );

                checkEqual( av, map );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
EraseRange< _K, _K >
erase( _K k )
{
    return EraseRange< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
EraseRange< _K, _K >
erase( Range< _K > const & range )
{
    return EraseRange< _K, _K >( range );
}

//
// FindRange
//
template<
      typename _K
    , typename _M
>
struct FindRange
{
    FindRange( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > const & av, std::map< __K, __M > const & map )const
    {
        typename AssocVector< __K, __M >::const_iterator inAV;
        typename std::map< __K, __M >::const_iterator inMAP;

        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            {// do test
                inAV = av.find( k );
                inMAP = map.find( k );
            }

            {// validate
                checkEqual( av.begin(), inAV, map.begin(), inMAP );
                checkEqual( inAV, av.end(), inMAP, map.end() );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
FindRange< _K, _K >
find( _K k )
{
    return FindRange< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
FindRange< _K, _K >
find( Range< _K > const & range )
{
    return FindRange< _K, _K >( range );
}

//
// Find & Erase
//
template<
      typename _K
    , typename _M
>
struct FindErase
{
    FindErase( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > & av, std::map< __K, __M > & map )const
    {
        typename AssocVector< __K, __M >::iterator inAV;
        typename std::map< __K, __M >::iterator inMAP;

        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            {// do test
                inAV = av.find( k );
                inMAP = map.find( k );

                if( inAV == av.end() )
                {
                    AV_CHECK( inMAP == map.end() );

                    continue;
                }

                inAV = av.erase( inAV );
                inMAP = map.erase( inMAP );
            }

            {// validate
                checkEqual( av.begin(), inAV, map.begin(), inMAP );
                checkEqual( inAV, av.end(), inMAP, map.end() );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
FindErase< _K, _K >
findErase( _K k )
{
    return FindErase< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
FindErase< _K, _K >
findErase( Range< _K > const & range )
{
    return FindErase< _K, _K >( range );
}

//
// CountRange
//
template<
      typename _K
    , typename _M
>
struct CountRange
{
    CountRange( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > const & av, std::map< __K, __M > const & map )const
    {
        typename AssocVector< __K, __M >::const_iterator inAV;
        typename std::map< __K, __M >::const_iterator inMAP;

        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            {// do test
                AV_ASSERT_EQUAL( av.count( k ), map.count( k ) );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
CountRange< _K, _K >
count( _K k )
{
    return CountRange< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
CountRange< _K, _K >
count( Range< _K > const & range )
{
    return CountRange< _K, _K >( range );
}

//
// LowerBound
//
template<
      typename _K
    , typename _M
>
struct LowerBound
{
    LowerBound( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > const & av, std::map< __K, __M > const & map )const
    {
        typename AssocVector< __K, __M >::const_iterator inAV;
        typename std::map< __K, __M >::const_iterator inMAP;

        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            {// do test
                inAV = av.lower_bound( k );
                inMAP = map.lower_bound( k );
            }

            {// validate
                checkEqual( av.begin(), inAV, map.begin(), inMAP );
                checkEqual( inAV, av.end(), inMAP, map.end() );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
LowerBound< _K, _K >
lower_bound( _K k )
{
    return LowerBound< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
LowerBound< _K, _K >
lower_bound( Range< _K > const & range )
{
    return LowerBound< _K, _K >( range );
}

//
// UpperBound
//
template<
      typename _K
    , typename _M
>
struct UpperBound
{
    UpperBound( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > const & av, std::map< __K, __M > const & map )const
    {
        typename AssocVector< __K, __M >::const_iterator inAV;
        typename std::map< __K, __M >::const_iterator inMAP;

        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            {// do test
                inAV = av.upper_bound( k );
                inMAP = map.upper_bound( k );
            }

            {// validate
                checkEqual( av.begin(), inAV, map.begin(), inMAP );
                checkEqual( inAV, av.end(), inMAP, map.end() );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
UpperBound< _K, _K >
upper_bound( _K k )
{
    return UpperBound< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
UpperBound< _K, _K >
upper_bound( Range< _K > const & range )
{
    return UpperBound< _K, _K >( range );
}

//
// EqualRange
//
template<
      typename _K
    , typename _M
>
struct EqualRange
{
    EqualRange( Range< _K > const & range )
        : _range( range )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > const & av, std::map< __K, __M > const & map )const
    {
        typedef typename AssocVector< __K, __M >::const_iterator AVConstIterator;
        typedef typename std::map< __K, __M >::const_iterator MAPConstIterator;

        std::pair< AVConstIterator, AVConstIterator > inAV;
        std::pair< MAPConstIterator, MAPConstIterator > inMAP;

        for( _K k = _range.start() ; k != _range.end() ; k += _range.step() )
        {
            {// do test
                inAV = av.equal_range( k );
                inMAP = map.equal_range( k );
            }

            {// validate
                checkEqual( av.begin(), inAV.first, map.begin(), inMAP.first );
                checkEqual( av.begin(), inAV.second, map.begin(), inMAP.second );

                checkEqual( inAV.first, av.end(), inMAP.first, map.end() );
                checkEqual( inAV.second, av.end(), inMAP.second, map.end() );
            }
        }
    }

private:
    Range< _K > _range;
};

template< typename _K >
EqualRange< _K, _K >
equal_range( _K k )
{
    return EqualRange< _K, _K >( Range< _K >( k, k + _K( 1 ) ) );
}

template< typename _K >
EqualRange< _K, _K >
equal_range( Range< _K > const & range )
{
    return EqualRange< _K, _K >( range );
}

//
// IndexGet
//
template< typename _K >
struct IndexGet
{
    IndexGet( _K k )
        : _k( k )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > & av, std::map< __K, __M > & map )const
    {
        {// do test
            AV_ASSERT_EQUAL( av[ _k ], map[ _k ] );
        }

        {// validate
            checkEqual( av, map );
        }
    }

private:
    _K _k;
};

template< typename _K >
IndexGet< _K >
index_get( _K k )
{
    return IndexGet< _K >( k );
}

//
// IndexPut
//
template<
      typename _K
    , typename _M
>
struct IndexPut
{
    IndexPut( _K k, _M m )
        : _k( k )
        , _m( m )
    {
    }

    template<
          typename __K
        , typename __M
    >
    void run( AssocVector< __K, __M > & av, std::map< __K, __M > & map )const
    {
        {// do test
            av[ _k ] = _m;
            map[ _k ] = _m;
        }

        {// validate
            checkEqual( av, map );
        }
    }

private:
    _K _k;
    _M _m;
};

template< typename _K >
IndexPut< _K, _K >
index_put( _K k, _K m = _K() )
{
    return IndexPut< _K, _K >( k, m );
}

//
// operator>>
//
template<
      typename _K1
    , typename _M1
    , typename _TestItem
>
TestCase< _K1, _M1 > &
operator>> ( TestCase< _K1, _M1 > & test, _TestItem const & item )
{
    item.run( test.av(), test.map() );

    return test;
}

//
// Unit tests
//

//
// test_CmpByFirst
//
void test_CmpByFirst_1()
{
    typedef std::pair< int, int > Pii;

    Pii p1( 1, 11 );
    Pii p2( 1, 22 );

    AV_ASSERT( ( util::CmpByFirst< Pii, std::equal_to< int > >()( p1, p2 ) ) );

    AV_ASSERT( ( util::CmpByFirst< Pii, std::equal_to< int > >()( p1, 1 ) ) );
}

//
// test_CmpByFirst_2
//
void test_CmpByFirst_2()
{
        typedef std::pair< int, int > Pii;

        Pii p1( 1, 22 );
        Pii p2( 2, 11 );

        AV_ASSERT( ( util::CmpByFirst< Pii, std::less< int > >()( p1, p2 ) ) );
        AV_ASSERT( ( util::CmpByFirst< Pii, std::less< int > >()( p1, 2 ) ) );

        AV_ASSERT_EQUAL( ( util::CmpByFirst< Pii, std::greater< int > >()( p1, p2 ) ), false );
}

//
// test_CmpByFirst_3
//
void test_CmpByFirst_3()
{
    typedef std::pair< int, int > Pii;

    Pii p1( 2, 11 );
    Pii p2( 1, 22 );

    AV_ASSERT( ( util::CmpByFirst< Pii, std::greater< int > >()( p1, p2 ) ) );
    AV_ASSERT( ( util::CmpByFirst< Pii, std::greater< int > >()( p1, 1 ) ) );

    AV_ASSERT_EQUAL( ( util::CmpByFirst< Pii, std::less< int > >()( p1, p2 ) ), false );
}

//
// test_isBetween
//
void test_isBetween()
{
    AV_ASSERT_EQUAL( util::is_between( 1, 0, 3 ), false );
    AV_ASSERT( ( util::is_between( 1, 1, 3 ) ) );
    AV_ASSERT( ( util::is_between( 1, 2, 3 ) ) );
    AV_ASSERT( ( util::is_between( 1, 3, 3 ) ) );
    AV_ASSERT_EQUAL( util::is_between( 1, 4, 3 ), false )    ;
}

//
// test_move
//
void test_move_empty_array()
{
    std::vector< int > array;
    std::vector< int > array2;

    util::move( array.begin(), array.end(), array2.begin() );

    AV_ASSERT( array2.empty() );
}

//
// test_move_self_copy
//
void test_move_self_copy()
{
    std::vector< int > array( { 1, 2, 3, 4, 5, 6 } );


    util::move( array.begin(), array.end(), array.begin() );

    std::vector< int > expected( { 1, 2, 3, 4, 5, 6 } );

    AV_ASSERT( array == expected );
}

//
// test_move_between_different_containers
//
void test_move_between_different_containers()
{
    std::vector< int > array( { 1, 2, 3, 4, 5, 6 } );

    std::vector< int > array2( 6, 0 );

    util::move( array.begin(), array.end(), array2.begin() );

    AV_ASSERT( array == array2 );
}

//
// test_move_overlap_less_then_half
//
void test_move_overlap_less_then_half()
{
    std::vector< int > array( { 1, 2, 3, 4, 5, 6 } );

    util::move( array.begin(), array.begin() + 3, array.begin() + 3 );

    std::vector< int > expected( { 1, 2, 3, 1, 2, 3 } );

    AV_ASSERT( array == expected );
}

//
// test_move_overlap_more_than_half
//
void test_move_overlap_more_than_half()
{
    std::vector< int > array( { 1, 2, 3, 4, 5, 6 } );

    util::move( array.begin(), array.begin() + 4, array.begin() + 2 );

    std::vector< int > expected( { 1, 2, 1, 2, 3, 4 } );

    AV_ASSERT( array == expected );
}

//
// test_move_overlap_copy_to_begining
//
void test_move_overlap_copy_to_begining()
{
    std::vector< int > array( { 1, 2, 3, 4, 5, 6 } );

    util::move( array.begin() + 2, array.end(), array.begin() );

    std::vector< int > expected( { 3, 4, 5, 6, 5, 6 } );

    AV_ASSERT( array == expected );
}

//
// test_last_less_equal
//
void test_last_less_equal()
{
    {
        std::vector< int > v;

        AV_ASSERT( util::last_less_equal( v.begin(), v.end(), 1, std::less< int >() ) == v.end() );
    }

    {
        std::vector< int > v( { 1, 3, 5, 7, 9 } );

        {
            int i = 1;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), i );

            i = 3;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), i );

            i = 5;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) !=v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), i );

            i = 7;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), i );

            i = 9;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), i );
        }
        {
            int i = 0;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == v.end() );

            i = 2;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), (i-1) );

            i = 4;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), (i-1) );

            i = 6;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), (i-1) );

            i = 8;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), (i-1) );

            i = 10;
            AV_ASSERT( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            AV_ASSERT_EQUAL( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ), (i-1) );
        }
    }
}

//
// test_merge_1
//
void test_merge_1()
{
    using array::Array;

    {
        Array< int > a1;
        a1.reserve( 10 );

        Array< int > a2;
        a2.reserve( 10 );

        for( int i : { 1, 2, 3, 4 } ){
            a2.place_back( i );
        }

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 4 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
    }

    {
        Array< int > a1;
        a1.reserve( 10 );

        for( int i : { 1, 2, 3, 4 } ){
            a1.place_back( i );
        }

        Array< int > a2;
        a2.reserve( 10 );

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 4 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 0 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
    }

    {
        Array< int > a1;
        a1.reserve( 10 );

        for( int i : { 1, 2, 3, 4, 5, 6 } ){
            a1.place_back( i );
        }

        Array< int > a2;
        a2.reserve( 10 );

        for( int i : { 7, 8, 9, 10 } ){
            a2.place_back( i );
        }

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 10 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
        AV_ASSERT_EQUAL( a1[4], 5 );
        AV_ASSERT_EQUAL( a1[5], 6 );
        AV_ASSERT_EQUAL( a1[6], 7 );
        AV_ASSERT_EQUAL( a1[7], 8 );
        AV_ASSERT_EQUAL( a1[8], 9 );
        AV_ASSERT_EQUAL( a1[9], 10 );
    }

    {
        Array< int > a1;
        a1.reserve( 10 );

        for( int i : { 1, 3, 5, 7, 9, 11 } ){
            a1.place_back( i );
        }

        Array< int > a2;
        a2.reserve( 10 );

        for( int i : { 2, 4, 6, 8 } ){
            a2.place_back( i );
        }

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 10 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
        AV_ASSERT_EQUAL( a1[4], 5 );
        AV_ASSERT_EQUAL( a1[5], 6 );
        AV_ASSERT_EQUAL( a1[6], 7 );
        AV_ASSERT_EQUAL( a1[7], 8 );
        AV_ASSERT_EQUAL( a1[8], 9 );
        AV_ASSERT_EQUAL( a1[9], 11 );
    }

    {
        Array< int > a1;
        a1.reserve( 10 );

        for( int i : { 5, 6, 7, 8, 9, 10 } ){
            a1.place_back( i );
        }

        Array< int > a2;
        a2.reserve( 10 );

        for( int i : { 1, 2, 3, 4 } ){
            a2.place_back( i );
        }

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 10 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
        AV_ASSERT_EQUAL( a1[4], 5 );
        AV_ASSERT_EQUAL( a1[5], 6 );
        AV_ASSERT_EQUAL( a1[6], 7 );
        AV_ASSERT_EQUAL( a1[7], 8 );
        AV_ASSERT_EQUAL( a1[8], 9 );
        AV_ASSERT_EQUAL( a1[9], 10 );
    }

    {
        Array< int > a1;
        a1.reserve( 10 );

        for( int i : { 1, 2, 3, 8, 9, 10 } ){
            a1.place_back( i );
        }

        Array< int > a2;
        a2.reserve( 10 );

        for( int i : { 4, 5, 6, 7 } ){
            a2.place_back( i );
        }

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 10 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
        AV_ASSERT_EQUAL( a1[4], 5 );
        AV_ASSERT_EQUAL( a1[5], 6 );
        AV_ASSERT_EQUAL( a1[6], 7 );
        AV_ASSERT_EQUAL( a1[7], 8 );
        AV_ASSERT_EQUAL( a1[8], 9 );
        AV_ASSERT_EQUAL( a1[9], 10 );
    }

    {
        Array< int > a1;
        a1.reserve( 10 );

        for( int i : { 2, 3, 5, 6, 8, 9 } ){
            a1.place_back( i );
        }

        Array< int > a2;
        a2.reserve( 10 );

        for( int i : { 1, 4, 7, 10 } ){
            a2.place_back( i );
        }

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 10 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );
        AV_ASSERT_EQUAL( a1[4], 5 );
        AV_ASSERT_EQUAL( a1[5], 6 );
        AV_ASSERT_EQUAL( a1[6], 7 );
        AV_ASSERT_EQUAL( a1[7], 8 );
        AV_ASSERT_EQUAL( a1[8], 9 );
        AV_ASSERT_EQUAL( a1[9], 10 );
    }
}

//
// test_merge_2
//
void test_merge_2()
{
    array::Array< std::pair< int, int > > storage;

    storage.reserve( 10 );

    for( int i = 10 ; i < 15 ; ++ i ){
        storage.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.reserve( 3 );

    for( int i = 3 ; i < 6 ; ++ i ){
        buffer.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 8 );

    AV_ASSERT_EQUAL( buffer.size(), 3 );
    AV_ASSERT_EQUAL( buffer.capacity(), 3 );

    AV_ASSERT_EQUAL( storage[0].first, 3 );
    AV_ASSERT_EQUAL( storage[1].first, 4 );
    AV_ASSERT_EQUAL( storage[2].first, 5 );
    AV_ASSERT_EQUAL( storage[3].first, 10 );
    AV_ASSERT_EQUAL( storage[4].first, 11 );
    AV_ASSERT_EQUAL( storage[5].first, 12 );
    AV_ASSERT_EQUAL( storage[6].first, 13 );
    AV_ASSERT_EQUAL( storage[7].first, 14 );
}

//
// test_merge_3
//
void test_merge_3()
{
    array::Array< std::pair< int, int > > storage;

    storage.reserve( 10 );

    for( int i = 10 ; i < 15 ; ++ i ){
        storage.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.reserve( 3 );

    for( int i = 23 ; i < 26 ; ++ i ){
        buffer.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 8 );

    AV_ASSERT_EQUAL( buffer.size(), 3 );
    AV_ASSERT_EQUAL( buffer.capacity(), 3 );

    AV_ASSERT_EQUAL( storage[0].first, 10 );
    AV_ASSERT_EQUAL( storage[1].first, 11 );
    AV_ASSERT_EQUAL( storage[2].first, 12 );
    AV_ASSERT_EQUAL( storage[3].first, 13 );
    AV_ASSERT_EQUAL( storage[4].first, 14 );
    AV_ASSERT_EQUAL( storage[5].first, 23 );
    AV_ASSERT_EQUAL( storage[6].first, 24 );
    AV_ASSERT_EQUAL( storage[7].first, 25 );
}

//
// test_merge_4
//
void test_merge_4()
{
    array::Array< std::pair< int, int > > storage;

    storage.reserve( 10 );

    for( int i = 10 ; i < 20 ; i += 2 ){
        storage.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.reserve( 10 );

    for( int i = 13 ; i < 19 ; i += 2 ){
        buffer.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 8 );

    AV_ASSERT_EQUAL( buffer.size(), 3 );
    AV_ASSERT_EQUAL( buffer.capacity(), 10 );

    AV_ASSERT_EQUAL( storage[0].first, 10 );
    AV_ASSERT_EQUAL( storage[1].first, 12 );
    AV_ASSERT_EQUAL( storage[2].first, 13 );
    AV_ASSERT_EQUAL( storage[3].first, 14 );
    AV_ASSERT_EQUAL( storage[4].first, 15 );
    AV_ASSERT_EQUAL( storage[5].first, 16 );
    AV_ASSERT_EQUAL( storage[6].first, 17 );
    AV_ASSERT_EQUAL( storage[7].first, 18 );
}

//
// test_merge_5
//
void test_merge_5()
{
    array::Array< std::pair< int, int > > storage;

    storage.reserve( 10 );

    for( int i = 10 ; i < 20 ; i += 2 ){
        storage.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.reserve( 3 );

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 5 );

    AV_ASSERT_EQUAL( buffer.size(), 0 );
    AV_ASSERT_EQUAL( buffer.capacity(), 3 );

    AV_ASSERT_EQUAL( storage[0].first, 10 );
    AV_ASSERT_EQUAL( storage[1].first, 12 );
    AV_ASSERT_EQUAL( storage[2].first, 14 );
    AV_ASSERT_EQUAL( storage[3].first, 16 );
    AV_ASSERT_EQUAL( storage[4].first, 18 );
}

//
// test_merge_6
//
void test_merge_6()
{
    array::Array< std::pair< int, int > > storage;

    storage.reserve( 10 );

    array::Array< std::pair< int, int > > buffer;

    buffer.reserve( 20 );

    for( int i = 13 ; i < 19 ; i += 2 ){
        buffer.place_back( std::pair< int, int >( i, 0 ) );
    }

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 3 );

    AV_ASSERT_EQUAL( buffer.size(), 3 );
    AV_ASSERT_EQUAL( buffer.capacity(), 20 );

    AV_ASSERT_EQUAL( storage[0].first, 13 );
    AV_ASSERT_EQUAL( storage[1].first, 15 );
    AV_ASSERT_EQUAL( storage[2].first, 17 );
}

//
// test_merge_7
//
void test_merge_7()
{
    array::Array< std::pair< int, int > > storage;
    storage.reserve( 10 );

    array::Array< std::pair< int, int > > buffer;
    buffer.reserve( 3 );

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 0 );

    AV_ASSERT_EQUAL( buffer.size(), 0 );
    AV_ASSERT_EQUAL( buffer.capacity(), 3 );
}

//
// test_empty_container
//
void test_empty_container()
{
    {
        AssocVector< Key, Value > av;

        AV_ASSERT( av.empty() );
        AV_ASSERT_EQUAL( av.size(), 0 );
        AV_ASSERT_EQUAL( av.capacity(), 0 );

        AV_ASSERT( av.begin() == av.end() );
        AV_ASSERT( av.cbegin() == av.cend() );
        AV_ASSERT( av.rbegin() == av.rend() );
        AV_ASSERT( av.crbegin() == av.crend() );
    }

    {
        std::allocator< std::pair< Key, Value > > allocator;
        AssocVector< Key, Value > av( allocator );

        AV_ASSERT( av.empty() );
        AV_ASSERT_EQUAL( av.size(), 0 );
        AV_ASSERT_EQUAL( av.capacity(), 0 );

        AV_ASSERT( av.begin() == av.end() );
        AV_ASSERT( av.cbegin() == av.cend() );
        AV_ASSERT( av.rbegin() == av.rend() );
        AV_ASSERT( av.crbegin() == av.crend() );
    }

    {
        std::less< Key > cmp;
        std::allocator< std::pair< Key, Value > > allocator;
        AssocVector< Key, Value > av( cmp, allocator );

        AV_ASSERT( av.empty() );
        AV_ASSERT_EQUAL( av.size(), 0 );
        AV_ASSERT_EQUAL( av.capacity(), 0 );

        AV_ASSERT( av.begin() == av.end() );
        AV_ASSERT( av.cbegin() == av.cend() );
        AV_ASSERT( av.rbegin() == av.rend() );
        AV_ASSERT( av.crbegin() == av.crend() );
    }
}

//
// test_empty_container_push_back
//
void test_empty_container_push_back()
{
    AssocVector< Key, Value > av;

    {
        av[ 2 ] = 2;

        AV_ASSERT( av.bufferSize() <= av.bufferCapacity() );
        AV_ASSERT( av.storageSize() <= av.storageCapacity() );
        AV_ASSERT( av.erasedSize() <= av.erasedCapacity() );
    }

    {
        av[ 1 ] = 1;

        AV_ASSERT( av.bufferSize() <= av.bufferCapacity() );
        AV_ASSERT( av.storageSize() <= av.storageCapacity() );
        AV_ASSERT( av.erasedSize() <= av.erasedCapacity() );
    }

    {
        av[ 3 ] = 3;

        AV_ASSERT( av.bufferSize() <= av.bufferCapacity() );
        AV_ASSERT( av.storageSize() <= av.storageCapacity() );
        AV_ASSERT( av.erasedSize() <= av.erasedCapacity() );
    }
}

//
// test_insert_in_increasing_order
//
void test_insert_in_increasing_order()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 32 ) );
}

//
// test_insert_in_decreasing_order
//
void test_insert_in_decreasing_order()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 32, -1, -1 ) );
}

//
// test_insert_in_random_order
//
void test_insert_in_random_order()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( 10 )
        >> insert( 9 )
        >> insert( 8 )
        >> insert( 16 )
        >> insert( 17 )
        >> insert( 17 )
        >> insert( 18 )
        >> insert( 7 )
        >> insert( 7 )
        >> insert( 6 )
        >> insert( 4 )
        >> insert( 3 )
        >> insert( 11 )
        >> insert( 12 )
        >> insert( 13 )
        >> insert( 2 )
        >> insert( 1 )
        >> insert( 0 )
        >> insert( 14 )
        >> insert( 15 )
        >> insert( 19 )
        >> insert( 5 );
}

//
// insert_init_list
//
void test_insert_init_list()
{
    typedef AssocVector< std::string, int > AssocVector;

    AssocVector av;
    av.insert( { { "1", 1 }, { "4", 4 }, { "2", 2 }, { "5", 5 }, { "3", 3 }, { "2", 2 } } );

    AV_ASSERT_EQUAL( av.size(), 5 );

    AssocVector::const_iterator found;

    found = av.find( "1" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "1" );
    AV_ASSERT_EQUAL( found->second, 1 );

    found = av.find( "2" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "2" );
    AV_ASSERT_EQUAL( found->second, 2 );

    found = av.find( "3" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "3" );
    AV_ASSERT_EQUAL( found->second, 3 );

    found = av.find( "4" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "4" );
    AV_ASSERT_EQUAL( found->second, 4 );

    found = av.find( "5" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "5" );
    AV_ASSERT_EQUAL( found->second, 5 );
}

//
// test_erase_in_increasing_order
//
void test_erase_in_increasing_order()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 32 ) )
        >> erase( Range<>( 0, 32 ) );
}

//
// test_erase_in_decreasing_order
//
void test_erase_in_decreasing_order()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 32, -1, -1 ) )
        >> erase( Range<>( 32, -1, -1 ) );
}

//
// test_erase_in_random_order
//
void test_erase_in_random_order()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 21 ) )
        >> erase( 10 )
        >> erase( 9 )
        >> erase( 8 )
        >> erase( 16 )
        >> erase( 17 )
        >> erase( 17 )
        >> erase( 18 )
        >> erase( 7 )
        >> erase( 7 )
        >> erase( 6 )
        >> erase( 4 )
        >> erase( 3 )
        >> erase( 11 )
        >> erase( 12 )
        >> erase( 13 )
        >> erase( 2 )
        >> erase( 1 )
        >> erase( 0 )
        >> erase( 14 )
        >> erase( 15 )
        >> erase( 19 )
        >> erase( 5 );
}

//
// test_insert_insert
//
void test_insert_insert()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( 1, 11 )
        >> insert( 2, 22 )
        >> insert( 3, 33 )
        >> insert( 4, 44 )

        >> insert( 1, 111 )
        >> insert( 2, 222 )
        >> insert( 3, 333 )
        >> insert( 4, 444 );
}

//
// test_insert_erase_erase
//
void test_insert_erase_erase()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 50 ) )

        >> erase( Range<>( 40, 45 ) )
        >> erase( Range<>( 35, 50 ) )

        >> erase( Range<>( 5, 15 ) )
        >> erase( Range<>( 0, 20 ) )

        >> erase( Range<>( 25, 30 ) )

        >> erase( Range<>( 0, 50 ) );
}

//
// test_insert_erase_insert
//
void test_insert_erase_insert()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 50, 2 ) )
        >> insert( Range<>( 1, 51, 2 ) )

        >> erase( Range<>( 30, 40, 2 ) )
        >> erase( Range<>( 31, 41, 2 ) )

        >> insert( Range<>( 30, 40, 2 ) )
        >> insert( Range<>( 31, 41, 2 ) )

        >> erase( Range<>( 10, 20, 2 ) )
        >> erase( Range<>( 11, 21, 2 ) )

        >> insert( Range<>( 10, 20, 2 ) )
        >> insert( Range<>( 11, 21, 2 ) );
}

//
// test_find
//
void test_find()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 32, 2 ) )
        >> find( Range<>( 0, 32, 2 ) )
        >> find( Range<>( 1, 33, 2 ) );
}

//
// test_find_erase_find
//
void test_find_erase_find()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 32, 2 ) )

        >> erase( 31 )
        >> find( 31 )

        >> erase( 1 )
        >> find( 1 )

        >> erase( Range<>( 15, 25 ) )
        >> find( Range<>( 15, 25 ) );
}

//
// test_find_erase_find_insert_find
//
void test_find_erase_find_insert_find()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 30, 2 ) )

        >> find( Range<>( 0, 10 ) )
        >> erase( Range<>( 0, 10 ) )
        >> insert( Range<>( 0, 10 ) )
        >> find( Range<>( 0, 10 ) )

        >> find( Range<>( 10, 20 ) )
        >> erase( Range<>( 10, 20 ) )
        >> insert( Range<>( 10, 20 ) )
        >> find( Range<>( 10, 20 ) )

        >> find( Range<>( 20, 30 ) )
        >> erase( Range<>( 20, 30 ) )
        >> insert( Range<>( 20, 30 ) )
        >> find( Range<>( 20, 30 ) );
}

//
// test_count
//
void test_count()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 0, 30, 2 ) )

        >> count( Range<>( 0, 10 ) )
        >> erase( Range<>( 0, 10 ) )
        >> insert( Range<>( 0, 10 ) )
        >> count( Range<>( 0, 10 ) )

        >> count( Range<>( 10, 20 ) )
        >> erase( Range<>( 10, 20 ) )
        >> insert( Range<>( 10, 20 ) )
        >> count( Range<>( 10, 20 ) )

        >> count( Range<>( 20, 30 ) )
        >> erase( Range<>( 20, 30 ) )
        >> insert( Range<>( 20, 30 ) )
        >> count( Range<>( 20, 30 ) );
}

//
// test_lower_bound
//
void test_lower_bound()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 30, 10, -1 ) )
        >> erase( 11 )
        >> erase( 20 )
        >> erase( 28 )

        >> lower_bound( Range<>( 0, 40, 2 ) )
        >> lower_bound( Range<>( 1, 41, 2 ) );
}

//
// test_upper_bound
//
void test_upper_bound()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 30, 10, -1 ) )
        >> erase( 11 )
        >> erase( 20 )
        >> erase( 28 )

        >> upper_bound( Range<>( 0, 40, 2 ) )
        >> upper_bound( Range<>( 1, 41, 2 ) );
}

//
// test_equal_range
//
void test_equal_range()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> insert( Range<>( 30, 10, -1 ) )
        >> erase( 11 )
        >> erase( 20 )
        >> erase( 28 )

        >> equal_range( Range<>( 0, 40, 2 ) )
        >> equal_range( Range<>( 1, 41, 2 ) );
}

//
// test_swap
//
void test_swap()
{
    // todo
}

//
// test_operator_index
//
void test_operator_index()
{
    AssocVector< Key, Value > av;
    std::map< Key, Value > map;

    TestCase< Key, Value > test( av, map );

    test
        >> index_put( 1, 1 )
        >> index_put( 1, 11 )
        >> index_put( 1, 111 )

        >> index_get( 2 )

        >> insert( 3, 33 )
        >> index_get( 3 )

        >> erase( 3 )
        >> index_get( 3 );
}

//
// test_user_type
//
void test_user_type()
{
    typedef AssocVector< Key, Value > AssocVector;

    AssocVector av;
    av.insert( AssocVector::value_type( Key(), Value() ) );
    Value & m = av[ Key() ];
    av[ Key() ] = m;
    av.find( Key() );
}

//
// test_types
//
void test_types()
{
    typedef AssocVector< Key, Value > AV;

    sizeof( AV::key_type );
    sizeof( AV::mapped_type );
    sizeof( AV::value_type );
    sizeof( AV::key_compare );
    sizeof( AV::value_compare );
    sizeof( AV::allocator_type );
    sizeof( AV::reference );
    sizeof( AV::const_reference );
    sizeof( AV::iterator );
    sizeof( AV::const_iterator );
    sizeof( AV::size_type );
    sizeof( AV::difference_type );
    sizeof( AV::pointer );
    sizeof( AV::const_pointer );
    sizeof( AV::reverse_iterator );
    sizeof( AV::const_reverse_iterator );
}

//
// test_constructor
//
void test_constructor()
{
    typedef std::vector< std::pair< std::string, int > > Vector;

    Vector v;
    v.push_back( std::make_pair( "1", 1 ) );
    v.push_back( std::make_pair( "2", 2 ) );
    v.push_back( std::make_pair( "3", 3 ) );
    v.push_back( std::make_pair( "4", 4 ) );
    v.push_back( std::make_pair( "5", 5 ) );

    typedef AssocVector< std::string, int > AssocVector;

    AssocVector av( v.begin(), v.end() );

    AV_ASSERT_EQUAL( av.size(), 5 );

    AssocVector::const_iterator found;

    found = av.find( "1" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "1" );
    AV_ASSERT_EQUAL( found->second, 1 );

    found = av.find( "2" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "2" );
    AV_ASSERT_EQUAL( found->second, 2 );

    found = av.find( "3" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "3" );
    AV_ASSERT_EQUAL( found->second, 3 );

    found = av.find( "4" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "4" );
    AV_ASSERT_EQUAL( found->second, 4 );

    found = av.find( "5" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "5" );
    AV_ASSERT_EQUAL( found->second, 5 );
}

//
// test_copy_constructor
//
void test_copy_constructor()
{
    typedef AssocVector< std::string, int > AssocVector;

    AssocVector assocVector1;

    assocVector1[ "a" ] = 1;
    assocVector1[ "b" ] = 2;
    assocVector1[ "c" ] = 3;
    assocVector1[ "d" ] = 4;
    assocVector1[ "e" ] = 5;

    AssocVector assocVector2( assocVector1 );
    AV_ASSERT( assocVector1 == assocVector2 );
}

//
// test_constructor_init_list
//
void test_constructor_init_list()
{
    typedef AssocVector< std::string, int > AssocVector;

    AssocVector av( { { "1", 1 }, { "4", 4 }, { "2", 2 }, { "5", 5 }, { "3", 3 }, { "2", 2 } } );

    AV_ASSERT_EQUAL( av.size(), 5 );

    AssocVector::const_iterator found;

    found = av.find( "1" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "1" );
    AV_ASSERT_EQUAL( found->second, 1 );

    found = av.find( "2" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "2" );
    AV_ASSERT_EQUAL( found->second, 2 );

    found = av.find( "3" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "3" );
    AV_ASSERT_EQUAL( found->second, 3 );

    found = av.find( "4" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "4" );
    AV_ASSERT_EQUAL( found->second, 4 );

    found = av.find( "5" );
    AV_ASSERT( found != av.end() );
    AV_ASSERT_EQUAL( found->first, "5" );
    AV_ASSERT_EQUAL( found->second, 5 );
}

//
// test_assign_operator
//
void test_assign_operator()
{
    typedef AssocVector< std::string, int > AssocVector;

    AssocVector assocVector1;

    assocVector1[ "a" ] = 1;
    assocVector1[ "b" ] = 2;
    assocVector1[ "c" ] = 3;
    assocVector1[ "d" ] = 4;
    assocVector1[ "e" ] = 5;

    AssocVector assocVector2;
    assocVector2 = assocVector1;

    AV_ASSERT( assocVector1 == assocVector2 );
}

//
// test_clear
//
void test_clear()
{
    typedef AssocVector< std::string, int > AssocVector;

    AssocVector assocVector;

    assocVector[ "a" ] = 1;
    assocVector[ "b" ] = 2;
    assocVector[ "c" ] = 3;
    assocVector[ "d" ] = 4;
    assocVector[ "e" ] = 5;

    AV_ASSERT_EQUAL( assocVector.erase( "a" ), 1 );

    AV_ASSERT_EQUAL( assocVector.size(), 4 );

    assocVector.clear();

    AV_ASSERT( assocVector.empty() );
}

//
// test_iterator_to_const_iterator_conversion
//
void test_iterator_to_const_iterator_conversion()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator iterator;
    typedef AssocVector::const_iterator const_iterator;

    AssocVector av;

    iterator i = av.find( 10 );
    AV_ASSERT( i == av.end() );

    const_iterator ci = av.find( 10 );
    AV_ASSERT( ci == av.end() );
}

//
// test_iterators_distance
//
void test_iterators_distance()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    AV_ASSERT( av.begin() == av.end() );
    AV_ASSERT_EQUAL( std::distance( av.begin(), av.end() ), 0 );

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT( av.begin() != av.end() );
    AV_ASSERT_EQUAL( std::distance( av.begin(), av.end() ), 1 );

    av.insert( AssocVector::value_type( 2, 2 ) );
    AV_ASSERT_EQUAL( std::distance( av.begin(), av.end() ), 2 );

    av.insert( AssocVector::value_type( 3, 3 ) );
    AV_ASSERT_EQUAL( std::distance( av.begin(), av.end() ), 3 );
}

//
// test_reverse_iterators_distance
//
void test_reverse_iterators_distance()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    AV_ASSERT( av.rbegin() == av.rend() );
    AV_ASSERT_EQUAL( std::distance( av.rbegin(), av.rend() ), 0 );

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT( av.rbegin() != av.rend() );
    AV_ASSERT_EQUAL( std::distance( av.rbegin(), av.rend() ), 1 );

    av.insert( AssocVector::value_type( 2, 2 ) );
    AV_ASSERT_EQUAL( std::distance( av.rbegin(), av.rend() ), 2 );

    av.insert( AssocVector::value_type( 3, 3 ) );
    AV_ASSERT_EQUAL( std::distance( av.rbegin(), av.rend() ), 3 );
}

//
// test_reverse_iterators_base
//
void test_reverse_iterators_base()
{
    typedef AssocVector< int, int > AssocVector;
    typedef std::map< int, int > Map;

    AssocVector av;
    av[1] = 9;
    av[0] = 7;
    av[7] = 9;
    av[2] = 4;

    Map map;
    map[1] = 9;
    map[0] = 7;
    map[7] = 9;
    map[2] = 4;

    AV_ASSERT( av.begin() == av.rend().base() );
    AV_ASSERT( av.end() == av.rbegin().base() );

    AssocVector::reverse_iterator avRBegin = av.rbegin();
    av.erase( -- avRBegin.base() );

    Map::reverse_iterator  mapRBegin = map.rbegin();
    map.erase( -- mapRBegin.base() );

    checkEqual( av, map );
}

//
// test_iterators_begin_equals_end_in_empty_container
//
void test_iterators_begin_equals_end_in_empty_container()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    AV_ASSERT( av.begin() == av.end() );
}

//
// test_reverse_iterators_begin_equal_end_in_empty_storage
//
void test_reverse_iterators_begin_equal_end_in_empty_storage()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    AV_ASSERT( av.rbegin() == av.rend() );
}

//
// test_iterators_iterate_not_empty_storage_empty_cache
//
void test_iterators_iterate_not_empty_storage_empty_cache()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;
    av.reserve( 32 );

    for( std::size_t i = 0 ; i < 1024 ; ++ i ){
        av.insert( AssocVector::value_type(  i, 0 ) );
    }

    AV_ASSERT_EQUAL( av.size(), 1024 );
    AV_ASSERT_EQUAL( av.bufferSize(), 0 );
    AV_ASSERT_EQUAL( av.storageSize(), 1024 );

    Iterator current = av.begin();
    Iterator const end = av.end();

    for( int i = 0 ; current != end ; ++ current, ++i ){
        AV_ASSERT_EQUAL( current->first, i );
    }
}

//
// test_iterators_increment_decrement_1
//
void test_iterators_increment_decrement_1()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;
    av.reserve( 128 );

    for( std::size_t i = 0 ; i < av.size() ; ++ i ){
        av.insert( AssocVector::value_type( i, i ) );
    }

    Iterator current = av.begin();
    Iterator const begin = av.begin();
    Iterator const end = av.end();

    for( /*empty*/ ; current != end ; ++ current );
    for( /*empty*/ ; current != begin ; -- current );

    AV_ASSERT( current == begin );
}

//
// test_iterators_increment_decrement_2
//
void test_iterators_increment_decrement_2()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    for( std::size_t i = 16 ; i != 0 ; -- i ){
        av.insert( AssocVector::value_type( i - 1, i - 1 ) );
    }

    {
        Iterator current = av.end();
        Iterator const begin = av.begin();

        for( int counter = 16 ; current != begin ; /*empty*/ )
        {
            -- current;
            -- counter;

            AV_ASSERT( current->first == counter );
            AV_ASSERT( current->second == counter );
        }

        AV_ASSERT( current == begin );
    }

    {
        Iterator current = av.begin();
        Iterator const end = av.end();

        for( int counter = 0 ; current != end ; /*empty*/ )
        {
            AV_ASSERT( current->first == counter );
            AV_ASSERT( current->second == counter );

            ++ current;
            ++ counter;
        }

        AV_ASSERT( current == end );
    }
}

//
// black_box_test
//
template< typename _Key, typename _Value >
void black_box_test( int rep )
{
    typedef AssocVector< _Key, _Value > AV;
    AV av;

    typedef std::map< _Key, _Value > MAP;
    MAP map;

    TestCase< _Key, _Value > test( av, map );

    checkEqual( av, map );

    int percentage = 1;
    int const progressStep = 20;
    int const numberOfRepetitions = rep * 256;

    {//draw simple progress bar
        std::cout << "[0%.."; std::flush( std::cout );
    }

    for( int i = 0 ; i < numberOfRepetitions ; ++ i )
    {
        {//draw simple progress bar
            if( 100.0 * i / numberOfRepetitions >= progressStep * percentage ){
                std::cout << progressStep * (percentage++) << "%.."; std::flush( std::cout );
            }
        }

        unsigned const maxKeyValue = 128;

        int operation = rand() % 10;

        switch( operation )
        {
            case 0:
            case 1:
            case 2:
                {
                    int const key = rand() % maxKeyValue;

                    test >> insert( key, key );
                }

                break;

            case 3:
                {
                    int const key = rand() % maxKeyValue;

                    test >> find( key );
                }

                break;

            case 4:
                {
                    int const key = rand() % maxKeyValue;

                    test >> erase( key );
                }

                break;

            case 5:
                {
                    int const key = rand() % maxKeyValue;

                    test >> findErase( key );
                }

                break;

            case 6:
                {
                    int const key = rand() % maxKeyValue;

                    test >> index_put( key, key );
                }

                break;

            case 7:
                {
                    int const key = rand() % maxKeyValue;

                    test >> lower_bound( key );
                }

                break;

            case 8:
                {
                    int const key = rand() % maxKeyValue;

                    test >> upper_bound( key );
                }

                break;

            case 9:
                {
                    int const key = rand() % maxKeyValue;

                    test >> equal_range( key );
                }

                break;

            default:
                AV_ASSERT( false );
        }
    }

    {//draw simple progress bar
        std::cout << "100%] "; std::flush( std::cout );
    }
}

//
// mem_leak_test_1
//
void mem_leak_test_1()
{
    typedef MyAllocator< std::pair< Key, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Key::createdObjects = 0;
    Key::destroyedObjects = 0;

    Key::moves = 0;
    Key::copies = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    Value::moves = 0;
    Value::copies = 0;

    {
        unsigned const maxKeyValue = 512;

        typedef AssocVector< Key, Value, std::less< Key >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < 128 * 1024 ; ++ i )
        {
            int const operation = rand() % 5;

            switch( operation )
            {
                case 0:
                    av.insert( typename AV::value_type( rand() % maxKeyValue, Value() ) );
                    break;

                case 1:
                    av.find( rand() % maxKeyValue );
                    break;

                case 2:
                    av.erase( rand() % maxKeyValue );
                    break;

                case 3:
                    {
                        typename AV::iterator foundAV = av.find( rand() % maxKeyValue );

                        if( foundAV != av.end() ){
                            av.erase( foundAV );
                        }
                    }

                    break;

                case 4:
                    av[ rand() % maxKeyValue ] = Value();
                    break;
            }
        }
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );

    AV_ASSERT_EQUAL( Key::createdObjects, Key::destroyedObjects );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// mem_leak_test_destructor
//
void mem_leak_test_destructor()
{
    typedef MyAllocator< std::pair< int, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    {
        int const numberOfObjects = 1024;

        typedef AssocVector< int, Value, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < numberOfObjects ; ++ i ){
            av.insert( AV::value_type( i, Value() ) );
        }
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// mem_leak_test_clear
//
void mem_leak_test_clear()
{
    typedef MyAllocator< std::pair< int, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    int const numberOfObjects = 1024;

    typedef AssocVector< int, Value, std::less< int >, Allocator > AV;
    AV av;

    for( int i = 0 ; i < numberOfObjects ; ++ i ){
        av.insert( AV::value_type( i, Value() ) );
    }

    av.clear();

    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// mem_leak_test_copy_constructor
//
void mem_leak_test_copy_constructor()
{
    typedef MyAllocator< std::pair< int, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    {
        int const numberOfObjects = 1024;

        typedef AssocVector< int, Value, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < numberOfObjects ; ++ i ){
            av.insert( AV::value_type( i, Value() ) );
        }

        AV av2( av );
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// mem_leak_test_assign_operator
//
void mem_leak_test_assign_operator()
{
    typedef MyAllocator< std::pair< int, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    {
        int const numberOfObjects = 1024;

        typedef AssocVector< int, Value, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < numberOfObjects ; ++ i ){
            av.insert( AV::value_type( i, Value() ) );
        }

        AV av2;
        av2 = av;
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// cxx11x_move_test_1
//
void cxx11x_move_test_1()
{
    typedef MyAllocator< std::pair< Key, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Key::createdObjects = 0;
    Key::destroyedObjects = 0;

    Key::moves = 0;
    Key::copies = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    Value::moves = 0;
    Value::copies = 0;

    {
        unsigned const counter = 1024;

        typedef AssocVector< Key, Value, std::less< Key >, Allocator > AV;
        AV av1;

        {// insert( value_type )
            for( unsigned i = 0 ; i < counter ; ++ i ){
                av1.insert( AV::value_type( i, Value() ) );
            }
        }

        AV_ASSERT_EQUAL( Value::copies, 0 );

        AV av2 = std::move( av1 );

        AV_ASSERT_EQUAL( Value::copies, 0 );
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );

    AV_ASSERT_EQUAL( Key::createdObjects, Key::destroyedObjects );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// cxx11x_move_test_2
//
void cxx11x_move_test_2()
{
    typedef MyAllocator< std::pair< Key, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Key::createdObjects = 0;
    Key::destroyedObjects = 0;

    Key::moves = 0;
    Key::copies = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    Value::moves = 0;
    Value::copies = 0;

    {
        unsigned const counter = 1024;

        typedef AssocVector< Key, Value, std::less< Key >, Allocator > AV;
        AV av;

        {// insert( value_type )
            for( unsigned i = 0 ; i < counter / 4 ; ++ i ){
                Value value;

                av.insert( AV::value_type( i, value ) );
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter / 4 );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// insert( value_type )
            for( unsigned i = counter / 4 ; i < counter / 2 ; ++ i ){
                av.insert( AV::value_type( Key( i ), Value( i ) ) );
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter / 2 );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// operator[]( value_type )
            for( unsigned i = counter / 4 ; i < counter / 2 ; ++ i ){
                av[ i ] = Value();
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter / 2 );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// find( value_type )
            for( unsigned i = 0 ; i < counter / 4 ; ++ i ){
                av.find( i );
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter / 2 );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// _find( value_type )
            for( unsigned i = counter / 4 ; i < counter / 2 ; ++ i ){
                av._find( i );
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter / 2 );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// operator[]( value_type )
            for( unsigned i = counter / 2 ; i < counter ; ++ i ){
                av[ i ] == i;
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// erase( value_type )
            for( unsigned i = 0 ; i < counter / 2 ; ++ i ){
                av.erase( i );
            }
        }

        AV_ASSERT_EQUAL( Key::copies, counter );
        AV_ASSERT_EQUAL( Value::copies, counter / 4 );

        {// erase( iterator )
            for( unsigned i = counter / 2 ; i < counter ; ++ i ){
                av.erase( av.find( i ) );
            }
        }

        AV_ASSERT( Key::copies <= 2 * counter );
        AV_ASSERT( Value::copies <= 2 * counter );
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );

    AV_ASSERT_EQUAL( Key::createdObjects, Key::destroyedObjects );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// cxx11x_forward_test_insert
//
void cxx11x_forward_test_insert()
{
    typedef MyAllocator< std::pair< Key, Value > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    Key::createdObjects = 0;
    Key::destroyedObjects = 0;

    Key::moves = 0;
    Key::copies = 0;

    Value::createdObjects = 0;
    Value::destroyedObjects = 0;

    Value::moves = 0;
    Value::copies = 0;

    {
        unsigned const counter = 1024;

        typedef AssocVector< Key, Value, std::less< Key >, Allocator > AV;
        AV av;

        {// insert( value_type )
            for( unsigned i = 0 ; i < counter / 2 ; ++ i ){
                av.insert( std::move( AV::value_type( i, std::move( Value() ) ) ) );
            }
        }

        AV_ASSERT_EQUAL( Value::copies, 0 );
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );

    AV_ASSERT_EQUAL( Key::createdObjects, Key::destroyedObjects );
    AV_ASSERT_EQUAL( Value::createdObjects, Value::destroyedObjects );
}

//
// cxx11x_at_test
//
void cxx11x_at_test()
{
    typedef AssocVector< std::string, int > AssocVector;

    AssocVector av;

    av[ "a" ] = 1;
    av[ "b" ] = 2;
    av[ "c" ] = 3;

    av.erase( "b" );

    AV_ASSERT( av.at( "a" ) == 1 );
    AV_ASSERT( av.at( "c" ) == 3 );

    try{
        av.at( "b" ) == 2;
        AV_ASSERT( false );
    }
    catch( std::out_of_range & ){
    }
    catch( ... ){
        AV_ASSERT( false );
    }

    try{
        av.at( "d" ) == 4;
        AV_ASSERT( false );
    }
    catch( std::out_of_range & ){
    }
    catch( ... ){
        AV_ASSERT( false );
    }
}

//
// cxx11x_emplace_test
//
void cxx11x_emplace_test()
{
    typedef AssocVector< Key, Value > AssocVector;

    {
        AssocVector av;

        Key::copies = 0;
        Value::copies = 0;

        av.emplace( 1 );

        AV_ASSERT_EQUAL( Key::copies, 0 );
        AV_ASSERT_EQUAL( Value::copies, 0 );

        AV_ASSERT( av.at( 1 ) == Value( 0, "" ) );
    }

    {
        AssocVector av;

        Key::copies = 0;
        Value::copies = 0;

        av.emplace( 11, 22 );

        AV_ASSERT_EQUAL( Key::copies, 0 );
        AV_ASSERT_EQUAL( Value::copies, 0 );

        AV_ASSERT( av.at( 11 ) == Value( 22, "" ) );
    }

    {
        AssocVector av;

        Key::copies = 0;
        Value::copies = 0;

        av.emplace( 111, 222, "333" );

        AV_ASSERT_EQUAL( Key::copies, 0 );
        AV_ASSERT_EQUAL( Value::copies, 0 );

        AV_ASSERT( av.at( 111 ) == Value( 222, "333" ) );
    }
}

int main( int argc, char * argv[] )
{
    {
        std::cout << "Util tests..."; std::flush( std::cout );

        test_CmpByFirst_1();
        test_CmpByFirst_2();
        test_CmpByFirst_3();

        test_isBetween();

        test_move_empty_array();
        test_move_self_copy();
        test_move_between_different_containers();
        test_move_overlap_less_then_half();
        test_move_overlap_more_than_half();
        test_move_overlap_copy_to_begining();

        test_last_less_equal();

        test_merge_1();
        test_merge_2();
        test_merge_3();
        test_merge_4();
        test_merge_5();
        test_merge_6();
        test_merge_7();

        std::cout << "OK." << std::endl;
    }

    {
        std::cout << "Core tests..."; std::flush( std::cout );

        test_constructor();
        test_copy_constructor();
        test_constructor_init_list();
        test_assign_operator();
        test_clear();

        test_empty_container();
        test_empty_container_push_back();

        test_insert_in_increasing_order();
        test_insert_in_decreasing_order();
        test_insert_in_random_order();
        test_insert_init_list();

        test_erase_in_increasing_order();
        test_erase_in_decreasing_order();
        test_erase_in_random_order();

        test_insert_insert();
        test_insert_erase_erase();
        test_insert_erase_insert();

        test_find();
        test_find_erase_find();
        test_find_erase_find_insert_find();

        test_count();
        test_lower_bound();
        test_upper_bound();
        test_equal_range();

        test_swap();

        test_operator_index();

        test_user_type();

        test_types();

        std::cout << "OK." << std::endl;
    }

    {
        std::cout << "Iterator tests..."; std::flush( std::cout );

        test_iterator_to_const_iterator_conversion();

        test_iterators_distance();
        test_reverse_iterators_distance();
        test_reverse_iterators_base();

        test_iterators_begin_equals_end_in_empty_container();
        test_reverse_iterators_begin_equal_end_in_empty_storage();

        test_iterators_iterate_not_empty_storage_empty_cache();

        test_iterators_increment_decrement_1();
        test_iterators_increment_decrement_2();

        std::cout << "OK." << std::endl;
    }

    {
        std::cout << "C++11x tests..."; std::flush( std::cout );

        cxx11x_move_test_1();
        cxx11x_move_test_2();

        cxx11x_forward_test_insert();

        cxx11x_at_test();
        cxx11x_emplace_test();

        std::cout << "OK." << std::endl;
    }

    {
        std::cout << "Memory tests..."; std::flush( std::cout );

        mem_leak_test_1();
        mem_leak_test_destructor();
        mem_leak_test_clear();
        mem_leak_test_copy_constructor();
        mem_leak_test_assign_operator();

        std::cout << "OK." << std::endl;
    }

    {
        std::cout << "BlackBox tests..."; std::flush( std::cout );

        int rep = argc == 2 ? atoi( argv[1] ) : 16;

        black_box_test< Key, Value >( rep );

        std::cout << "OK." << std::endl;
    }

#if defined _MSC_VER
    system( "pause" );
#endif

    return 0;
}
