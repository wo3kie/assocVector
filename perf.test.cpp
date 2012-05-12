//#define AV_UNIT_TESTS

#define AV_TEST_BOOST_RANDOM

#define AV_BREAK_IF_TIMEOUT( _timeout_ ) \
    { \
        if( ( counter & ( 512 - 1 ) ) == 0 ) \
        { \
            if( std::difftime( std::clock(), start_suite )/((double)CLOCKS_PER_SEC) > ( _timeout_ ) ){ \
                timeout = true; \
            } \
        } \
    }

#ifdef AV_UNIT_TESTS
    unsigned const REPS = 1000;

    unsigned const AV_TIMEOUT = 10;
#else
    #define AV_TEST_VECTOR
    #define AV_TEST_LOKI
    #define AV_TEST_STD_MAP
    #define AV_TEST_BOOST_HASH

    unsigned const REPS = 10000000;

    unsigned const AV_TIMEOUT = 60;
#endif

#ifdef AV_TEST_LOKI
    #include <loki/AssocVector.h>
#endif

#ifdef AV_TEST_STD_MAP
    #include <map>
#endif

#ifdef AV_TEST_BOOST_HASH
    #include <boost/unordered_map.hpp>
#endif

#ifdef AV_TEST_BOOST_RANDOM
    #include <boost/random/mersenne_twister.hpp>
    #include <boost/random/uniform_int_distribution.hpp>
#endif

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "AssocVector.hpp"

int random()
{
#ifdef AV_TEST_BOOST_RANDOM
    static boost::random::mt19937 gen;

    boost::random::uniform_int_distribution<> dist( 1, 1024 * 1024 * 1024 );

    return dist( gen );
#else
    return rand();
#endif
}

unsigned const MessageAlignment = 50;

std::string make_padding( std::string const & message, int length )
{
    int const paddingSize = std::max< int >( 0, length - message.size() );

    return message + std::string( paddingSize, ' ' );
}

struct S
{
    S & operator=( S const & other )
    {
        ++counter;

        return * this;
    }

    static unsigned counter;
};

unsigned S::counter = 0;

void printSummary(
      std::string const & message
    , unsigned tests
    , unsigned rep
    , bool timeout
    , std::clock_t total_time
)
{
    std::cout
        << make_padding( message, MessageAlignment )
        << ": "
        << tests
        << "x"
        <<  rep
        << " times: "
        << ( timeout ? 999.0 : total_time/((double)CLOCKS_PER_SEC) )
        << "s\n";
}

template< typename _Storage >
void test_push_increasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av.insert( std::make_pair( counter, S() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_push_decreasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = rep ; ! timeout && counter > 0 ; --counter ){
            av.insert( std::make_pair( counter, S() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_push_decreasing_push_back_reverse( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av.push_back( std::make_pair( counter, S() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        std::reverse( av.begin(), av.end() );

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_push_random_push_back_sort( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++counter ){
            av.push_back( std::make_pair( array[ counter ], S() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        std::sort(
              av.begin()
            , av.end()
            , util::CmpByFirst< std::pair< int, S >, std::less< int > >()
        );

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}

template< typename _Storage >
void test_push_random( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++counter ){
            av.insert( std::make_pair( array[ counter ], S() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}

template< typename _Storage >
void test_erase_increasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        for( unsigned i = 0 ; i < rep ; ++i ){
            av.insert( std::make_pair( i, S() ) );
        }

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av.erase( counter );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_erase_decreasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        for( unsigned i = 0 ; i < rep ; ++i ){
            av.insert( std::make_pair( i, S() ) );
        }

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = rep ; ! timeout && counter > 0 ; --counter ){
            av.erase( counter );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_erase_random( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        for( unsigned i = 0 ; i < array.size() ; ++i ){
            av.insert( std::make_pair( array[ i ], S() ) );
        }

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++counter ){
            av.erase( array[ counter ] );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}

#ifdef AV_TEST_LOKI
    template< typename T, typename K >
    void merge( Loki::AssocVector< T, K > & ){}
#endif

#ifdef AV_TEST_STD_MAP
    template< typename T, typename K >
    void merge( std::map< int, S > & ){}
#endif

#ifdef AV_TEST_BOOST_HASH
    template< typename T, typename K >
    void merge( boost::unordered_map< int, S > & ){}
#endif

template< typename T, typename K >
void merge( AssocVector< T, K > & av ){ av.merge(); }

template< typename _Storage >
void test_find( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    _Storage av;

    for( unsigned i = 0 ; i < rep ; ++i ){
        av.insert( std::make_pair( i, S() ) );
    }

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av.find( counter );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_index_operator_increasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av[ counter ] = S();
            S s = av[counter];

            (void)s;

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_index_operator_decreasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = rep ; ! timeout && counter > 0 ; --counter ){
            av[ counter ] = S();
            S s = av[counter];

            (void)s;

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_index_operator_random( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++ counter ){
            int const value = array[ counter ];

            av[ value ] = S();
            S s = av[ value ];

            (void)s;

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}

template< typename _Storage >
void test_random_operations(
      unsigned tests
    , std::vector< std::pair< int, int > > const & array
    , std::string const & message
)
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++ counter ){
            int const operation = array[ counter ].first;
            int const value = array[ counter ].second;

            switch( operation )
            {
                case 0:
                    av.insert( std::make_pair( value, S() ) );
                    break;

                case 1:
                    av.find( value );
                    break;

                case 2:
                    av.erase( value );
                    break;

                default:
                    assert( false );
            }

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}

void push_increasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_push_increasing< AssocVector< int, S > >( REPS / i, i, "push_increasing.AssocVector" );

#ifdef AV_TEST_LOKI
        test_push_increasing< Loki::AssocVector< int, S > >( REPS / i, i, "push_increasing.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_push_increasing< std::map< int, S > >( REPS / i, i, "push_increasing.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_push_increasing< boost::unordered_map< int, S > >( REPS / i, i, "push_increasing.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void push_decreasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_push_decreasing< AssocVector< int, S > >( REPS / i, i, "push_decreasing.AssocVector" );

#ifdef AV_TEST_VECTOR
        test_push_decreasing_push_back_reverse< std::vector< std::pair< int, S > > >( REPS / i, i, "  push_decreasing.std::vector.push_back.reverse" );
#endif

#ifdef AV_TEST_LOKI
        test_push_decreasing< Loki::AssocVector< int, S > >( REPS / i, i, "push_decreasing.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_push_decreasing< std::map< int, S > >( REPS / i, i, "push_decreasing.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_push_decreasing< boost::unordered_map< int, S > >( REPS / i, i, "push_decreasing.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void push_random()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< int > array;

        for( unsigned j = 0 ; j < i ; ++ j )
            array.push_back( rand() + rand() - rand() );

        test_push_random< AssocVector< int, S > >( REPS / i, array, "push_random.AssocVector" );

#ifdef AV_TEST_VECTOR
        test_push_random_push_back_sort< std::vector< std::pair< int, S > > >( REPS / i, array, "  push_random.std::vector.push_back.sort" );
#endif

#ifdef AV_TEST_LOKI
        test_push_random< Loki::AssocVector< int, S > >( REPS / i, array, "push_random.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_push_random< std::map< int, S > >( REPS / i, array, "push_random.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_push_random< boost::unordered_map< int, S > >( REPS / i, array, "push_random.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void index_operator_increasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_index_operator_increasing< AssocVector< int, S > >( REPS / i, i, "index_operator_increasing.AssocVector" );

#ifdef AV_TEST_LOKI
        test_index_operator_increasing< Loki::AssocVector< int, S > >( REPS / i, i, "index_operator_increasing.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_index_operator_increasing< std::map< int, S > >( REPS / i, i, "index_operator_increasing.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_index_operator_increasing< boost::unordered_map< int, S > >( REPS / i, i, "index_operator_increasing.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void index_operator_decreasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_index_operator_decreasing< AssocVector< int, S > >( REPS / i, i, "index_operator_decreasing.AssocVector" );

#ifdef AV_TEST_LOKI
        test_index_operator_decreasing< Loki::AssocVector< int, S > >( REPS / i, i, "index_operator_decreasing.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_index_operator_decreasing< std::map< int, S > >( REPS / i, i, "index_operator_decreasing.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_index_operator_decreasing< boost::unordered_map< int, S > >( REPS / i, i, "index_operator_decreasing.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void index_operator_random()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< int > array;

        for( unsigned j = 0 ; j < i ; ++ j )
            array.push_back( rand() + rand() - rand() );

        test_index_operator_random< AssocVector< int, S > >( REPS / i, array, "index_operator_random.AssocVector" );

#ifdef AV_TEST_LOKI
        test_index_operator_random< Loki::AssocVector< int, S > >( REPS / i, array, "index_operator_random.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_index_operator_random< std::map< int, S > >( REPS / i, array, "index_operator_random.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_index_operator_random< boost::unordered_map< int, S > >( REPS / i, array, "index_operator_random.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void find()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_find< AssocVector< int, S > >( REPS / i, i, "find.AssocVector" );

#ifdef AV_TEST_LOKI
    test_find< Loki::AssocVector< int, S > >( REPS / i, i, "find.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
    test_find< std::map< int, S > >( REPS / i, i, "find.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
    test_find< boost::unordered_map< int, S > >( REPS / i, i, "find.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void erase_increasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_erase_increasing< AssocVector< int, S > >( REPS / i, i, "erase_increasing.AssocVector" );

#ifdef AV_TEST_LOKI
        test_erase_increasing< Loki::AssocVector< int, S > >( REPS / i, i, "erase_increasing.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_erase_increasing< std::map< int, S > >( REPS / i, i, "erase_increasing.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_erase_increasing< boost::unordered_map< int, S > >( REPS / i, i, "erase_increasing.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void erase_decreasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_erase_decreasing< AssocVector< int, S > >( REPS / i, i, "erase_decreasing.AssocVector" );

#ifdef AV_TEST_LOKI
        test_erase_decreasing< Loki::AssocVector< int, S > >( REPS / i, i, "erase_decreasing.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_erase_decreasing< std::map< int, S > >( REPS / i, i, "erase_decreasing.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_erase_decreasing< boost::unordered_map< int, S > >( REPS / i, i, "erase_decreasing.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void erase_random()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< int > array;

        for( unsigned j = 0 ; j < i ; ++ j )
            array.push_back( rand() + rand() - rand() );

        test_erase_random< AssocVector< int, S > >( REPS / i, array, "erase_random.AssocVector" );

#ifdef AV_TEST_LOKI
        test_erase_random< Loki::AssocVector< int, S > >( REPS / i, array, "erase_random.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_erase_random< std::map< int, S > >( REPS / i, array, "erase_random.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_erase_random< boost::unordered_map< int, S > >( REPS / i, array, "erase_random.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

void random_operations()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< std::pair< int, int > > array;

        for( unsigned j = 0 ; j < i/2 ; ++ j )
            array.push_back( std::make_pair( 0, rand() + rand() - rand() ) );

        for( unsigned j = i/2 ; j < i ; ++ j )
            array.push_back( std::make_pair( rand() % 3, rand() + rand() - rand() ) );

        test_random_operations< AssocVector< int, S > >( REPS / i, array, "test_random_operations.AssocVector" );

#ifdef AV_TEST_LOKI
        test_random_operations< Loki::AssocVector< int, S > >( REPS / i, array, "test_random_operations.Loki::AssocVector" );
#endif

#ifdef AV_TEST_STD_MAP
        test_random_operations< std::map< int, S > >( REPS / i, array, "test_random_operations.std::map" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_random_operations< boost::unordered_map< int, S > >( REPS / i, array, "test_random_operations.boost::unordered_map" );
#endif

        std::cout << std::endl;
    }
}

std::string getTestMode()
{
    #ifdef AV_UNIT_TESTS
        return "Unit Tests";
    #else
        return "Perf Tests";
    #endif
}

int main()
{
    {
        std::cout
            << std::string( 10, '*' )
            << ' '
            << getTestMode()
            << ' '
            << std::string( 10, '*' )
            << "\n"
            << std::endl;
    }

    push_increasing();
    push_decreasing();
    push_random();

    index_operator_increasing();
    index_operator_decreasing();
    index_operator_random();

    find();

    erase_increasing();
    erase_decreasing();
    erase_random();

    random_operations();
}
