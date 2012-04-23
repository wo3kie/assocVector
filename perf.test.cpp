#include <ctime>
#include <iostream>
#include <map>

#include <loki/AssocVector.h>

#include "AssocVector.hpp"

struct S{
    S & operator=( S const & other ){
        ++counter;

        return * this;
    }

    static int counter;
};

int S::counter = 0;

template< template< typename, typename > class _Storage >
void test_push_increasing( int _N, std::string const & message )
{
    std::clock_t const start( std::clock() );

    _Storage< int, S > av;

    for( int i = 0 ; i < _N ; ++i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_push_decreasing( int _N, std::string const & message )
{
    std::clock_t const start( std::clock() );

    _Storage< int, S > av;

    for( int i = _N ; i > 0 ; --i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_push_random( std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    _Storage< int, S > av;

    for( int i = 0 ; i < array.size() ; ++i ){
        av.insert( std::make_pair( array[ i ], S() ) );
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << array.size()
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
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
void test_find( int _N, std::string const & message )
{
    _Storage< int, S > av;

    for( int i = 0 ; i < _N ; ++i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::clock_t const start( std::clock() );

    for( int i = 0 ; i < _N ; ++i ){
        av.find( i );
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_increasing( int _N, std::string const & message )
{
    _Storage< int, S > av;

    std::clock_t const start( std::clock() );

    for( int i = 0 ; i < _N ; ++i ){
        av[ i ] = S();
        S s = av[i];
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_decreasing( int _N, std::string const & message )
{
    _Storage< int, S > av;

    std::clock_t const start( std::clock() );

    for( int i = _N ; i > 0 ; --i ){
        av[ i ] = S();
        S s = av[i];
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_random( std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    _Storage< int, S > av;
    
    for( int i = 0 ; i < array.size() ; ++ i ){
        int const value = array[ i ];
        
        av[ value ] = S();
        S s = av[ value ];
    }

    std::clock_t const end( std::clock() );
    
    std::cout
        << message
        << ": "
        << array.size()
        << " times: "
        << std::difftime( end, start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

void push_increasing()
{
    test_push_increasing< AssocVector >( 10000000, "AssocVector.push_increasing" );
    test_push_increasing< Loki::AssocVector >( 10000000, "Loki::AssocVector.push_increasing" );
    test_push_increasing< std::map >( 10000000, "std::map.push_increasing" );

    std::cout << std::endl;
}

void push_decreasing()
{
    test_push_decreasing< AssocVector >( 100000, "AssocVector.push_decreasing" );
    test_push_decreasing< Loki::AssocVector >( 100000, "Loki::AssocVector.push_decreasing" );
    test_push_decreasing< std::map >( 100000, "std::map.push_decreasing" );

    std::cout << std::endl;
}

void push_random()
{
    std::vector< int > array;
    
    for( int i = 0 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_push_random< AssocVector >( array, "AssocVector.push_random" );
    test_push_random< Loki::AssocVector >( array, "Loki::AssocVector.push_random" );
    test_push_random< std::map >( array, "std::map.push_random" );

    std::cout << std::endl;
}

void index_operator_increasing()
{
    test_index_operator_increasing< AssocVector >( 1000000, "AssocVector.index_operator_increasing" );
    test_index_operator_increasing< Loki::AssocVector >( 1000000, "Loki::AssocVector.index_operator_increasing" );
    test_index_operator_increasing< std::map >( 1000000, "std::map.index_operator_increasing" );
    
    std::cout << std::endl;
}

void index_operator_decreasing()
{
    test_index_operator_decreasing< AssocVector >( 100000, "AssocVector.index_operator_decreasing" );
    test_index_operator_decreasing< Loki::AssocVector >( 100000, "Loki::AssocVector.index_operator_decreasing" );
    test_index_operator_decreasing< std::map >( 100000, "std::map.index_operator_decreasing" );
    
    std::cout << std::endl;
}

void index_operator_random()
{
    std::vector< int > array;
    
    for( int i = 0 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() - rand() );
    
    test_index_operator_random< AssocVector >( array, "AssocVector.index_operator_random" );
    test_index_operator_random< Loki::AssocVector >( array, "Loki::AssocVector.index_operator_random" );
    test_index_operator_random< std::map >( array, "std::map.index_operator_random" );
    
    std::cout << std::endl;
}

void find()
{
    test_find< AssocVector >( 10000000, "AssocVector.find" );
    test_find< Loki::AssocVector >( 10000000, "Loki::AssocVector.find" );
    test_find< std::map >( 10000000, "std::map.find" );

    std::cout << std::endl;
}

int main()
{
    push_increasing();
    push_decreasing();
    push_random();

    index_operator_increasing();
    index_operator_decreasing();
    index_operator_random();
    
    find();
}
