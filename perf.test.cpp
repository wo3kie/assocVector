#include <ctime>
#include <iostream>
#include <map>

#include <loki/AssocVector.h>

#include "AssocVector.hpp"

#define UNIT_TESTS

#ifdef UNIT_TESTS

    bool const LOKI = false;
    bool const STL = false;

    unsigned const REPS = 1;
#else
    bool const LOKI = false;
    bool const STL = false;

    unsigned const REPS = 10;
#endif


struct S{
    S & operator=( S const & other ){
        ++counter;

        return * this;
    }

    static int counter;
};

int S::counter = 0;

template< template< typename, typename > class _Storage >
void test_push_increasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;
        
        for( int i = 0 ; i < rep ; ++i ){
            av.insert( std::make_pair( i, S() ) );
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_push_decreasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;
    
        for( int i = rep ; i > 0 ; --i ){
            av.insert( std::make_pair( i, S() ) );
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_push_random( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;

        for( int i = 0 ; i < array.size() ; ++i ){
            av.insert( std::make_pair( array[ i ], S() ) );
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  array.size()
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_erase_increasing( int tests, int rep, std::string const & message )
{
    std::clock_t total_time = 0;
    
    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;

        for( int i = 0 ; i < rep ; ++i ){
            av.insert( std::make_pair( i, S() ) );
        }

        std::clock_t const start( std::clock() );

        for( int i = 0 ; i < rep ; ++i ){
            av.erase( i );
        }

        std::clock_t const end( std::clock() );
        
        total_time += ( end - start );
    }
    
    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << total_time/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_erase_decreasing( int tests, int rep, std::string const & message )
{
    std::clock_t total_time = 0;
    
    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;

        for( int i = 0 ; i < rep ; ++i ){
            av.insert( std::make_pair( i, S() ) );
        }

        std::clock_t const start( std::clock() );

        for( int i = rep ; i > 0 ; --i ){
            av.erase( i );
        }

        std::clock_t const end( std::clock() );
        
        total_time += ( end - start );
    }
    
    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << total_time/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_erase_random( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t total_time = 0;
    
    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;

        for( int i = 0 ; i < array.size() ; ++i ){
            av.insert( std::make_pair( array[ i ], S() ) );
        }

        std::clock_t const start( std::clock() );

        for( int i = 0 ; i < array.size() ; ++i ){
            av.erase( array[ i ] );
        }

        std::clock_t const end( std::clock() );
        
        total_time += ( end - start );
    }

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  array.size()
        << " times: "
        << total_time/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< typename T, typename K >
void merge( Loki::AssocVector< T, K > & ){}

template< typename T, typename K >
void merge( std::map< T, K > & ){}

template< typename T, typename K >
void merge( AssocVector< T, K > & av ){ av.merge(); }

template< template< typename, typename > class _Storage >
void test_find( int tests, int rep, std::string const & message )
{
    _Storage< int, S > av;

    for( int i = 0 ; i < rep ; ++i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        for( int i = 0 ; i < rep ; ++i ){
            av.find( i );
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_increasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;
        
        for( int i = 0 ; i < rep ; ++i ){
            av[ i ] = S();
            S s = av[i];
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_decreasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;
        
        for( int i = rep ; i > 0 ; --i ){
            av[ i ] = S();
            S s = av[i];
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_random( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage< int, S > av;

        for( int i = 0 ; i < array.size() ; ++ i ){
            int const value = array[ i ];

            av[ value ] = S();
            S s = av[ value ];
        }
    }
    
    std::clock_t const end( std::clock() );

    std::cout
        << message
        << ": "
        << tests
        << "x"
        <<  array.size()
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

void push_increasing()
{
    test_push_increasing< AssocVector >( 100 * REPS, 1000, "AssocVector.push_increasing" );
    if( LOKI ) test_push_increasing< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.push_increasing" );
    if( STL ) test_push_increasing< std::map >( 100 * REPS, 1000, "std::map.push_increasing" );

    test_push_increasing< AssocVector >( REPS, 1000000, "AssocVector.push_increasing" );
    if( LOKI ) test_push_increasing< Loki::AssocVector >( REPS, 1000000, "Loki::AssocVector.push_increasing" );
    if( STL ) test_push_increasing< std::map >( REPS, 1000000, "std::map.push_increasing" );
    
    std::cout << std::endl;
}

void push_decreasing()
{
    test_push_decreasing< AssocVector >( 100 * REPS, 1000, "AssocVector.push_decreasing" );
    if( LOKI ) test_push_decreasing< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.push_decreasing" );
    if( STL ) test_push_decreasing< std::map >( 100 * REPS, 1000, "std::map.push_decreasing" );

    test_push_decreasing< AssocVector >( REPS, 1000000, "AssocVector.push_decreasing" );
    if( LOKI ) std::cout << "Loki::AssocVector.push_decreasing: REPSx1000000 times: -s," << std::endl;
    if( STL ) test_push_decreasing< std::map >( REPS, 1000000, "std::map.push_decreasing" );
    
    std::cout << std::endl;
}

void push_random()
{
    std::vector< int > array;

    for( int i = 0 ; i < 1000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_push_random< AssocVector >( 100 * REPS, array, "AssocVector.push_random" );
    if( LOKI ) test_push_random< Loki::AssocVector >( 100 * REPS, array, "Loki::AssocVector.push_random" );
    if( STL ) test_push_random< std::map >( 100 * REPS, array, "std::map.push_random" );

    for( int i = 1000 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() + rand() - rand() );

    test_push_random< AssocVector >( REPS, array, "AssocVector.push_random" );
    if( LOKI ) test_push_random< Loki::AssocVector >( REPS, array, "Loki::AssocVector.push_random" );
    if( STL ) test_push_random< std::map >( REPS, array, "std::map.push_random" );
    
    
    std::cout << std::endl;
}

void index_operator_increasing()
{
    test_index_operator_increasing< AssocVector >( 100 * REPS, 1000, "AssocVector.index_operator_increasing" );
    if( LOKI ) test_index_operator_increasing< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.index_operator_increasing" );
    if( STL ) test_index_operator_increasing< std::map >( 100 * REPS, 1000, "std::map.index_operator_increasing" );

    test_index_operator_increasing< AssocVector >( REPS, 1000000, "AssocVector.index_operator_increasing" );
    if( LOKI ) test_index_operator_increasing< Loki::AssocVector >( REPS, 1000000, "Loki::AssocVector.index_operator_increasing" );
    if( STL ) test_index_operator_increasing< std::map >( REPS, 1000000, "std::map.index_operator_increasing" );
    
    std::cout << std::endl;
}

void index_operator_decreasing()
{
    test_index_operator_decreasing< AssocVector >( 100 * REPS, 1000, "AssocVector.index_operator_decreasing" );
    if( LOKI ) test_index_operator_decreasing< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.index_operator_decreasing" );
    if( STL ) test_index_operator_decreasing< std::map >( 100 * REPS, 1000, "std::map.index_operator_decreasing" );

    test_index_operator_decreasing< AssocVector >( REPS, 1000000, "AssocVector.index_operator_decreasing" );
    if( LOKI ) std::cout << "Loki::AssocVector.index_operator_decreasing: REPSx1000000 times: -s," << std::endl;
    if( STL ) test_index_operator_decreasing< std::map >( REPS, 1000000, "std::map.index_operator_decreasing" );
    
    std::cout << std::endl;
}

void index_operator_random()
{
    std::vector< int > array;

    for( int i = 0 ; i < 1000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_index_operator_random< AssocVector >( 100 * REPS, array, "AssocVector.index_operator_random" );
    if( LOKI ) test_index_operator_random< Loki::AssocVector >( 100 * REPS, array, "Loki::AssocVector.index_operator_random" );
    if( STL ) test_index_operator_random< std::map >( 100 * REPS, array, "std::map.index_operator_random" );

    for( int i = 1000 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() + rand() - rand() );

    test_index_operator_random< AssocVector >( REPS, array, "AssocVector.index_operator_random" );
    if( LOKI ) test_index_operator_random< Loki::AssocVector >( REPS, array, "Loki::AssocVector.index_operator_random" );
    if( STL ) test_index_operator_random< std::map >( REPS, array, "std::map.index_operator_random" );
    
    std::cout << std::endl;
}

void find()
{
    test_find< AssocVector >( 100 * REPS, 1000, "AssocVector.find" );
    if( LOKI ) test_find< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.find" );
    if( STL ) test_find< std::map >( 100 * REPS, 1000, "std::map.find" );

    test_find< AssocVector >( REPS, 1000000, "AssocVector.find" );
    if( LOKI ) test_find< Loki::AssocVector >( REPS, 1000000, "Loki::AssocVector.find" );
    if( STL ) test_find< std::map >( REPS, 1000000, "std::map.find" );
    
    std::cout << std::endl;
}

void erase_increasing()
{
    test_erase_increasing< AssocVector >( 100 * REPS, 1000, "AssocVector.erase_increasing" );
    if( LOKI ) test_erase_increasing< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.erase_increasing" );
    if( STL ) test_erase_increasing< std::map >( 100 * REPS, 1000, "std::map.erase_increasing" );

    test_erase_increasing< AssocVector >( REPS, 1000000, "AssocVector.erase_increasing" );
    if( LOKI ) std::cout << "Loki::AssocVector.erase_increasing: REPSx1000000 times: -s," << std::endl;
    if( STL ) test_erase_increasing< std::map >( REPS, 1000000, "std::map.erase_increasing" );
    
    std::cout << std::endl;
}

void erase_decreasing()
{
    test_erase_decreasing< AssocVector >( 100 * REPS, 1000, "AssocVector.erase_decreasing" );
    if( LOKI ) test_erase_decreasing< Loki::AssocVector >( 100 * REPS, 1000, "Loki::AssocVector.erase_decreasing" );
    if( STL ) test_erase_decreasing< std::map >( 100 * REPS, 1000, "std::map.erase_decreasing" );

    test_erase_decreasing< AssocVector >( REPS, 1000000, "AssocVector.erase_decreasing" );
    if( LOKI ) test_erase_decreasing< Loki::AssocVector >( REPS, 1000000, "Loki::AssocVector.erase_decreasing" );
    if( STL ) test_erase_decreasing< std::map >( REPS, 1000000, "std::map.erase_decreasing" );
    
    std::cout << std::endl;
}

void erase_random()
{
    std::vector< int > array;

    for( int i = 0 ; i < 1000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_erase_random< AssocVector >( 100 * REPS, array, "AssocVector.erase_random" );
    if( LOKI ) test_erase_random< Loki::AssocVector >( 100 * REPS, array, "Loki::AssocVector.erase_random" );
    if( STL ) test_erase_random< std::map >( 100 * REPS, array, "std::map.erase_random" );

    for( int i = 1000 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() + rand() - rand() );

    test_erase_random< AssocVector >( REPS, array, "AssocVector.erase_random" );
    if( LOKI ) test_erase_random< Loki::AssocVector >( REPS, array, "Loki::AssocVector.erase_random" );
    if( STL ) test_erase_random< std::map >( REPS, array, "std::map.erase_random" );
    
    std::cout << std::endl;
}

int main()
{
    std::cout
        << std::string( 10, '*' ) 
    
    #ifdef UNIT_TESTS
        << " Run as: Unit Tests "
    #else    
        << " Run as: Perf Tests "
    #endif
    
        << std::string( 10, '*' )
        << "\n"
        << std::endl;

    //push_increasing();
    //push_decreasing();
    //push_random();

    //index_operator_increasing();
    //index_operator_decreasing();
    //index_operator_random();

    //find();

    erase_increasing();
    erase_decreasing();
    erase_random();
}
