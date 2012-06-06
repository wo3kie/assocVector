#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "AssocVector.hpp"

#define AV_ASSERT( expression )\
    assert( expression )

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

namespace util
{

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

    assert( result1 == result2 );

    return result1;
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

void test_CmpByFirst_2()
{
        typedef std::pair< int, int > Pii;

        Pii p1( 1, 22 );
        Pii p2( 2, 11 );

        AV_ASSERT( ( util::CmpByFirst< Pii, std::less< int > >()( p1, p2 ) ) );
        AV_ASSERT( ( util::CmpByFirst< Pii, std::less< int > >()( p1, 2 ) ) );

        AV_ASSERT( ( util::CmpByFirst< Pii, std::greater< int > >()( p1, p2 ) == false ) );
}

void test_CmpByFirst_3()
{
    typedef std::pair< int, int > Pii;

    Pii p1( 2, 11 );
    Pii p2( 1, 22 );

    AV_ASSERT( ( util::CmpByFirst< Pii, std::greater< int > >()( p1, p2 ) ) );
    AV_ASSERT( ( util::CmpByFirst< Pii, std::greater< int > >()( p1, 1 ) ) );

    AV_ASSERT( ( util::CmpByFirst< Pii, std::less< int > >()( p1, p2 ) == false ) );
}

//
// test_isBetween
//
void test_isBetween()
{
    AV_ASSERT( ( util::isBetween( 1, 0, 3 ) == false ) );
    AV_ASSERT( ( util::isBetween( 1, 1, 3 ) ) );
    AV_ASSERT( ( util::isBetween( 1, 2, 3 ) ) );
    AV_ASSERT( ( util::isBetween( 1, 3, 3 ) ) );
    AV_ASSERT( ( util::isBetween( 1, 4, 3 ) == false ) );
}

namespace detail
{
    //
    // For_ConstructRange_DestroyRange_Tests
    //
    struct For_ConstructRange_DestroyRange_Tests
    {
        For_ConstructRange_DestroyRange_Tests(){ ++ counter; }
        ~For_ConstructRange_DestroyRange_Tests(){ -- counter; }

    public:
        static int counter;

    private:
        int _some_data_to_makes_size_of_not_zero;
    };

    int For_ConstructRange_DestroyRange_Tests::counter = 0;
}

//
// test_copyRange
//
void test_copyRange_empty_array()
{
    std::vector< int > array;
    std::vector< int > array2;

    util::copyRange( array.begin(), array.end(), array2.begin() );

    AV_ASSERT( array2.empty() );
}

void test_copyRange_self_copy()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::copyRange( array.begin(), array.end(), array.begin() );

    std::vector< int > expected;
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );
    expected.push_back( 4 );
    expected.push_back( 5 );
    expected.push_back( 6 );

    AV_ASSERT( array == expected );
}

void test_copyRange_between_different_containers()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    std::vector< int > array2( 6, 0 );

    util::copyRange( array.begin(), array.end(), array2.begin() );

    AV_ASSERT( array == array2 );
}

void test_copyRange_overlap_less_then_half()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::copyRange( array.begin(), array.begin() + 3, array.begin() + 3 );

    std::vector< int > expected;
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );

    AV_ASSERT( array == expected );
}
void test_copyRange_overlap_more_than_half()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::copyRange( array.begin(), array.begin() + 4, array.begin() + 2 );

    std::vector< int > expected;
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 1 );
    expected.push_back( 2 );
    expected.push_back( 3 );
    expected.push_back( 4 );

    AV_ASSERT( array == expected );
}

void test_copyRange_overlap_copy_to_begining()
{
    std::vector< int > array;
    array.push_back( 1 );
    array.push_back( 2 );
    array.push_back( 3 );
    array.push_back( 4 );
    array.push_back( 5 );
    array.push_back( 6 );

    util::copyRange( array.begin() + 2, array.end(), array.begin() );

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

        assert( util::last_less_equal( v.begin(), v.end(), 1, std::less< int >() ) == v.end() );
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
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == i );

            i = 3;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == i );

            i = 5;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == i );

            i = 7;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == i );

            i = 9;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == i );
        }
        {
            int i = 0;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == v.end() );

            i = 2;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == (i-1) );

            i = 4;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == (i-1) );

            i = 6;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == (i-1) );

            i = 8;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == (i-1) );

            i = 10;
            assert( util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) != v.end() );
            assert( * util::last_less_equal( v.begin(), v.end(), i, std::less< int >() ) == (i-1) );
        }
    }
}

//
// test_merge
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

        a2[0] = 1;
        a2[1] = 2;
        a2[2] = 3;
        a2[3] = 4;

        a2.setSize( 4 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 4 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 4 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );

        a1[0] = 1;
        a1[1] = 2;
        a1[2] = 3;
        a1[3] = 4;

        a1.setSize( 4 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );
        a2.setSize( 0 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 4 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 0 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );

        a1[0] = 1;
        a1[1] = 2;
        a1[2] = 3;
        a1[3] = 4;
        a1[4] = 5;
        a1[5] = 6;

        a1.setSize( 6 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );

        a2[0] = 7;
        a2[1] = 8;
        a2[2] = 9;
        a2[3] = 10;

        a2.setSize( 4 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 10 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 4 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );
        AV_ASSERT( a1[4] == 5 );
        AV_ASSERT( a1[5] == 6 );
        AV_ASSERT( a1[6] == 7 );
        AV_ASSERT( a1[7] == 8 );
        AV_ASSERT( a1[8] == 9 );
        AV_ASSERT( a1[9] == 10 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );

        a1[0] = 1;
        a1[1] = 3;
        a1[2] = 5;
        a1[3] = 7;
        a1[4] = 9;
        a1[5] = 11;

        a1.setSize( 6 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );

        a2[0] = 2;
        a2[1] = 4;
        a2[2] = 6;
        a2[3] = 8;

        a2.setSize( 4 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 10 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 4 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );
        AV_ASSERT( a1[4] == 5 );
        AV_ASSERT( a1[5] == 6 );
        AV_ASSERT( a1[6] == 7 );
        AV_ASSERT( a1[7] == 8 );
        AV_ASSERT( a1[8] == 9 );
        AV_ASSERT( a1[9] == 11 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );

        a1[0] = 5;
        a1[1] = 6;
        a1[2] = 7;
        a1[3] = 8;
        a1[4] = 9;
        a1[5] = 10;

        a1.setSize( 6 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );

        a2[0] = 1;
        a2[1] = 2;
        a2[2] = 3;
        a2[3] = 4;

        a2.setSize( 4 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 10 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 4 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );
        AV_ASSERT( a1[4] == 5 );
        AV_ASSERT( a1[5] == 6 );
        AV_ASSERT( a1[6] == 7 );
        AV_ASSERT( a1[7] == 8 );
        AV_ASSERT( a1[8] == 9 );
        AV_ASSERT( a1[9] == 10 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );

        a1[0] = 1;
        a1[1] = 2;
        a1[2] = 3;
        a1[3] = 8;
        a1[4] = 9;
        a1[5] = 10;

        a1.setSize( 6 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );

        a2[0] = 4;
        a2[1] = 5;
        a2[2] = 6;
        a2[3] = 7;

        a2.setSize( 4 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 10 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 4 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );
        AV_ASSERT( a1[4] == 5 );
        AV_ASSERT( a1[5] == 6 );
        AV_ASSERT( a1[6] == 7 );
        AV_ASSERT( a1[7] == 8 );
        AV_ASSERT( a1[8] == 9 );
        AV_ASSERT( a1[9] == 10 );

        delete [] a1.getData();
        delete [] a2.getData();
    }

    {
        Array< int > a1;
        a1.setData( new int[ 10 ] );
        a1.setCapacity( 10 );

        a1[0] = 2;
        a1[1] = 3;
        a1[2] = 5;
        a1[3] = 6;
        a1[4] = 8;
        a1[5] = 9;

        a1.setSize( 6 );

        Array< int > a2;
        a2.setData( new int[ 10 ] );
        a2.setCapacity( 10 );

        a2[0] = 1;
        a2[1] = 4;
        a2[2] = 7;
        a2[3] = 10;

        a2.setSize( 4 );

        array::merge( a1, a2, std::less< int >() );

        AV_ASSERT( a1.size() == 10 );
        AV_ASSERT( a1.capacity() == 10 );

        AV_ASSERT( a2.size() == 4 );
        AV_ASSERT( a2.capacity() == 10 );

        AV_ASSERT( a1[0] == 1 );
        AV_ASSERT( a1[1] == 2 );
        AV_ASSERT( a1[2] == 3 );
        AV_ASSERT( a1[3] == 4 );
        AV_ASSERT( a1[4] == 5 );
        AV_ASSERT( a1[5] == 6 );
        AV_ASSERT( a1[6] == 7 );
        AV_ASSERT( a1[7] == 8 );
        AV_ASSERT( a1[8] == 9 );
        AV_ASSERT( a1[9] == 10 );

        delete [] a1.getData();
        delete [] a2.getData();
    }
}

void test_merge_2()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 15 ; ++ i ){
        storage[ storage.size() ] = std::pair< int, int >( i, 0 );
        storage.setSize( storage.size() + 1 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 3 );

    for( int i = 3 ; i < 6 ; ++ i ){
        buffer[ buffer.size() ] = std::pair< int, int >( i, 0 );
        buffer.setSize( buffer.size() + 1 );
    }

    array::merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT( storage.capacity() == 10 );
    AV_ASSERT( storage.size() == 8 );

    AV_ASSERT( buffer.size() == 3 );
    AV_ASSERT( buffer.capacity() == 3 );

    AV_ASSERT( storage[0].first == 3 );
    AV_ASSERT( storage[1].first == 4 );
    AV_ASSERT( storage[2].first == 5 );
    AV_ASSERT( storage[3].first == 10 );
    AV_ASSERT( storage[4].first == 11 );
    AV_ASSERT( storage[5].first == 12 );
    AV_ASSERT( storage[6].first == 13 );
    AV_ASSERT( storage[7].first == 14 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

void test_merge_3()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 15 ; ++ i ){
        storage[ storage.size() ] = std::pair< int, int >( i, 0 );
        storage.setSize( storage.size() + 1 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 15 );

    for( int i = 23 ; i < 26 ; ++ i ){
        buffer[ buffer.size() ] = std::pair< int, int >( i, 0 );
        buffer.setSize( buffer.size() + 1 );
    }

    array::merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT( storage.capacity() == 10 );
    AV_ASSERT( storage.size() == 8 );

    AV_ASSERT( buffer.size() == 3 );
    AV_ASSERT( buffer.capacity() == 15 );

    AV_ASSERT( storage[0].first == 10 );
    AV_ASSERT( storage[1].first == 11 );
    AV_ASSERT( storage[2].first == 12 );
    AV_ASSERT( storage[3].first == 13 );
    AV_ASSERT( storage[4].first == 14 );
    AV_ASSERT( storage[5].first == 23 );
    AV_ASSERT( storage[6].first == 24 );
    AV_ASSERT( storage[7].first == 25 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

void test_merge_4()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 20 ; i += 2 ){
        storage[ storage.size() ] = std::pair< int, int >( i, 0 );
        storage.setSize( storage.size() + 1 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 10 );

    for( int i = 13 ; i < 19 ; i += 2 ){
        buffer[ buffer.size() ] = std::pair< int, int >( i, 0 );
        buffer.setSize( buffer.size() + 1 );
    }

    array::merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT( storage.capacity() == 10 );
    AV_ASSERT( storage.size() == 8 );

    AV_ASSERT( buffer.size() == 3 );
    AV_ASSERT( buffer.capacity() == 10 );

    AV_ASSERT( storage[0].first == 10 );
    AV_ASSERT( storage[1].first == 12 );
    AV_ASSERT( storage[2].first == 13 );
    AV_ASSERT( storage[3].first == 14 );
    AV_ASSERT( storage[4].first == 15 );
    AV_ASSERT( storage[5].first == 16 );
    AV_ASSERT( storage[6].first == 17 );
    AV_ASSERT( storage[7].first == 18 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

void test_merge_5()
{
    array::Array< std::pair< int, int > > storage;

    storage.setData( new std::pair< int, int >[ 10 ] );
    storage.setSize( 0 );
    storage.setCapacity( 10 );

    for( int i = 10 ; i < 20 ; i += 2 ){
        storage[ storage.size() ] = std::pair< int, int >( i, 0 );
        storage.setSize( storage.size() + 1 );
    }

    array::Array< std::pair< int, int > > buffer;

    buffer.setData( new std::pair< int, int >[ 3 ] );
    buffer.setSize( 0 );
    buffer.setCapacity( 3 );

    array::merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT( storage.capacity() == 10 );
    AV_ASSERT( storage.size() == 5 );

    AV_ASSERT( buffer.size() == 0 );
    AV_ASSERT( buffer.capacity() == 3 );

    AV_ASSERT( storage[0].first == 10 );
    AV_ASSERT( storage[1].first == 12 );
    AV_ASSERT( storage[2].first == 14 );
    AV_ASSERT( storage[3].first == 16 );
    AV_ASSERT( storage[4].first == 18 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

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
        buffer[ buffer.size() ] = std::pair< int, int >( i, 0 );
        buffer.setSize( buffer.size() + 1 );
    }

    array::merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT( storage.capacity() == 10 );
    AV_ASSERT( storage.size() == 3 );

    AV_ASSERT( buffer.size() == 3 );
    AV_ASSERT( buffer.capacity() == 20 );

    AV_ASSERT( storage[0].first == 13 );
    AV_ASSERT( storage[1].first == 15 );
    AV_ASSERT( storage[2].first == 17 );

    delete [] storage.getData();
    delete [] buffer.getData();
}

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

    array::merge( storage, buffer, util::CmpByFirst< std::pair< int, int >, std::less< int > >() );

    AV_ASSERT( storage.capacity() == 10 );
    AV_ASSERT( storage.size() == 0 );

    AV_ASSERT( buffer.size() == 0 );
    AV_ASSERT( buffer.capacity() == 3 );

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

    AV_ASSERT( av[ "1" ] == 1 );
    AV_ASSERT( av[ "2" ] == 2 );
    AV_ASSERT( av[ "3" ] == 3 );
    AV_ASSERT( av[ "4" ] == 4 );
    AV_ASSERT( av[ "5" ] == 5 );
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
    AV_ASSERT( p.first->first == 10 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 9, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 9 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 8, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 8 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 16, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 16 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 17, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 17 );
    AV_ASSERT( p.second == true );

    {
        p = av.insert( AssocVector::value_type( 17, 0 ) );
        AV_ASSERT( p.first != av.end() );
        AV_ASSERT( p.first->first == 17 );
        AV_ASSERT( p.second == false );
    }

    p = av.insert( AssocVector::value_type( 18, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 18 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 7, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 7 );
    AV_ASSERT( p.second == true );

    {
        p = av.insert( AssocVector::value_type( 7, 0 ) );
        AV_ASSERT( p.first != av.end() );
        AV_ASSERT( p.first->first == 7 );
        AV_ASSERT( p.second == false );
    }

    p = av.insert( AssocVector::value_type( 6, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 6 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 5, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 5 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 4, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 4 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 11, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 11 );
    AV_ASSERT( p.second == true );

    {
        p = av.insert( AssocVector::value_type( 11, 0 ) );
        AV_ASSERT( p.first != av.end() );
        AV_ASSERT( p.first->first == 11 );
        AV_ASSERT( p.second == false );
    }

    p = av.insert( AssocVector::value_type( 12, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 12 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 13, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 13 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 3, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 3 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 2, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 2 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 1, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 1 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 0, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 0 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 14, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 14 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 15, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 15 );
    AV_ASSERT( p.second == true );

    p = av.insert( AssocVector::value_type( 19, 0 ) );
    AV_ASSERT( p.first != av.end() );
    AV_ASSERT( p.first->first == 19 );
    AV_ASSERT( p.second == true );

    AV_ASSERT( av.size() == 20 );

    for( int i = 0 ; i < av.size() ; ++ i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT( av.find( i )->first == i );
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
        AV_ASSERT( p.first->first == i );
        AV_ASSERT( p.second == true );
    }

    for( std::size_t i = 0 ; i < counter ; ++ i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT( av.find( i )->first == i );
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
        AV_ASSERT( p.first->first == i );
        AV_ASSERT( p.second == true );
    }

    for( std::size_t i = counter ; i > 0 ; -- i ){
        AV_ASSERT( av.find( i ) != av.end() );
        AV_ASSERT( av.find( i )->first == i );
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

    AV_ASSERT( util::isEqual( av, map ) );

    {// insert at the beginnig
        AVIterator const avInserted = av.insert( AssocVector::value_type( 0, 0 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 0, 0 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {// insert at the end
        AVIterator const avInserted = av.insert( AssocVector::value_type( 33, 33 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 33, 33 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {// insert in middle
        AVIterator const avInserted = av.insert( AssocVector::value_type( 1, 1 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 1, 1 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {//insert in middle
        AVIterator const avInserted = av.insert( AssocVector::value_type( 11, 11 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 11, 11 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {//insert in middle
        AVIterator const avInserted = av.insert( AssocVector::value_type( 21, 21 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 21, 21 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
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

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );

        avInserted = av.insert( AssocVector::value_type( 8, 8 ) ).first;
        mapInserted = map.insert( Map::value_type( 8, 8 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );

        avInserted = av.insert( AssocVector::value_type( 10, 10 ) ).first;
        mapInserted = map.insert( Map::value_type( 10, 10 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );

        avInserted = av.insert( AssocVector::value_type( 12, 12 ) ).first;
        mapInserted = map.insert( Map::value_type( 12, 12 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
        );

        AV_ASSERT( std::equal( av.begin(), avInserted, map.begin() ) );
        AV_ASSERT( std::equal( avInserted, av.end(), mapInserted ) );
    }

    {// erase item and put it back again
        av.erase( 12 );
        map.erase( 12 );

        AVIterator const avInserted = av.insert( AssocVector::value_type( 12, 12 ) ).first;
        MapIterator const mapInserted = map.insert( Map::value_type( 12, 12 ) ).first;

        AV_ASSERT(
               std::distance( av.begin(), avInserted )
            == std::distance( map.begin(), mapInserted )
        );

        AV_ASSERT(
               std::distance( avInserted, av.end() )
            == std::distance( mapInserted,map.end() )
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

    AV_ASSERT( av.erase( 3 ) == 1 );

    AV_ASSERT( av.insert( AssocVector::value_type( 3, 33 ) ).second == true );

    AV_ASSERT( av.size() == 5 );

    AV_ASSERT( av[ 1 ] == 1 );
    AV_ASSERT( av[ 2 ] == 2 );
    AV_ASSERT( av[ 3 ] == 33 );
    AV_ASSERT( av[ 4 ] == 4 );
    AV_ASSERT( av[ 5 ] == 5 );
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

    AV_ASSERT( av.erase( 5 ) == 1 );

    AV_ASSERT( av.insert( AssocVector::value_type( 5, 55 ) ).second == true );

    AV_ASSERT( av.size() == 5 );

    AV_ASSERT( av[ 1 ] == 1 );
    AV_ASSERT( av[ 2 ] == 2 );
    AV_ASSERT( av[ 3 ] == 3 );
    AV_ASSERT( av[ 4 ] == 4 );
    AV_ASSERT( av[ 5 ] == 55 );
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

    AV_ASSERT( av.erase( 3 ) == 1 );

    av[ 3 ] = 33;

    AV_ASSERT( av.size() == 5 );

    AV_ASSERT( av[ 1 ] == 1 );
    AV_ASSERT( av[ 2 ] == 2 );
    AV_ASSERT( av[ 3 ] == 33 );
    AV_ASSERT( av[ 4 ] == 4 );
    AV_ASSERT( av[ 5 ] == 5 );
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

    AV_ASSERT( av.erase( 5 ) == 1 );

    av[ 5 ] = 55;

    AV_ASSERT( av.size() == 5 );

    AV_ASSERT( av[ 1 ] == 1 );
    AV_ASSERT( av[ 2 ] == 2 );
    AV_ASSERT( av[ 3 ] == 3 );
    AV_ASSERT( av[ 4 ] == 4 );
    AV_ASSERT( av[ 5 ] == 55 );
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
    AV_ASSERT( av.find( 2 )->second == 3 );
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
        AV_ASSERT( av.find( i )->second == i * i );
    }

    for( int i = 32 + 64 ; i < 32 + 64 + 32; ++ i ){
        AV_ASSERT( av.find( i ) == av.end() );
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

    AV_ASSERT( av.count( 1 ) == 1 );
    AV_ASSERT( av.count( 2 ) == 0 );
    AV_ASSERT( av.count( 3 ) == 1 );
    AV_ASSERT( av.count( 4 ) == 0 );
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

    AV_ASSERT( av.size() == counter );

    for( std::size_t i = 0 ; i < counter ; i += 2 ){
        AV_ASSERT( av.erase( i ) == 1 );
    }

    AV_ASSERT( av.size() == counter / 2 );

    for( std::size_t i = 0 ; i < counter ; i += 2 ){
        AV_ASSERT( av.find( i ) == av.end() );
    }

    for( std::size_t i = 1 ; i < counter ; i += 2 ){
        AV_ASSERT( av.erase( i ) == 1 );
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
        AV_ASSERT( av.size() == counter - i - 1 );
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

    AV_ASSERT( av.erase( 1 ) == 1 );
    AV_ASSERT( av.erase( 1 ) == 0 );

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT( av.erase( 1 ) == 1 );
    AV_ASSERT( av.erase( 1 ) == 0 );
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
    AV_ASSERT( av.size() == 1 );
    AV_ASSERT( av.find( 1 ) != av.end() );
    AV_ASSERT( av.find( 1 )->second == 1 );

    av[ 1 ] = 11;
    AV_ASSERT( av[ 1 ] == 11 );

    av[ 3 ] = 33;
    AV_ASSERT( av[ 3 ] == 33 );

    av[ 2 ] = 22;
    AV_ASSERT( av[ 2 ] == 22 );
}

void test_operator_index_2()
{
    int const counter = 20000;

    std::vector< int > randomValues( counter, 0 );
    std::generate( randomValues.begin(), randomValues.end(), & rand );

    typedef AssocVector< int, int > AssocVector;
    AssocVector av;

    for( int i = 0 ; i < counter ; ++ i ){
        av[ randomValues[ i ] ] = i;
        AV_ASSERT( av[ randomValues[ i ] ] == i );
    }
}

namespace detail
{
    struct For_Test_User_Type_K
    {
        bool operator<( For_Test_User_Type_K const & k )const{return false;}
    };

    std::ostream & operator<<( std::ostream & out , For_Test_User_Type_K const & )
    {
        return out << "[]";
    }

    struct For_Test_User_Type_M
    {
    };

    std::ostream & operator<<( std::ostream & out , For_Test_User_Type_M const & )
    {
        return out << "[]";
    }
}

//
// test_user_type
//
void test_user_type()
{
    typedef AssocVector< detail::For_Test_User_Type_K, detail::For_Test_User_Type_M > AssocVector;

    AssocVector av;
    av.insert( AssocVector::value_type( detail::For_Test_User_Type_K(), detail::For_Test_User_Type_M() ) );
    detail::For_Test_User_Type_M & m = av[ detail::For_Test_User_Type_K() ];
    av[ detail::For_Test_User_Type_K() ] = m;
    av.find( detail::For_Test_User_Type_K() );
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

    AV_ASSERT( assocVector.erase( "a" ) == 1 );

    AV_ASSERT( assocVector.size() == 4 );

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
    AV_ASSERT( ( std::distance( av.begin(), av.end() ) == 0 ) );

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT( av.begin() != av.end() );
    AV_ASSERT( ( std::distance( av.begin(), av.end() ) == 1 ) );

    av.insert( AssocVector::value_type( 2, 2 ) );
    AV_ASSERT( ( std::distance( av.begin(), av.end() ) == 2 ) );

    av.insert( AssocVector::value_type( 3, 3 ) );
    AV_ASSERT( ( std::distance( av.begin(), av.end() ) == 3 ) );
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
    AV_ASSERT( ( std::distance( av.rbegin(), av.rend() ) == 0 ) );

    av.insert( AssocVector::value_type( 1, 1 ) );

    AV_ASSERT( av.rbegin() != av.rend() );
    AV_ASSERT( ( std::distance( av.rbegin(), av.rend() ) == 1 ) );

    av.insert( AssocVector::value_type( 2, 2 ) );
    AV_ASSERT( ( std::distance( av.rbegin(), av.rend() ) == 2 ) );

    av.insert( AssocVector::value_type( 3, 3 ) );
    AV_ASSERT( ( std::distance( av.rbegin(), av.rend() ) == 3 ) );
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
// test_iterators_increment_decrement
//
void test_iterators_increment_decrement()
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

    AV_ASSERT( current->first == 0 );
    ++current;
    AV_ASSERT( current->first == 1 );
    ++current;
    AV_ASSERT( current->first == 2 );
    ++current;
    AV_ASSERT( current->first == 3 );
    ++current;
    AV_ASSERT( current->first == 4 );
    ++current;
    AV_ASSERT( current->first == 5 );
    ++current;
    AV_ASSERT( current->first == 6 );
    ++current;
    AV_ASSERT( current == av.end() );
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

    AV_ASSERT( current->first == 6 );
    ++current;
    AV_ASSERT( current->first == 5 );
    ++current;
    AV_ASSERT( current->first == 4 );
    ++current;
    AV_ASSERT( current->first == 3 );
    ++current;
    AV_ASSERT( current->first == 2 );
    ++current;
    AV_ASSERT( current->first == 1 );
    ++current;
    AV_ASSERT( current->first == 0 );
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

    AV_ASSERT( av.erase( 1 ) == 1 );
    AV_ASSERT( av.erase( 3 ) == 1 );

    RIterator current = av.rbegin();

    AV_ASSERT( current->first == 6 );
    ++current;
    AV_ASSERT( current->first == 5 );
    ++current;
    AV_ASSERT( current->first == 4 );
    ++current;
    AV_ASSERT( current->first == 2 );
    ++current;
    AV_ASSERT( current->first == 0 );
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

    AV_ASSERT( av.size() == 1024 );
    AV_ASSERT( av.bufferSize() == 0 );
    AV_ASSERT( av.storageSize() == 1024 );

    Iterator current = av.begin();
    Iterator const end = av.end();

    for( int i = 0 ; current != end ; ++ current, ++i ){
        AV_ASSERT( current->first == i );
    }
}

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
        AV_ASSERT( av.erase( 5 ) == 0 );

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
        AV_ASSERT( av.erase( 4 ) == 1 );

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
        AV_ASSERT( av.erase( 1 ) == 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.end() );
    }
    {
        AV_ASSERT( av.erase( 2 ) == 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.end() );
    }
    {
        AV_ASSERT( av.erase( 3 ) == 1 );

        AVII::iterator current = av.begin();

        AV_ASSERT( current == av.end() );
    }
}

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
        AV_ASSERT( av.erase( 5 ) == 0 );

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
        AV_ASSERT( av.erase( 4 ) == 1 );

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
        AV_ASSERT( av.erase( 1 ) == 1 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( ( * current ).first == 2 && ( * current ).second == 22 );

        ++ current;
        AV_ASSERT( current == av.rend() );
    }
    {
        AV_ASSERT( av.erase( 2 ) == 1 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( ( * current ).first == 3 && ( * current ).second == 33 );

        ++ current;
        AV_ASSERT( current == av.rend() );
    }
    {
        AV_ASSERT( av.erase( 3 ) == 1 );

        AVII::reverse_iterator current = av.rbegin();

        AV_ASSERT( current == av.rend() );
    }
}

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

    int i;
};

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

struct S3
{
    S3()
    {
        for( int i = 0 ; i < 10 ; ++ i ){
            array.push_back( rand() );
        }
    }

    S3 & operator=( S3 const & other )
    {
        array = other.array;

        return * this;
    }

    bool operator==( S3 const & other )const
    {
        return array == other.array;
    }

    std::vector< int > array;
};

std::ostream & operator<<( std::ostream & out, S3 const & s3 ){
    util::dump( s3.array.begin(), s3.array.end(), out );

    return out;
}

template< typename _T >
void black_box_test()
{
    typedef AssocVector< int, _T > AV;
    AV av;

    typedef std::map< int, _T > MAP;
    MAP map;

    AV_ASSERT( util::isEqual( av, map ) );

    for( int i = 0 ; i < 1024 * 8 ; ++ i )
    {
        int const operation = rand() % 5;

        switch( operation )
        {
            case 0:
                {
                    int const key = rand() % 512;
                    _T const value = _T();

                    av.insert( typename AV::value_type( key, value ) );
                    map.insert( typename MAP::value_type( key, value ) );

                    AV_ASSERT( util::isEqual( av, map ) );
                }

                break;

            case 1:
                {
                    int const key = rand() % 512;

                    typename AV::iterator foundAV = av.find( key );
                    typename MAP::iterator foundMap = map.find( key );

                    AV_ASSERT(
                           ( foundAV == av.end() && foundMap == map.end() )
                        || ( * foundAV == * foundMap )
                    );

                    AV_ASSERT( util::isEqual( av, map ) );
                }

                break;

            case 2:
                {
                    int const key = rand() % 512;

                    AV_ASSERT( av.erase( key ) == map.erase( key ) );

                    AV_ASSERT( util::isEqual( av, map ) );
                }

            case 3:
                {
                    int const key = rand() % 512;

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

                    AV_ASSERT( util::isEqual( av, map ) );
                }

                break;

            case 4:
                {
                    int const key = rand() % 512;
                    _T const value = _T();

                    av[ key ] = value;
                    map[ key ] = value;

                    AV_ASSERT( util::isEqual( av, map ) );
                }

                break;
        }
    }
}

int main()
{
    black_box_test< S1 >();
    black_box_test< S2 >();
    black_box_test< S3 >();

    test_CmpByFirst_1();
    test_CmpByFirst_2();
    test_CmpByFirst_3();

    test_isBetween();

    test_copyRange_empty_array();
    test_copyRange_self_copy();
    test_copyRange_between_different_containers();
    test_copyRange_overlap_less_then_half();
    test_copyRange_overlap_more_than_half();
    test_copyRange_overlap_copy_to_begining();

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
    test_count();

    test_erase_1();
    test_erase_2();
    test_erase_3();

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

    test_iterators_increment_decrement();

    test_iterator_1();
    test_reverse_iterator_1();
    test_reverse_iterator_2();

    test_iterators_iterate_not_empty_storage_empty_cache();

    std::cout << "OK" << std::endl;

    return 0;
}

