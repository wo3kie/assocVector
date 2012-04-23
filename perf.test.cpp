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
void test_push_front( int _N, std::string const & message )
{
    std::clock_t start( std::clock() );

    _Storage< int, S > av;

    for( int i = _N ; i > 0 ; --i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( std::clock(), start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_push_back( int _N, std::string const & message )
{
    std::clock_t start( std::clock() );

    _Storage< int, S > av;

    for( int i = 0 ; i < _N ; ++i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( std::clock(), start )/((double)CLOCKS_PER_SEC)
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

    std::clock_t start( std::clock() );

    for( int i = 0 ; i < _N ; ++i ){
        av.find( i );
    }

    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( std::clock(), start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_increasing( int _N, std::string const & message )
{
    _Storage< int, S > av;

    std::clock_t start( std::clock() );

    for( int i = 0 ; i < _N ; ++i ){
        av[ i ] = S();
        S s = av[i];
    }

    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( std::clock(), start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

template< template< typename, typename > class _Storage >
void test_index_operator_decreasing( int _N, std::string const & message )
{
    _Storage< int, S > av;

    std::clock_t start( std::clock() );

    for( int i = _N ; i > 0 ; --i ){
        av[ i ] = S();
        S s = av[i];
    }

    std::cout
        << message
        << ": "
        << _N
        << " times: "
        << std::difftime( std::clock(), start )/((double)CLOCKS_PER_SEC)
        << "s, "
        << std::endl;
}

void push_front()
{
    test_push_front< AssocVector >( 100000, "AssocVector.push_front" );
    test_push_front< Loki::AssocVector >( 100000, "Loki::AssocVector.push_front" );
    test_push_front< std::map >( 100000, "std::map.push_front" );

    std::cout << std::endl;
}

void push_back()
{
    test_push_back< AssocVector >( 10000000, "AssocVector.push_back" );
    test_push_back< Loki::AssocVector >( 10000000, "Loki::AssocVector.push_back" );
    test_push_back< std::map >( 10000000, "std::map.push_back" );

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

void find()
{
    test_find< AssocVector >( 10000000, "AssocVector.find" );
    test_find< Loki::AssocVector >( 10000000, "Loki::AssocVector.find" );
    test_find< std::map >( 10000000, "std::map.find" );

    std::cout << std::endl;
}

int main()
{
    push_front();
    push_back();

    index_operator_increasing();
    index_operator_decreasing();
    
    find();
}
