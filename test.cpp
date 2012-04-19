#include <cassert>
#include <string>

#include "AssocVector.hpp"

//
// test_CmpByFirst
//
void test_CmpByFirst()
{
    {
        typedef std::pair< int, int > Pii;
        
        Pii p1( 1, 11 );
        Pii p2( 1, 22 );
        
        assert( ( detail::CmpByFirst< Pii, std::equal_to< int > >()( p1, p2 ) ) );
        
        assert( ( detail::CmpByFirst< Pii, std::equal_to< int > >()( p1, 1 ) ) );
    }
    {
        typedef std::pair< int, int > Pii;
        
        Pii p1( 1, 22 );
        Pii p2( 2, 11 );
        
        assert( ( detail::CmpByFirst< Pii, std::less< int > >()( p1, p2 ) ) );
        
        assert( ( detail::CmpByFirst< Pii, std::less< int > >()( p1, 2 ) ) );
    }
    {
        typedef std::pair< int, int > Pii;
        
        Pii p1( 2, 11 );
        Pii p2( 1, 22 );
        
        assert( ( detail::CmpByFirst< Pii, std::greater< int > >()( p1, p2 ) ) );
        
        assert( ( detail::CmpByFirst< Pii, std::greater< int > >()( p1, 1 ) ) );
    }
}

//
// test_isBetween
//
void test_isBetween()
{
    assert( ( detail::isBetween( 1, 0, 3 ) == false ) );
    assert( ( detail::isBetween( 1, 1, 3 ) ) );
    assert( ( detail::isBetween( 1, 2, 3 ) ) );
    assert( ( detail::isBetween( 1, 3, 3 ) ) );
    assert( ( detail::isBetween( 1, 4, 3 ) == false ) );
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
        int _some_data_to_make_sizeof_not_zero;
    };

    int For_ConstructRange_DestroyRange_Tests::counter = 0;
}

//
// test_constructRange_destroyRange
//
void test_constructRange_destroyRange()
{
    int const numberOfObjects = 10;

    std::allocator< detail::For_ConstructRange_DestroyRange_Tests > allocator;
    assert( detail::For_ConstructRange_DestroyRange_Tests::counter == 0 );

    detail::For_ConstructRange_DestroyRange_Tests * memory = allocator.allocate( numberOfObjects );
    assert( detail::For_ConstructRange_DestroyRange_Tests::counter == 0 );

    detail::construct_range( memory, memory + numberOfObjects );
    assert( detail::For_ConstructRange_DestroyRange_Tests::counter == 10 );

    detail::destroy_range( memory, memory + numberOfObjects );
    assert( detail::For_ConstructRange_DestroyRange_Tests::counter == 0 );

    allocator.deallocate( memory, numberOfObjects );
}

//
// test_push_back
//
void test_push_back_1()
{
    typedef AssocVector< std::string, int > AssocVector;
    typedef AssocVector::iterator Iterator;

    AssocVector av;

    av.insert( AssocVector::value_type( "1", 1 ) );
    av.insert( AssocVector::value_type( "2", 2 ) );
    av.insert( AssocVector::value_type( "3", 3 ) );
    av.insert( AssocVector::value_type( "4", 4 ) );
    av.insert( AssocVector::value_type( "5", 5 ) );

    assert( av[ "1" ] == 1 );
    assert( av[ "2" ] == 2 );
    assert( av[ "3" ] == 3 );
    assert( av[ "4" ] == 4 );
    assert( av[ "5" ] == 5 );
}


int main()
{
    test_CmpByFirst();

    test_isBetween();
 
    test_constructRange_destroyRange();
 
    test_push_back_1();
 
    return 0;
}

