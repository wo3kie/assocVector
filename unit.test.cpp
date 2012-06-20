#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#define AV_ENABLE_EXTENSIONS
#include "AssocVector.hpp"

#ifdef AV_DEBUG
    #define AV_ASSERT( expression )\
        assert( expression )

    #define AV_ASSERT_EQUAL( actual, expected )\
        if( ( actual ) != ( expected ) ){ std::cout << "(" << actual << ") == " << expected << std::endl; assert( false );}

    #define AV_ASSERT_NOT_EQUAL( actual, expected )\
        if( ( actual ) == ( expected ) ){ std::cout << "(" << actual << ") != " << expected << std::endl; assert( false );}
#else
    #error Are you trying to run unit tests with assertions disabled ?

    #define AV_ASSERT( expression ) (void)( expression );
    #define AV_ASSERT_EQUAL( actual, expected ) (void)( actual ); (void)( expected );
    #define AV_ASSERT_NOT_EQUAL( actual, expected ) (void)( actual ); (void)( expected );
#endif

template< typename T, typename K >
typename std::map< T, K >::iterator
erase( std::map< T, K > & map, typename std::map< T, K >::iterator pos )
{
    if( pos != map.end() )
    {
#ifdef AV_MAP_ERASE_RETURNS_ITERATOR
        return map.erase( pos );
#else
        map.erase( pos );
#endif
    }

    return map.end();
}

template< typename T, typename K >
typename AssocVector< T, K >::iterator
erase( AssocVector< T, K > & av, typename AssocVector< T, K >::iterator pos )
{
    if( pos != av.end() ){
#ifdef AV_MAP_ERASE_RETURNS_ITERATOR
        return av.erase( pos );
#else
        av.erase( pos );
#endif
    }

    return av.end();
}

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

//
// isEqual
//
template<
      typename _T1
    , typename _T2
>
bool isEqual(
      AssocVector< _T1, _T2 > const & av
    , std::map< _T1, _T2 > const & map
)
{
    if( av.size() != map.size() )
    {
        return false;
    }

    bool const result1 = std::equal( av.begin(), av.end(), map.begin() );
    bool const result2 = std::equal( av.rbegin(), av.rend(), map.rbegin() );

    AV_ASSERT_EQUAL( result1, result2 );

    if( av.empty() ){
        return result1;
    }

    {
        typename AssocVector< _T1, _T2 >::const_iterator currentAV = av.end();
        typename AssocVector< _T1, _T2 >::const_iterator const beginAV = av.begin();

        typename std::map< _T1, _T2 >::const_iterator currentMAP = map.end();
        typename std::map< _T1, _T2 >::const_iterator const beginMAP = map.begin();

        do
        {
            -- currentAV;
            -- currentMAP;

            AV_ASSERT_EQUAL( * currentAV, * currentMAP );
        }
        while( currentMAP != beginMAP );

        AV_ASSERT_EQUAL( currentAV, beginAV );

        AV_ASSERT_EQUAL( * currentAV, * currentMAP );
    }

    {
        typename AssocVector< _T1, _T2 >::const_reverse_iterator currentAV = av.rend();
        typename AssocVector< _T1, _T2 >::const_reverse_iterator const rbeginAV = av.rbegin();

        typename std::map< _T1, _T2 >::const_reverse_iterator currentMAP = map.rend();
        typename std::map< _T1, _T2 >::const_reverse_iterator const rbeginMAP = map.rbegin();

        do
        {
            -- currentAV;
            -- currentMAP;

            AV_ASSERT_EQUAL( * currentAV, * currentMAP );
        }
        while( currentMAP != rbeginMAP );

        AV_ASSERT_EQUAL( currentAV, rbeginAV );
    }

    return true;
}

//
// S1
//
struct S1
{
    S1 & operator=( S1 const & other )
    {
        i = other.i;

        return * this;
    }

    bool operator==( S1 const & other )const
    {
        return i == other.i;
    }

    bool operator<( S1 const & other )const
    {
        return i < other.i;
    }

    int i;
};

std::ostream & operator<<( std::ostream & out, S1 const & s1 )
{
    return out << s1.i;
}

//
// S2
//
struct S2
{
    S2()
    {
        b = 0;
        s = 0;
        i = 0;
        u = 0;
        c = 0;
        f = 0;
        d = 0;
        v = 0;
    }

    S2 & operator=( S2 const & other )
    {
        b = other.b;
        s = other.s;
        i = other.i;
        u = other.u;
        c = other.c;
        f = other.f;
        d = other.d;
        v = other.v;

        return * this;
    }

    bool operator==( S2 const & other )const
    {
        return
               b == other.b
            && s == other.s
            && i == other.i
            && u == other.u
            && c == other.c
            && f == other.f
            && d == other.d
            && v == other.v;
    }

    bool b;
    short s;
    int i;
    unsigned u;
    char c;
    float f;
    double d;
    void * v;
};

std::ostream & operator<<( std::ostream & out, S2 const & s2 )
{
    return out << s2.b << ", " << s2.s << ", " << s2.i << ", " << s2.u << ", "
        << s2.c << ", " << s2.f << ", " << s2.d << ", " << s2.v;
}

//
// S3
//
struct S3
{
    S3()
    {
        ++ createdObjects;

        for( int i = 0 ; i < 10 ; ++ i ){
            array.push_back( rand() );
        }
    }

    S3( S3 const & other )
        : array( other.array )
    {
        ++ createdObjects;
        ++ copies;
    }

    ~S3()
    {
        ++ destroyedObjects;
    }

#ifdef AV_CXX11X_RVALUE_REFERENCE
    S3( S3 && other )
        : array( std::move( other.array ) )
    {
        ++ createdObjects;
        ++ moves;
    }
#endif

    S3 & operator=( S3 const & other )
    {
        ++ copies;

        S3 temp( other );
        this->swap( temp );

        return * this;
    }

#ifdef AV_CXX11X_RVALUE_REFERENCE
    S3 & operator=( S3 && other )
    {
        ++ moves;

        array = std::move( other.array );

        return * this;
    }
#endif

    bool operator==( S3 const & other )const
    {
        return array == other.array;
    }

    bool operator<( S3 const & other )const
    {
        return array < other.array;
    }

    void swap( S3 & other )
    {
        array.swap( other.array );
    }

    std::vector< int > array;

    static unsigned createdObjects;
    static unsigned destroyedObjects;

    static unsigned copies;
    static unsigned moves;
};

unsigned S3::createdObjects = 0;

unsigned S3::destroyedObjects = 0;

unsigned S3::copies = 0;

unsigned S3::moves = 0;

std::ostream & operator<<( std::ostream & out, S3 const & s3 )
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
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::move( array.begin(), array.end(), array.begin() );

    std::vector< int > expected;
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );
    expected.push_back( 4 );
    expected.push_back( 5 );
    expected.push_back( 6 );

    AV_ASSERT( array == expected );
}

//
// test_move_between_different_containers
//
void test_move_between_different_containers()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    std::vector< int > array2( 6, 0 );

    util::move( array.begin(), array.end(), array2.begin() );

    AV_ASSERT( array == array2 );
}

//
// test_move_overlap_less_then_half
//
void test_move_overlap_less_then_half()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::move( array.begin(), array.begin() + 3, array.begin() + 3 );

    std::vector< int > expected;
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );

    AV_ASSERT( array == expected );
}

//
// test_move_overlap_more_than_half
//
void test_move_overlap_more_than_half()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::move( array.begin(), array.begin() + 4, array.begin() + 2 );

    std::vector< int > expected;
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );
    expected.push_back( 4 );

    AV_ASSERT( array == expected );
}

//
// test_move_overlap_copy_to_begining
//
void test_move_overlap_copy_to_begining()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::move( array.begin() + 2, array.end(), array.begin() );

    std::vector< int > expected;
    expected.push_back( 3 );
    expected.push_back( 4 );
    expected.push_back( 5 );
    expected.push_back( 6 );
    expected.push_back( 5 );
    expected.push_back( 6 );

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
        std::vector< int > v;
        v.push_back( 1 );
        v.push_back( 3 );
        v.push_back( 5 );
        v.push_back( 7 );
        v.push_back( 9 );

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
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 0 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 4 );

        a2[0] = 1;
        a2[1] = 2;
        a2[2] = 3;
        a2[3] = 4;

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 4 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 4 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 4 );

        a1[0] = 1;
        a1[1] = 2;
        a1[2] = 3;
        a1[3] = 4;

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 0 );

        array::move_merge( a1, a2, std::less< int >() );

        AV_ASSERT_EQUAL( a1.size(), 4 );
        AV_ASSERT_EQUAL( a1.capacity(), 10 );

        AV_ASSERT_EQUAL( a2.size(), 0 );
        AV_ASSERT_EQUAL( a2.capacity(), 10 );

        AV_ASSERT_EQUAL( a1[0], 1 );
        AV_ASSERT_EQUAL( a1[1], 2 );
        AV_ASSERT_EQUAL( a1[2], 3 );
        AV_ASSERT_EQUAL( a1[3], 4 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 6 );

        a1[0] = 1;
        a1[1] = 2;
        a1[2] = 3;
        a1[3] = 4;
        a1[4] = 5;
        a1[5] = 6;

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 4 );

        a2[0] = 7;
        a2[1] = 8;
        a2[2] = 9;
        a2[3] = 10;

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

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 6 );

        a1[0] = 1;
        a1[1] = 3;
        a1[2] = 5;
        a1[3] = 7;
        a1[4] = 9;
        a1[5] = 11;

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 4 );

        a2[0] = 2;
        a2[1] = 4;
        a2[2] = 6;
        a2[3] = 8;

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

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 6 );

        a1[0] = 5;
        a1[1] = 6;
        a1[2] = 7;
        a1[3] = 8;
        a1[4] = 9;
        a1[5] = 10;

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 4 );

        a2[0] = 1;
        a2[1] = 2;
        a2[2] = 3;
        a2[3] = 4;

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

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 6 );

        a1[0] = 1;
        a1[1] = 2;
        a1[2] = 3;
        a1[3] = 8;
        a1[4] = 9;
        a1[5] = 10;

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 4 );

        a2[0] = 4;
        a2[1] = 5;
        a2[2] = 6;
        a2[3] = 7;

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

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );
        a1.setSize( 6 );

        a1[0] = 2;
        a1[1] = 3;
        a1[2] = 5;
        a1[3] = 6;
        a1[4] = 8;
        a1[5] = 9;

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 4 );

        a2[0] = 1;
        a2[1] = 4;
        a2[2] = 7;
        a2[3] = 10;

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

        delete [] a1.getData();
        delete [] a2.getData();
    }
}

//
// test_merge_2
//
void test_merge_2()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 15 ; ++ i ){
        storage.setSize( storage.size() + 1 );
        storage[ storage.size() - 1 ] = std::pair< int, int >( i, 0 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 3 );

    for( int i = 3 ; i < 6 ; ++ i ){
        buffer.setSize( buffer.size() + 1 );
        buffer[ buffer.size() - 1 ] = std::pair< int, int >( i, 0 );

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

    delete [] storage.getData();
    delete [] buffer.getData();
}

//
// test_merge_3
//
void test_merge_3()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 15 ; ++ i ){
        storage.setSize( storage.size() + 1 );
        storage[ storage.size() - 1 ] = std::pair< int, int >( i, 0 );

    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 15 );

    for( int i = 23 ; i < 26 ; ++ i ){
        buffer.setSize( buffer.size() + 1 );
        buffer[ buffer.size() - 1 ] = std::pair< int, int >( i, 0 );
    }

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 8 );

    AV_ASSERT_EQUAL( buffer.size(), 3 );
    AV_ASSERT_EQUAL( buffer.capacity(), 15 );

    AV_ASSERT_EQUAL( storage[0].first, 10 );
    AV_ASSERT_EQUAL( storage[1].first, 11 );
    AV_ASSERT_EQUAL( storage[2].first, 12 );
    AV_ASSERT_EQUAL( storage[3].first, 13 );
    AV_ASSERT_EQUAL( storage[4].first, 14 );
    AV_ASSERT_EQUAL( storage[5].first, 23 );
    AV_ASSERT_EQUAL( storage[6].first, 24 );
    AV_ASSERT_EQUAL( storage[7].first, 25 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

//
// test_merge_4
//
void test_merge_4()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 20 ; i += 2 ){
        storage.setSize( storage.size() + 1 );
        storage[ storage.size() - 1 ] = std::pair< int, int >( i, 0 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 10 );

    for( int i = 13 ; i < 19 ; i += 2 ){
        buffer.setSize( buffer.size() + 1 );
        buffer[ buffer.size() - 1 ] = std::pair< int, int >( i, 0 );
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

    delete [] storage.getData();
    delete [] buffer.getData();
}

//
// test_merge_5
//
void test_merge_5()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 20 ; i += 2 ){
        storage.setSize( storage.size() + 1 );
        storage[ storage.size() - 1 ] = std::pair< int, int >( i, 0 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 3 );

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

    delete [] storage.getData();
    delete [] buffer.getData();
}

//
// test_merge_6
//
void test_merge_6()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 20 );

    for( int i = 13 ; i < 19 ; i += 2 ){
        buffer.setSize( buffer.size() + 1 );
        buffer[ buffer.size() - 1 ] = std::pair< int, int >( i, 0 );
    }

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 3 );

    AV_ASSERT_EQUAL( buffer.size(), 3 );
    AV_ASSERT_EQUAL( buffer.capacity(), 20 );

    AV_ASSERT_EQUAL( storage[0].first, 13 );
    AV_ASSERT_EQUAL( storage[1].first, 15 );
    AV_ASSERT_EQUAL( storage[2].first, 17 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

//
// test_merge_7
//
void test_merge_7()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 3 );

    array::move_merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT_EQUAL( storage.capacity(), 10 );
    AV_ASSERT_EQUAL( storage.size(), 0 );

    AV_ASSERT_EQUAL( buffer.size(), 0 );
    AV_ASSERT_EQUAL( buffer.capacity(), 3 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

//
// test_push_back
//
void test_push_back()
{
    typedef AssocVector< std::string, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    av.insert( AssocVector::value_type( "1", 1 ) );
    av.insert( AssocVector::value_type( "2", 2 ) );
    av.insert( AssocVector::value_type( "3", 3 ) );
    av.insert( AssocVector::value_type( "4", 4 ) );
    av.insert( AssocVector::value_type( "5", 5 ) );

    AV_ASSERT_EQUAL( av[ "1" ], 1 );
    AV_ASSERT_EQUAL( av[ "2" ], 2 );
    AV_ASSERT_EQUAL( av[ "3" ], 3 );
    AV_ASSERT_EQUAL( av[ "4" ], 4 );
    AV_ASSERT_EQUAL( av[ "5" ], 5 );
}

//
// test_insert_in_random_order
//
void test_insert_in_random_order()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;
    std::pair< Iterator, bool > p;

    p = av.insert( AssocVector::value_type( 10, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 10 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 9, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 9 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 8, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 8 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 16, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 16 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 17, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 17 );
    AV_ASSERT_EQUAL( p.second, true );

    {
        p = av.insert( AssocVector::value_type( 17, 0 ) );
        AV_ASSERT( p.first != av.end() );
        AV_ASSERT_EQUAL( p.first->first, 17 );
        AV_ASSERT_EQUAL( p.second, false );
    }

    p = av.insert( AssocVector::value_type( 18, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 18 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 7, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 7 );
    AV_ASSERT_EQUAL( p.second, true );

    {
        p = av.insert( AssocVector::value_type( 7, 0 ) );
        AV_ASSERT( p.first != av.end() );
        AV_ASSERT_EQUAL( p.first->first, 7 );
        AV_ASSERT_EQUAL( p.second, false );
    }

    p = av.insert( AssocVector::value_type( 6, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 6 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 5, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 5 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 4, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 4 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 11, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 11 );
    AV_ASSERT_EQUAL( p.second, true );

    {
        p = av.insert( AssocVector::value_type( 11, 0 ) );
        AV_ASSERT( p.first != av.end() );
        AV_ASSERT_EQUAL( p.first->first, 11 );
        AV_ASSERT_EQUAL( p.second, false );
    }

    p = av.insert( AssocVector::value_type( 12, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 12 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 13, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 13 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 3, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 3 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 2, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 2 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 1, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 1 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 0, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 0 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 14, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 14 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 15, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 15 );
    AV_ASSERT_EQUAL( p.second, true );

    p = av.insert( AssocVector::value_type( 19, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT_EQUAL( p.first->first, 19 );
    AV_ASSERT_EQUAL( p.second, true );

    AV_ASSERT_EQUAL( av.size(), 20 );

    for( unsigned i = 0 ; i < av.size() ; ++ i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT_EQUAL( av.find( i )->first, i );
    }
}

//
// test_insert_in_increasing_order
//
void test_insert_in_increasing_order()
{
    std::size_t const counter = 10;

    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;
    std::pair< Iterator, bool > p;

    for( std::size_t i = 0 ; i < counter ; ++ i ){
        p = av.insert( AssocVector::value_type( i, 0 ) );

        AV_ASSERT( p.first != av.end() );
        AV_ASSERT_EQUAL( p.first->first, i );
        AV_ASSERT_EQUAL( p.second, true );
    }

    for( std::size_t i = 0 ; i < counter ; ++ i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT_EQUAL( av.find( i )->first, i );
    }
}

//
// test_insert_in_decreasing_order
//
void test_insert_in_decreasing_order()
{
    std::size_t const counter = 10;

    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;
    std::pair< Iterator, bool > p;

    for( std::size_t i = counter ; i > 0 ; -- i ){
        p = av.insert( AssocVector::value_type( i, 0 ) );

        AV_ASSERT( p.first != av.end() );
        AV_ASSERT_EQUAL( p.first->first, i );
        AV_ASSERT_EQUAL( p.second, true );
    }

    for( std::size_t i = counter ; i > 0 ; -- i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT_EQUAL( av.find( i )->first, i );
    }
}

//
// test_insert_check_iterator_1
//
void test_insert_check_iterator_1()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator AVIterator;

    typedef std::map< int, int > Map;
    typedef Map::iterator MapIterator;

    AssocVector av;
    Map map;

    for( int i = 1 ; i < 32 ; i += 2 )
    {
        av.insert( AssocVector::value_type( i, i ) );
        map.insert( Map::value_type( i, i ) );
    }

    AV_ASSERT( isEqual( av, map ) );

    {// insert at the beginnig
        AVIterator const avInserted = av.insert( AssocVector::value_type( 0, 0 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 0, 0 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {// insert at the end
        AVIterator const avInserted = av.insert( AssocVector::value_type( 33, 33 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 33, 33 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {// insert in middle
        AVIterator const avInserted = av.insert( AssocVector::value_type( 1, 1 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 1, 1 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {//insert in middle
        AVIterator const avInserted = av.insert( AssocVector::value_type( 11, 11 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 11, 11 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {//insert in middle
        AVIterator const avInserted = av.insert( AssocVector::value_type( 21, 21 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 21, 21 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }
}

//
// test_insert_check_iterator_2
//
void test_insert_check_iterator_2()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator AVIterator;

    typedef std::map< int, int > Map;
    typedef Map::iterator MapIterator;

    AssocVector av;
    Map map;

    {// fill storage (push_back) (1,3,5,...,31)
        for( int i = 1 ; i < 32 ; i += 2 )
        {
            av.insert( AssocVector::value_type( i, i ) );
            map.insert( Map::value_type( i, i ) );
        }
    }

    {// fill buffer (0,2,4)
        for( int i = 0 ; i < 5 ; i += 2 )
        {
            av.insert( AssocVector::value_type( i, i ) );
            map.insert( Map::value_type( i, i ) );
        }
    }

    {// overflow buffer and enforce merge
        AVIterator avInserted = av.insert( AssocVector::value_type( 6, 6 ) ).first;
        MapIterator mapInserted = map.insert( Map::value_type( 6, 6 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );

        avInserted = av.insert( AssocVector::value_type( 8, 8 ) ).first;
        mapInserted = map.insert( Map::value_type( 8, 8 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );

        avInserted = av.insert( AssocVector::value_type( 10, 10 ) ).first;
        mapInserted = map.insert( Map::value_type( 10, 10 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );

        avInserted = av.insert( AssocVector::value_type( 12, 12 ) ).first;
        mapInserted = map.insert( Map::value_type( 12, 12 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {// erase item and put it back again
        av.erase( 12 );
        map.erase( 12 );

        AVIterator const avInserted = av.insert( AssocVector::value_type( 12, 12 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 12, 12 ) ).first;

        AV_ASSERT_EQUAL(
             std::distance( av.begin(), avInserted )
           , std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT_EQUAL(
             std::distance( avInserted, av.end() )
           , std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

}

//
// test_insert_erase_insert_1
//
void test_insert_erase_insert_1()
{
    typedef AssocVector< int, int > AssocVector;

    AssocVector av;

    av[ 1 ] = 1;
    av[ 2 ] = 2;
    av[ 3 ] = 3;
    av[ 4 ] = 4;
    av[ 5 ] = 5;

    AV_ASSERT_EQUAL( av.erase( 3 ), 1 );

    AV_ASSERT_EQUAL( av.insert( AssocVector::value_type( 3, 33 ) ).second, true );

    AV_ASSERT_EQUAL( av.size(), 5 );

    AV_ASSERT_EQUAL( av[ 1 ], 1 );
    AV_ASSERT_EQUAL( av[ 2 ], 2 );
    AV_ASSERT_EQUAL( av[ 3 ], 33 );
    AV_ASSERT_EQUAL( av[ 4 ], 4 );
    AV_ASSERT_EQUAL( av[ 5 ], 5 );
}

//
// test_insert_erase_insert_2
//
void test_insert_erase_insert_2()
{
    typedef AssocVector< int, int > AssocVector;

    AssocVector av;

    av[ 1 ] = 1;
    av[ 2 ] = 2;
    av[ 3 ] = 3;
    av[ 4 ] = 4;
    av[ 5 ] = 5;

    AV_ASSERT_EQUAL( av.erase( 5 ), 1 );

    AV_ASSERT_EQUAL( av.insert( AssocVector::value_type( 5, 55 ) ).second, true );

    AV_ASSERT_EQUAL( av.size(), 5 );

    AV_ASSERT_EQUAL( av[ 1 ], 1 );
    AV_ASSERT_EQUAL( av[ 2 ], 2 );
    AV_ASSERT_EQUAL( av[ 3 ], 3 );
    AV_ASSERT_EQUAL( av[ 4 ], 4 );
    AV_ASSERT_EQUAL( av[ 5 ], 55 );
}

//
// test_insert_erase_insert_3
//
void test_insert_erase_insert_3()
{
    typedef AssocVector< int, int > AssocVector;

    AssocVector av;

    av[ 1 ] = 1;
    av[ 2 ] = 2;
    av[ 3 ] = 3;
    av[ 4 ] = 4;
    av[ 5 ] = 5;

    AV_ASSERT_EQUAL( av.erase( 3 ), 1 );

    av[ 3 ] = 33;

    AV_ASSERT_EQUAL( av.size(), 5 );

    AV_ASSERT_EQUAL( av[ 1 ], 1 );
    AV_ASSERT_EQUAL( av[ 2 ], 2 );
    AV_ASSERT_EQUAL( av[ 3 ], 33 );
    AV_ASSERT_EQUAL( av[ 4 ], 4 );
    AV_ASSERT_EQUAL( av[ 5 ], 5 );
}

//
// test_insert_erase_insert_4
//
void test_insert_erase_insert_4()
{
    typedef AssocVector< int, int > AssocVector;

    AssocVector av;

    av[ 1 ] = 1;
    av[ 2 ] = 2;
    av[ 3 ] = 3;
    av[ 4 ] = 4;
    av[ 5 ] = 5;

    AV_ASSERT_EQUAL( av.erase( 5 ), 1 );

    av[ 5 ] = 55;

    AV_ASSERT_EQUAL( av.size(), 5 );

    AV_ASSERT_EQUAL( av[ 1 ], 1 );
    AV_ASSERT_EQUAL( av[ 2 ], 2 );
    AV_ASSERT_EQUAL( av[ 3 ], 3 );
    AV_ASSERT_EQUAL( av[ 4 ], 4 );
    AV_ASSERT_EQUAL( av[ 5 ], 55 );
}

//
// test_find_1
//
void test_find_1()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    AV_ASSERT( av.find( 2 ) == av.end() );

    av.insert( AssocVector::value_type( 2, 3 ) );

    AV_ASSERT( av.find( 2 ) != av.end() );
    AV_ASSERT_EQUAL( av.find( 2 )->second, 3 );
}

//
// test_find_2
//
void test_find_2()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    for( int i = 32 ; i < 32 + 64 ; ++ i ){
        av.insert( AssocVector::value_type( i, i * i ) );
    }

    for( int i = 0 ; i < 32 ; ++ i ){
        AV_ASSERT( av.find( i ) == av.end() );
    }

    for( int i = 32 ; i < 32 + 64 ; ++ i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT_EQUAL( av.find( i )->second, i * i );
    }

    for( int i = 32 + 64 ; i < 32 + 64 + 32; ++ i ){
        AV_ASSERT( av.find( i ) == av.end() );
    }
}

//
// test_find_check_iterator
//
void test_find_check_iterator()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator AVIterator;

    typedef std::map< int, int > Map;
    typedef Map::iterator MapIterator;

    AssocVector av;
    Map map;

    for( int i = 32 ; i > 0 ; -- i ){
        av.insert( AssocVector::value_type( i, i ) );
        map.insert( Map::value_type( i, i ) );
    }

    {
        AVIterator const avIterator = av.find( 1 );
        MapIterator const mapIterator = map.find( 1 );

        AV_ASSERT_EQUAL( std::distance( av.begin(), avIterator ), std::distance( map.begin(), mapIterator ) );
        AV_ASSERT_EQUAL( std::distance( avIterator, av.end() ), std::distance( mapIterator, map.end() ) );

        AV_ASSERT( std::equal( av.begin(), avIterator, map.begin() ) );
        AV_ASSERT( std::equal( avIterator, av.end(), mapIterator ) );
    }

    {
        AVIterator const avIterator = av.find( 16 );
        MapIterator const mapIterator = map.find( 16 );

        AV_ASSERT_EQUAL( std::distance( av.begin(), avIterator ), std::distance( map.begin(), mapIterator ) );
        AV_ASSERT_EQUAL( std::distance( avIterator, av.end() ), std::distance( mapIterator, map.end() ) );

        AV_ASSERT( std::equal( av.begin(), avIterator, map.begin() ) );
        AV_ASSERT( std::equal( avIterator, av.end(), mapIterator ) );
    }

    {
        AVIterator const avIterator = av.find( 32 );
        MapIterator const mapIterator = map.find( 32 );

        AV_ASSERT_EQUAL( std::distance( av.begin(), avIterator ), std::distance( map.begin(), mapIterator ) );
        AV_ASSERT_EQUAL( std::distance( avIterator, av.end() ), std::distance( mapIterator, map.end() ) );

        AV_ASSERT( std::equal( av.begin(), avIterator, map.begin() ) );
        AV_ASSERT( std::equal( avIterator, av.end(), mapIterator ) );
    }
}

//
// test_count
//
void test_count()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    av.insert( AssocVector::value_type( 1, 1 ) );
    av.insert( AssocVector::value_type( 3, 2 ) );

    AV_ASSERT_EQUAL( av.count( 1 ), 1 );
    AV_ASSERT_EQUAL( av.count( 2 ), 0 );
    AV_ASSERT_EQUAL( av.count( 3 ), 1 );
    AV_ASSERT_EQUAL( av.count( 4 ), 0 );
}

//
// test_erase_1
//
void test_erase_1()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    std::size_t const counter = 128;

    for( std::size_t i = 0 ; i < counter ; ++ i ){
        av.insert( AssocVector::value_type( i, i ) );
    }

    AV_ASSERT_EQUAL( av.size(), counter );

    for( std::size_t i = 0 ; i < counter ; i += 2 ){
        AV_ASSERT_EQUAL( av.erase( i ), 1 );
    }

    AV_ASSERT_EQUAL( av.size(), counter / 2 );

    for( std::size_t i = 0 ; i < counter ; i += 2 ){
        AV_ASSERT( av.find( i ) == av.end() );
    }

    for( std::size_t i = 1 ; i < counter ; i += 2 ){
        AV_ASSERT_EQUAL( av.erase( i ), 1 );
    }

    AV_ASSERT( av.empty() );
}

//
// test_erase_2
//
void test_erase_2()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    int const counter = 10;

    for( int i = 0 ; i < counter ; ++ i ){
        av.insert( AssocVector::value_type( i, i ) );
    }

    for( int i = 0 ; i < counter ; ++ i ){
        AV_ASSERT( av.find( i ) != av.end() );
        av.erase( av.find( i ) );
        AV_ASSERT_EQUAL( av.size(), counter - i - 1 );
    }

    AV_ASSERT( av.empty() );
}

//
// test_erase_3
//
void test_erase_3()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT_EQUAL( av.erase( 1 ), 1 );
    AV_ASSERT_EQUAL( av.erase( 1 ), 0 );

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT_EQUAL( av.erase( 1 ), 1 );
    AV_ASSERT_EQUAL( av.erase( 1 ), 0 );
 }

//
// test_erase_from_back_already_erased
//
void test_erase_from_back_already_erased()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    av.insert( AssocVector::value_type( 4, 4 ) );
    av.insert( AssocVector::value_type( 5, 5 ) );
    av.insert( AssocVector::value_type( 6, 6 ) );

    av.insert( AssocVector::value_type( 2, 2 ) );
    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT_EQUAL( av.erase( 5 ), 1 );
    AV_ASSERT_EQUAL( av.erase( 6 ), 1 );
    AV_ASSERT_EQUAL( av.erase( 5 ), 0 );
 }

//
// test_erase_iterator_1
//
void test_erase_iterator_1()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    av[ 2 ] = 2;
    av[ 4 ] = 4;
    av[ 1 ] = 1;
    av[ 6 ] = 6;
    av[ 3 ] = 3;
    av[ 8 ] = 8;

    AssocVector::iterator pos;

    pos = av.erase( av.begin() );
    AV_ASSERT( pos == av.begin() );

    pos = av.erase( av.begin() );
    AV_ASSERT( pos == av.begin() );

    pos = av.erase( av.begin() );
    AV_ASSERT( pos == av.begin() );

    pos = av.erase( av.begin() );
    AV_ASSERT( pos == av.begin() );

    pos = av.erase( av.begin() );
    AV_ASSERT( pos == av.begin() );

    pos = av.erase( av.begin() );
    AV_ASSERT( pos == av.begin() );

    AV_ASSERT( av.begin() == av.end() );
    AV_ASSERT( av.empty() );
}

//
// test_erase_iterator_2
//
void test_erase_iterator_2()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    av[ 2 ] = 2;
    av[ 4 ] = 4;
    av[ 1 ] = 1;
    av[ 6 ] = 6;
    av[ 3 ] = 3;
    av[ 8 ] = 8;

    AssocVector::iterator pos;

    pos = av.end();
    -- pos;
    pos = av.erase( pos );
    AV_ASSERT( pos == av.end() );

    pos = av.end();
    -- pos;
    pos = av.erase( pos );
    AV_ASSERT( pos == av.end() );

    pos = av.end();
    -- pos;
    pos = av.erase( pos );
    AV_ASSERT( pos == av.end() );

    pos = av.end();
    -- pos;
    pos = av.erase( pos );
    AV_ASSERT( pos == av.end() );

    pos = av.end();
    -- pos;
    pos = av.erase( pos );
    AV_ASSERT( pos == av.end() );

    pos = av.end();
    -- pos;
    pos = av.erase( pos );
    AV_ASSERT( pos == av.end() );

    AV_ASSERT( av.begin() == av.end() );
    AV_ASSERT( av.empty() );
}

//
// test_erase_iterator_3
//
void test_erase_iterator_3()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    //s: 1357
    //b: 46
    av[ 1 ] = 1;
    av[ 3 ] = 3;
    av[ 5 ] = 5;
    av[ 7 ] = 7;
    av[ 4 ] = 4;
    av[ 6 ] = 6;

    AssocVector::iterator pos;

    pos = av.erase( av.find( 7 ) );
}

//
// test_operator_index
//
void test_operator_index_1()
{
    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    AV_ASSERT( av.empty() );

    av[ 1 ] = 1;
    AV_ASSERT_EQUAL( av.size(), 1 );
    AV_ASSERT( av.find( 1 ) != av.end() );
    AV_ASSERT_EQUAL( av.find( 1 )->second, 1 );

    av[ 1 ] = 11;
    AV_ASSERT_EQUAL( av[ 1 ], 11 );

    av[ 3 ] = 33;
    AV_ASSERT_EQUAL( av[ 3 ], 33 );

    av[ 2 ] = 22;
    AV_ASSERT_EQUAL( av[ 2 ], 22 );
}

//
// test_operator_index_2
//
void test_operator_index_2()
{
    int const counter = 20000;

    std::vector< int > randomValues( counter, 0 );
    std::generate( randomValues.begin(), randomValues.end(), & rand );

    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    for( int i = 0 ; i < counter ; ++ i ){
        av[ randomValues[ i ] ] = i;
        AV_ASSERT_EQUAL( av[ randomValues[ i ] ], i );
    }
}

//
// test_user_type
//
void test_user_type()
{
    typedef AssocVector< S1, S3 > AssocVector;

    AssocVector av;
    av.insert( AssocVector::value_type( S1(), S3() ) );
    S3 & m = av[ S1() ];
    av[ S1() ] = m;
    av.find( S1() );
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
// test_iterators_equal
//
void test_iterators_equal()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector assocVector1024;
    assocVector1024.reserve( 1024 );

    AssocVector assocVector8;
    assocVector8.reserve( 8 );

    std::vector< int > randomValues( 1024, 0 );
    std::generate( randomValues.begin(), randomValues.end(), & rand );

    for( std::size_t i = 0 ; i < randomValues.size() ; ++ i )
    {
        assocVector8.insert( AssocVector::value_type( randomValues[i], i ) );
        assocVector1024.insert( AssocVector::value_type( randomValues[i], i ) );
    }

    AV_ASSERT(
        (
            std::equal(
                  assocVector8.begin()
                , assocVector8.end()
                , assocVector1024.begin()
                , std::equal_to< std::pair< int, int > >()
            )
        )
    );
}

//
// test_reverse_iterators_equal
//
void test_reverse_iterators_equal()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector assocVector1024;
    assocVector1024.reserve( 1024 );

    AssocVector assocVector8;
    assocVector8.reserve( 8 );

    std::vector< int > randomValues( 1024, 0 );
    std::generate( randomValues.begin(), randomValues.end(), & rand );

    for( std::size_t i = 0 ; i < randomValues.size() ; ++ i )
    {
        assocVector8.insert( AssocVector::value_type( randomValues[ i ], i ) );
        assocVector1024.insert( AssocVector::value_type( randomValues[ i ], i ) );
    }

    AV_ASSERT(
        (
            std::equal(
                  assocVector8.rbegin()
                , assocVector8.rend()
                , assocVector1024.rbegin()
                , std::equal_to< std::pair< int, int > >()
            )
        )
    );
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
// test_iterator_1
//
void test_iterator_1()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    for( int i = 0 ; i < 7 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }

    Iterator current = av.begin();

    AV_ASSERT_EQUAL( current->first, 0 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 1 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 2 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 3 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 4 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 5 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 6 );
    ++current;
    AV_ASSERT( current == av.end() );
}

//
// test_iterator_erased_1
//
void test_iterator_erased_1()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    for( int i = 0 ; i < 7 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }

    {
        av.erase( 0 );

        Iterator current = av.begin();

        AV_ASSERT_EQUAL( current->first, 1 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 2 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 3 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 4 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 5 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 6 );
        ++current;
        AV_ASSERT( current == av.end() );
    }

    {
        av.erase( 1 );

        Iterator current = av.begin();

        AV_ASSERT_EQUAL( current->first, 2 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 3 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 4 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 5 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 6 );
        ++current;
        AV_ASSERT( current == av.end() );
    }

    {
        av.erase( 3 );

        Iterator current = av.begin();

        AV_ASSERT_EQUAL( current->first, 2 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 4 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 5 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 6 );
        ++current;
        AV_ASSERT( current == av.end() );
    }

    {
        av.erase( 2 );

        Iterator current = av.begin();

        AV_ASSERT_EQUAL( current->first, 4 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 5 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 6 );
        ++current;
        AV_ASSERT( current == av.end() );
    }

    {
        av.erase( 5 );

        Iterator current = av.begin();

        AV_ASSERT_EQUAL( current->first, 4 );
        ++current;
        AV_ASSERT_EQUAL( current->first, 6 );
        ++current;
        AV_ASSERT( current == av.end() );
    }

    {
        av.erase( 6 );

        Iterator current = av.begin();

        AV_ASSERT_EQUAL( current->first, 4 );
        ++current;
        AV_ASSERT( current == av.end() );
    }

    {
        av.erase( 4 );

        AV_ASSERT( av.begin() == av.end() );
    }
}

//
// test_iterator_erased_2
//
void test_iterator_erased_2()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    for( int i = 0 ; i < 7 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }

    {
        av.erase( 0 );

        Iterator current = av.end();

        --current;
        AV_ASSERT_EQUAL( current->first, 6 );
        --current;
        AV_ASSERT_EQUAL( current->first, 5 );
        --current;
        AV_ASSERT_EQUAL( current->first, 4 );
        --current;
        AV_ASSERT_EQUAL( current->first, 3 );
        --current;
        AV_ASSERT_EQUAL( current->first, 2 );
        --current;
        AV_ASSERT_EQUAL( current->first, 1 );
        AV_ASSERT( current == av.begin() );
    }

    {
        av.erase( 1 );

        Iterator current = av.end();

        --current;
        AV_ASSERT_EQUAL( current->first, 6 );
        --current;
        AV_ASSERT_EQUAL( current->first, 5 );
        --current;
        AV_ASSERT_EQUAL( current->first, 4 );
        --current;
        AV_ASSERT_EQUAL( current->first, 3 );
        --current;
        AV_ASSERT_EQUAL( current->first, 2 );
        AV_ASSERT( current == av.begin() );
    }

    {
        av.erase( 6 );

        Iterator current = av.end();

        --current;
        AV_ASSERT_EQUAL( current->first, 5 );
        --current;
        AV_ASSERT_EQUAL( current->first, 4 );
        --current;
        AV_ASSERT_EQUAL( current->first, 3 );
        --current;
        AV_ASSERT_EQUAL( current->first, 2 );
        AV_ASSERT( current == av.begin() );
    }

    {
        av.erase( 5 );

        Iterator current = av.end();

        --current;
        AV_ASSERT_EQUAL( current->first, 4 );
        --current;
        AV_ASSERT_EQUAL( current->first, 3 );
        --current;
        AV_ASSERT_EQUAL( current->first, 2 );
        AV_ASSERT( current == av.begin() );
    }

    {
        av.erase( 2 );

        Iterator current = av.end();

        --current;
        AV_ASSERT_EQUAL( current->first, 4 );
        --current;
        AV_ASSERT_EQUAL( current->first, 3 );
        AV_ASSERT( current == av.begin() );
    }

    {
        av.erase( 4 );

        Iterator current = av.end();

        --current;
        AV_ASSERT_EQUAL( current->first, 3 );
        AV_ASSERT( current == av.begin() );
    }

    {
        av.erase( 3 );
        AV_ASSERT( av.begin() == av.end() );
    }
}

//
// test_iterator_begin
//
void test_iterator_begin()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    for( int i = 2; i < 5 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }

    for( int i = 0 ; i < 2 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }


    AV_ASSERT_EQUAL( av.begin()->first, 0 );

    av.erase( 0 );
    AV_ASSERT_EQUAL( av.begin()->first, 1 );

    av.erase( 1 );
    AV_ASSERT_EQUAL( av.begin()->first, 2 );

    av.erase( 2 );
    AV_ASSERT_EQUAL( av.begin()->first, 3 );

    av.erase( 3 );
    AV_ASSERT_EQUAL( av.begin()->first, 4 );

    av.erase( 4 );
    AV_ASSERT( av.begin() == av.end() );
}

//
// test_reverse_iterator_1
//
void test_reverse_iterator_1()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::reverse_iterator RIterator;

    AssocVector av;

    for( int i = 0 ; i < 7 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }

    RIterator current = av.rbegin();

    AV_ASSERT_EQUAL( current->first, 6 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 5 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 4 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 3 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 2 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 1 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 0 );
    ++ current;
    AV_ASSERT( current == av.rend() );
}

//
// test_reverse_iterator_2
//
void test_reverse_iterator_2()
{
    typedef AssocVector< int, int > AssocVector;
    typedef AssocVector::reverse_iterator RIterator;

    AssocVector av;

    for( int i = 0 ; i < 7 ; ++ i ){
        av.insert( AssocVector::value_type( i, 0 ) );
    }

    AV_ASSERT_EQUAL( av.erase( 1 ), 1 );
    AV_ASSERT_EQUAL( av.erase( 3 ), 1 );

    RIterator current = av.rbegin();

    AV_ASSERT_EQUAL( current->first, 6 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 5 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 4 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 2 );
    ++current;
    AV_ASSERT_EQUAL( current->first, 0 );
    ++ current;
    AV_ASSERT( current == av.rend() );
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
// test_erase_iterator
//
void test_erase_iterator()
{
    typedef AssocVector< int, int > AVII;

    AVII av;

    av[ 1 ] = 11;
    av[ 2 ] = 22;
    av[ 3 ] = 33;
    av[ 4 ] = 44;

    av.merge();

    {
        AV_ASSERT_EQUAL( av.erase( 5 ), 0 );

        AVII::iterator current = av.begin();

        AV_ASSERT( ( * current ).first == 1 && ( * current ).second == 11 );

        ++ current;
        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( ( * current ).first == 4 && ( * current ).second == 44 );

        ++ current;
        AV_ASSERT( current == av.end() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 4 ), 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( ( * current ).first == 1 && ( * current ).second == 11 );

        ++ current;
        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.end() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 1 ), 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.end() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 2 ), 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.end() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 3 ), 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( current == av.end() );
    }
}

//
// test_erase_reverse_iterator
//
void test_erase_reverse_iterator()
{
    typedef AssocVector< int, int > AVII;

    AVII av;

    av[ 1 ] = 11;
    av[ 2 ] = 22;
    av[ 3 ] = 33;
    av[ 4 ] = 44;

    av.merge();

    {
        AV_ASSERT_EQUAL( av.erase( 5 ), 0 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( ( * current ).first == 4 && ( * current ).second == 44 );

        ++ current;
        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( ( * current ).first == 1 && ( * current ).second == 11 );

        ++ current;
        AV_ASSERT( current == av.rend() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 4 ), 1 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( ( * current ).first == 1 && ( * current ).second == 11 );

        ++ current;
        AV_ASSERT( current == av.rend() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 1 ), 1 );

        AVII::reverse_iterator current = av.rbegin();
        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( current == av.rend() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 2 ), 1 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.rend() );
    }
    {
        AV_ASSERT_EQUAL( av.erase( 3 ), 1 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( current == av.rend() );
    }
}

//
// black_box_test
//
template< typename _T >
void black_box_test()
{
    typedef AssocVector< int, _T > AV;
    AV av;

    typedef std::map< int, _T > MAP;
    MAP map;

    AV_ASSERT( isEqual( av, map ) );

    int percentage = 1;
    int const progressStep = 20;
    int const numberOfRepetitions = 64 * 1024;

    {//draw simple progress bar
        std::cout << "["; std::flush( std::cout );
    }

    for( int i = 0 ; i < numberOfRepetitions ; ++ i )
    {
        {//draw simple progress bar
            if( 100 * i / numberOfRepetitions >= progressStep * percentage ){
                std::cout << progressStep * (percentage++) << "%.."; std::flush( std::cout );
            }
        }

        unsigned const maxKeyValue = 128;

        int operation = rand() % 6;

        switch( operation )
        {
            case 0:
                {
                    int const key = rand() % maxKeyValue;
                    _T const value = _T();

                    std::pair< typename AV::iterator, bool> insertedAV
                        = av.insert( typename AV::value_type( key, value ) );

                    std::pair< typename MAP::iterator, bool> insertedMap =
                        map.insert( typename MAP::value_type( key, value ) );

                    AV_ASSERT( insertedAV.second == insertedMap.second );
                    AV_ASSERT( * insertedAV.first == * insertedMap.first );

                    AV_ASSERT_EQUAL(
                          std::distance( av.begin(), insertedAV.first )
                        , std::distance( map.begin(), insertedMap.first )
                    );

                    AV_ASSERT( std::equal( av.begin(), insertedAV.first, map.begin() ) );

                    AV_ASSERT_EQUAL(
                          std::distance( insertedAV.first, av.end() )
                        , std::distance( insertedMap.first, map.end() )
                    );

                    AV_ASSERT( std::equal( insertedAV.first, av.end(), insertedMap.first ) );

                    AV_ASSERT( isEqual( av, map ) );
                }

                break;

            case 1:
                {
                    int const key = rand() % maxKeyValue;

                    typename AV::iterator foundAV = av.find( key );
                    typename MAP::iterator foundMap = map.find( key );

                    AV_ASSERT(
                           ( foundAV == av.end() && foundMap == map.end() )
                        || ( * foundAV == * foundMap )
                    );

                    AV_ASSERT_EQUAL(
                          std::distance( av.begin(), foundAV )
                        , std::distance( map.begin(), foundMap )
                    );

                    AV_ASSERT( std::equal( av.begin(), foundAV, map.begin() ) );

                    AV_ASSERT_EQUAL(
                          std::distance( foundAV, av.end() )
                        , std::distance( foundMap, map.end() )
                    );

                    AV_ASSERT( std::equal( foundAV, av.end(), foundMap ) );

                    AV_ASSERT( isEqual( av, map ) );
                }

                break;

            case 2:
                {
                    int const key = rand() % maxKeyValue;

                    AV_ASSERT_EQUAL( av.erase( key ), map.erase( key ) );

                    AV_ASSERT( isEqual( av, map ) );
                }

                break;

            case 3:
                {
                    int const key = rand() % maxKeyValue;

#ifdef AV_MAP_ERASE_RETURNS_ITERATOR
                    typename AV::iterator avIterator = erase( av, av.find( key ) );
                    typename MAP::iterator mapIterator = erase( map, map.find( key ) );

                    AV_ASSERT_EQUAL(
                          std::distance( av.begin(), avIterator )
                        , std::distance( map.begin(), mapIterator )
                    );

                    AV_ASSERT( std::equal( av.begin(), avIterator, map.begin() ) );

                    AV_ASSERT_EQUAL(
                          std::distance( avIterator, av.end() )
                        , std::distance( mapIterator, map.end() )
                    );

                    AV_ASSERT( std::equal( avIterator, av.end(), mapIterator ) );
#else
                    erase( av, av.find( key ) );
                    erase( map, map.find( key ) );
#endif

                    AV_ASSERT( isEqual( av, map ) );
                }

                break;

            case 4:
                {
                    int const key = rand() % maxKeyValue;

                    typename AV::iterator foundAV = av.find( key );
                    typename MAP::iterator foundMap = map.find( key );

                    if( foundAV == av.end() && foundMap == map.end() )
                    {
                        // empty
                    }
                    else
                    {
                        av.erase( foundAV );
                        map.erase( foundMap );
                    }

                    AV_ASSERT( isEqual( av, map ) );
                }

                break;

            case 5:
                {
                    int const key = rand() % maxKeyValue;
                    _T const value = _T();

                    av[ key ] = value;
                    map[ key ] = value;

                    AV_ASSERT( isEqual( av, map ) );
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
    typedef MyAllocator< std::pair< int, S3 > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    S3::createdObjects = 0;
    S3::destroyedObjects = 0;

    S3::moves = 0;
    S3::copies = 0;

    {
        unsigned const maxKeyValue = 512;

        typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < 128 * 1024 ; ++ i )
        {
            int const operation = rand() % 5;

            switch( operation )
            {
                case 0:
                    av.insert( typename AV::value_type( rand() % maxKeyValue, S3() ) );
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
                    av[ rand() % maxKeyValue ] = S3();
                    break;
            }
        }
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
}

//
// mem_leak_test_destructor
//
void mem_leak_test_destructor()
{
    typedef MyAllocator< std::pair< int, S3 > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    S3::createdObjects = 0;
    S3::destroyedObjects = 0;

    {
        int const numberOfObjects = 1024;

        typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < numberOfObjects ; ++ i ){
            av.insert( AV::value_type( i, S3() ) );
        }
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
}

//
// mem_leak_test_clear
//
void mem_leak_test_clear()
{
    typedef MyAllocator< std::pair< int, S3 > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    S3::createdObjects = 0;
    S3::destroyedObjects = 0;

    int const numberOfObjects = 1024;

    typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
    AV av;

    for( int i = 0 ; i < numberOfObjects ; ++ i ){
        av.insert( AV::value_type( i, S3() ) );
    }

    av.clear();

    AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
}

//
// mem_leak_test_copy_constructor
//
void mem_leak_test_copy_constructor()
{
    typedef MyAllocator< std::pair< int, S3 > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    S3::createdObjects = 0;
    S3::destroyedObjects = 0;

    {
        int const numberOfObjects = 1024;

        typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < numberOfObjects ; ++ i ){
            av.insert( AV::value_type( i, S3() ) );
        }

        AV av2( av );
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
}

//
// mem_leak_test_assign_operator
//
void mem_leak_test_assign_operator()
{
    typedef MyAllocator< std::pair< int, S3 > > Allocator;

    Allocator::notFreedMemory = 0;
    Allocator::totalMemory = 0;

    S3::createdObjects = 0;
    S3::destroyedObjects = 0;

    {
        int const numberOfObjects = 1024;

        typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
        AV av;

        for( int i = 0 ; i < numberOfObjects ; ++ i ){
            av.insert( AV::value_type( i, S3() ) );
        }

        AV av2;
        av2 = av;
    }

    AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
    AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
}

#ifdef AV_CXX11X_RVALUE_REFERENCE

    //
    // cxx11x_move_test_1
    //
    void cxx11x_move_test_1()
    {
        typedef MyAllocator< std::pair< int, S3 > > Allocator;

        Allocator::notFreedMemory = 0;
        Allocator::totalMemory = 0;

        S3::createdObjects = 0;
        S3::destroyedObjects = 0;

        S3::moves = 0;
        S3::copies = 0;

        {
            unsigned const counter = 1024;

            typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
            AV av1;

            {// insert( value_type )
                for( unsigned i = 0 ; i < counter ; ++ i ){
                    av1.insert( AV::value_type( i, S3() ) );
                }
            }

            AV_ASSERT_EQUAL( S3::copies, counter );

            AV av2 = AV_MOVE( av1 );

            AV_ASSERT_EQUAL( S3::copies, counter );
        }

        AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
        AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
    }

    //
    // cxx11x_move_test_2
    //
    void cxx11x_move_test_2()
    {
        typedef MyAllocator< std::pair< int, S3 > > Allocator;

        Allocator::notFreedMemory = 0;
        Allocator::totalMemory = 0;

        S3::createdObjects = 0;
        S3::destroyedObjects = 0;

        S3::moves = 0;
        S3::copies = 0;

        {
            unsigned const counter = 1024;

            typedef AssocVector< int, S3, std::less< int >, Allocator > AV;
            AV av;

            {// insert( value_type )
                for( unsigned i = 0 ; i < counter / 2 ; ++ i ){
                    av.insert( AV::value_type( i, S3() ) );
                }
            }

            AV_ASSERT_EQUAL( S3::copies, counter / 2 );

            {// _insert( value_type )
                for( unsigned i = counter / 2 ; i < counter ; ++ i ){
                    av._insert( AV::value_type( i, S3() ) );
                }
            }

            AV_ASSERT_EQUAL( S3::copies, counter );

            {// find( value_type )
                for( unsigned i = 0 ; i < counter / 2 ; ++ i ){
                    av.find( i );
                }
            }

            AV_ASSERT_EQUAL( S3::copies, counter );

            {// _find( value_type )
                for( unsigned i = 0 ; i < counter / 2 ; ++ i ){
                    av._find( i );
                }
            }

            AV_ASSERT_EQUAL( S3::copies, counter );

            {// erase( value_type )
                for( unsigned i = 0 ; i < counter / 2 ; ++ i ){
                    av.erase( i );
                }
            }

            AV_ASSERT_EQUAL( S3::copies, counter );

            {// erase( iterator )
                for( unsigned i = counter / 2 ; i < counter ; ++ i ){
                    av.erase( av.find( i ) );
                }
            }

            AV_ASSERT( S3::copies < 2 * counter );
        }

        AV_ASSERT_EQUAL( Allocator::notFreedMemory, 0 );
        AV_ASSERT_EQUAL( S3::createdObjects, S3::destroyedObjects );
    }

#endif

int main()
{
    {
        std::cout << "Core tests..."; std::flush( std::cout );

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

        test_push_back();

        test_insert_in_random_order();
        test_insert_in_increasing_order();
        test_insert_in_decreasing_order();
        test_insert_check_iterator_1();
        test_insert_check_iterator_2();

        test_insert_erase_insert_1();
        test_insert_erase_insert_2();
        test_insert_erase_insert_3();
        test_insert_erase_insert_4();

        test_find_1();
        test_find_2();
        test_find_check_iterator();

        test_count();

        test_erase_1();
        test_erase_2();
        test_erase_3();
        test_erase_from_back_already_erased();

        test_erase_iterator_1();
        test_erase_iterator_2();
        test_erase_iterator_3();

        test_erase_iterator();
        test_erase_reverse_iterator();

        test_operator_index_1();
        test_operator_index_2();

        test_user_type();

        test_copy_constructor();
        test_assign_operator();
        test_clear();

        test_iterator_to_const_iterator_conversion();

        test_iterators_equal();
        test_reverse_iterators_equal();

        test_iterators_distance();
        test_reverse_iterators_distance();

        test_iterators_begin_equals_end_in_empty_container();
        test_reverse_iterators_begin_equal_end_in_empty_storage();

        test_iterators_increment_decrement_1();
        test_iterators_increment_decrement_2();

        test_iterator_1();

        test_iterator_erased_1();
        test_iterator_erased_2();

        test_iterator_begin();

        test_reverse_iterator_1();
        test_reverse_iterator_2();

        test_iterators_iterate_not_empty_storage_empty_cache();

        std::cout << "OK." << std::endl;
    }

#ifdef AV_CXX11X_RVALUE_REFERENCE
    {
        std::cout << "C++11x tests..."; std::flush( std::cout );

        cxx11x_move_test_1();
        cxx11x_move_test_2();

        std::cout << "OK." << std::endl;
    }
#endif

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

        black_box_test< S1 >();
        black_box_test< S2 >();
        black_box_test< S3 >();

        std::cout << "OK." << std::endl;
    }

    return 0;
}
