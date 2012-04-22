#include <ctime>
#include <iostream>

#include <loki/AssocVector.h>

#include "AssocVector.hpp"

struct S{
    S & operator=( S const & other ){
        ++counter;
    }

    static int counter;
};

int S::counter = 0;

template<
      template< typename, typename > class _Storage
    , int _N
>
void test_push_front( std::string const & message )
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

template<
      template< typename, typename > class _Storage
    , int _N
>
void test_push_back( std::string const & message )
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
void merge( AssocVector< T, K > & av ){ av.merge(); }

template<
      template< typename, typename > class _Storage
    , int _N
>
void test_find( std::string const & message )
{
    _Storage< int, S > av;
    
    for( int i = 0 ; i < _N ; ++i ){
        av.insert( std::make_pair( i, S() ) );
    }

    merge( av );
    
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

int main()
{
    test_push_front< AssocVector, 1000 >( "AssocVector.push_front" );
    test_push_front< Loki::AssocVector, 1000 >( "Loki::AssocVector.push_front" );
    
    test_push_front< AssocVector, 10000 >( "AssocVector.push_front" );
    test_push_front< Loki::AssocVector, 10000 >( "Loki::AssocVector.push_front" );

    test_push_front< AssocVector, 100000 >( "AssocVector.push_front" );
    test_push_front< Loki::AssocVector, 100000 >( "Loki::AssocVector.push_front" );
    
    std::cout << std::endl;
    
    test_push_back< AssocVector, 100000 >( "AssocVector.push_back" );
    test_push_back< Loki::AssocVector, 100000 >( "Loki::AssocVector.push_back" );
    
    test_push_back< AssocVector, 1000000 >( "AssocVector.push_back" );
    test_push_back< Loki::AssocVector, 1000000 >( "Loki::AssocVector.push_back" );

    test_push_back< AssocVector, 2000000 >( "AssocVector.push_back" );
    test_push_back< Loki::AssocVector, 2000000 >( "Loki::AssocVector.push_back" );
    
    std::cout << std::endl;
    
    test_find< AssocVector, 100000 >( "AssocVector.find" );
    test_find< Loki::AssocVector, 100000 >( "Loki::AssocVector.find" );
    
    test_find< AssocVector, 1000000 >( "AssocVector.find" );
    test_find< Loki::AssocVector, 1000000 >( "Loki::AssocVector.find" );

    test_find< AssocVector, 2000000 >( "AssocVector.find" );
    test_find< Loki::AssocVector, 2000000 >( "Loki::AssocVector.find" );
    
}
