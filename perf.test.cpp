/*
 * Author: Lukasz Czerwinski
 * GitHub: https://github.com/wo3kie/AssocVector
 * Website: http://www.lukaszczerwinski.pl/assoc_vector.en.html
 */

/*
 * AssocVector is developed under the BSD license. It is allowed to
 * distribute source, use it and modify without any limitations either
 * for personal or commercial goal. Whilst distributing source code
 * using AssocVector it is obligate to mark the author
 */

 /*
 * There is absolutely no guarantee that AssocVector will work without
 * any errors and that is free of any bugs. AssocVector author is not
 * responsible for any damages, mind and financial loses related with
 * AssocVector using.
 */

 //#define AV_UNIT_TESTS

// configuration.begin

#ifdef AV_UNIT_TESTS
    #define AV_TEST_EXTENSIONS

    unsigned const REPS = 1000;

    unsigned const AV_TIMEOUT = 10;
#else
    #define AV_TEST_EXTENSIONS
    //#define AV_TEST_VECTOR
    #define AV_TEST_LOKI
    //#define AV_TEST_STD_MAP
    //#define AV_TEST_BOOST_HASH

    //#define AV_TEST_BOOST_RANDOM

    unsigned const REPS = 1000000;

    unsigned const AV_TIMEOUT = 60;
#endif

// configuration.end

// includes.begin

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

#define AV_ENABLE_EXTENSIONS
#include "AssocVector.hpp"

// includes.end

#define AV_BREAK_IF_TIMEOUT( _timeout_ ) \
    { \
        if( ( counter & ( 512 - 1 ) ) == 0 ) \
        { \
            if( std::difftime( std::clock(), start_suite )/((double)CLOCKS_PER_SEC) > ( _timeout_ ) ){ \
                timeout = true; \
            } \
        } \
    }

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

unsigned const MessageAlignment = 60;

std::string make_padding( std::string const & message, int length )
{
    int const paddingSize = std::max< int >( 0, length - message.size() );

    return message + std::string( paddingSize, ' ' );
}

template< typename _T >
std::string name(){ return "?"; }

struct S1
{
    S1 & operator=( S1 const & other )
    {
        ++counter;

        return * this;
    }

    static unsigned counter;
};

unsigned S1::counter = 0;

template<>
std::string name< S1 >(){ return "S1"; }

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

    bool b;
    short s;
    int i;
    unsigned u;
    char c;
    float f;
    double d;
    void * v;
};

template<>
std::string name< S2 >(){ return "S2"; }

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

    void swap( S3 & other )
    {
        array.swap( other.array );
    }

    std::vector< int > array;

    static int createdObjects;
    static int destroyedObjects;

    static int copies;
    static int moves;
};

int S3::createdObjects = 0;

int S3::destroyedObjects = 0;

int S3::copies = 0;

int S3::moves = 0;

template<>
std::string name< S3 >(){ return "S3"; }

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
        << "- "
        << tests
        << "-"
        << rep
        << " - "
        << ( timeout ? 999.0 : total_time/((double)CLOCKS_PER_SEC) )
        << '\n';
}

template< typename _Storage >
void test_insert_increasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av.insert( std::make_pair( counter, typename _Storage::mapped_type() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test__insert_increasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av._insert( std::make_pair( counter, typename _Storage::mapped_type() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_insert_decreasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = rep ; ! timeout && counter > 0 ; --counter ){
            av.insert( std::make_pair( counter, typename _Storage::mapped_type() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test__insert_decreasing( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = rep ; ! timeout && counter > 0 ; --counter ){
            av._insert( std::make_pair( counter, typename _Storage::mapped_type() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}


template< typename _Storage >
void test_insert_decreasing_push_back_reverse( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av.push_back( std::make_pair( counter, typename _Storage::value_type::second_type() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        std::reverse( av.begin(), av.end() );

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, rep, timeout, total_time );
}

template< typename _Storage >
void test_insert_random_push_back_sort( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++counter ){
            av.push_back(
                std::make_pair(
                      array[ counter ]
                    , typename _Storage::value_type::second_type()
                )
            );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        std::sort(
              av.begin()
            , av.end()
            , util::CmpByFirst<
                  std::pair< int, typename _Storage::value_type::second_type() >
                , std::less< int >
            >()
        );

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}

template< typename _Storage >
void test_insert_random( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++counter ){
            av.insert( std::make_pair( array[ counter ], typename _Storage::mapped_type() ) );

            AV_BREAK_IF_TIMEOUT( AV_TIMEOUT );
        }

        total_time += ( std::clock() - start_test );
    }

    printSummary( message, tests, array.size(), timeout, total_time );
}


template< typename _Storage >
void test__insert_random( unsigned tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        _Storage av;

        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < array.size() ; ++counter ){
            av._insert( std::make_pair( array[ counter ], typename _Storage::mapped_type() ) );

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
            av.insert( std::make_pair( i, typename _Storage::mapped_type() ) );
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
            av.insert( std::make_pair( i, typename _Storage::mapped_type() ) );
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
            av.insert( std::make_pair( array[ i ], typename _Storage::mapped_type() ) );
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

template< typename _Storage >
void test_find( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    _Storage av;

    for( unsigned i = 0 ; i < rep ; ++i ){
        av.insert( std::make_pair( i, typename _Storage::mapped_type() ) );
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
void test__find( unsigned tests, unsigned rep, std::string const & message )
{
    std::clock_t const start_suite( std::clock() );

    bool timeout = false;

    _Storage av;

    for( unsigned i = 0 ; i < rep ; ++i ){
        av.insert( std::make_pair( i, typename _Storage::mapped_type() ) );
    }

    std::clock_t total_time = 0;

    for( unsigned j = 0 ; ! timeout && j < tests ; ++ j )
    {
        std::clock_t const start_test( std::clock() );

        for( unsigned counter = 0 ; ! timeout && counter < rep ; ++counter ){
            av._find( counter );

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
            av[ counter ] = typename _Storage::mapped_type();
            typename _Storage::mapped_type s = av[counter];

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
            av[ counter ] = typename _Storage::mapped_type();
            typename _Storage::mapped_type s = av[counter];

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

            av[ value ] = typename _Storage::mapped_type();
            typename _Storage::mapped_type s = av[ value ];

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
                    av.insert( std::make_pair( value, typename _Storage::mapped_type() ) );
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


template< typename _T >
void insert_increasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_insert_increasing< AssocVector< int, _T > >( REPS / i, i, "insert_increasing.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_EXTENSIONS
        test__insert_increasing< AssocVector< int, _T > >( REPS / i, i, "    _insert_increasing.AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_LOKI
        test_insert_increasing< Loki::AssocVector< int, _T > >( REPS / i, i, "insert_increasing.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_insert_increasing< std::map< int, _T > >( REPS / i, i, "insert_increasing.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_insert_increasing< boost::unordered_map< int, _T > >( REPS / i, i, "insert_increasing.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void insert_decreasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_insert_decreasing< AssocVector< int, _T > >( REPS / i, i, "insert_decreasing.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_EXTENSIONS
        test__insert_decreasing< AssocVector< int, _T > >( REPS / i, i, "    _insert_decreasing.AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_VECTOR
        test_insert_decreasing_push_back_reverse< std::vector< std::pair< int, _T > > >( REPS / i, i, "    insert_decreasing.std::vector< int, _T >.push_back.reverse" );
#endif

#ifdef AV_TEST_LOKI
        test_insert_decreasing< Loki::AssocVector< int, _T > >( REPS / i, i, "insert_decreasing.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_insert_decreasing< std::map< int, _T > >( REPS / i, i, "insert_decreasing.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_insert_decreasing< boost::unordered_map< int, _T > >( REPS / i, i, "insert_decreasing.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void insert_random()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< int > array;

        for( unsigned j = 0 ; j < i ; ++ j )
            array.push_back( rand() + rand() - rand() );

        test_insert_random< AssocVector< int, _T > >( REPS / i, array, "insert_random.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_EXTENSIONS
        test__insert_random< AssocVector< int, _T > >( REPS / i, array, "    _insert_random.AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_VECTOR
        //test_insert_random_push_back_sort< std::vector< std::pair< int, _T > > >( REPS / i, array, "    insert_random.std::vector< int, _T >.push_back.sort" );
#endif

#ifdef AV_TEST_LOKI
        test_insert_random< Loki::AssocVector< int, _T > >( REPS / i, array, "insert_random.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_insert_random< std::map< int, _T > >( REPS / i, array, "insert_random.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_insert_random< boost::unordered_map< int, _T > >( REPS / i, array, "insert_random.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void index_operator_increasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_index_operator_increasing< AssocVector< int, _T > >( REPS / i, i, "index_operator_increasing.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_index_operator_increasing< Loki::AssocVector< int, _T > >( REPS / i, i, "index_operator_increasing.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_index_operator_increasing< std::map< int, _T > >( REPS / i, i, "index_operator_increasing.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_index_operator_increasing< boost::unordered_map< int, _T > >( REPS / i, i, "index_operator_increasing.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void index_operator_decreasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_index_operator_decreasing< AssocVector< int, _T > >( REPS / i, i, "index_operator_decreasing.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_index_operator_decreasing< Loki::AssocVector< int, _T > >( REPS / i, i, "index_operator_decreasing.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_index_operator_decreasing< std::map< int, _T > >( REPS / i, i, "index_operator_decreasing.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_index_operator_decreasing< boost::unordered_map< int, _T > >( REPS / i, i, "index_operator_decreasing.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void index_operator_random()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< int > array;

        for( unsigned j = 0 ; j < i ; ++ j )
            array.push_back( rand() + rand() - rand() );

        test_index_operator_random< AssocVector< int, _T > >( REPS / i, array, "index_operator_random.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_index_operator_random< Loki::AssocVector< int, _T > >( REPS / i, array, "index_operator_random.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_index_operator_random< std::map< int, _T > >( REPS / i, array, "index_operator_random.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_index_operator_random< boost::unordered_map< int, _T > >( REPS / i, array, "index_operator_random.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void find()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_find< AssocVector< int, _T > >( REPS / i, i, "find.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_EXTENSIONS
        test__find< AssocVector< int, _T > >( REPS / i, i, "    _find.AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_LOKI
    test_find< Loki::AssocVector< int, _T > >( REPS / i, i, "find.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
    test_find< std::map< int, _T > >( REPS / i, i, "find.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
    test_find< boost::unordered_map< int, _T > >( REPS / i, i, "find.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void erase_increasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_erase_increasing< AssocVector< int, _T > >( REPS / i, i, "erase_increasing.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_erase_increasing< Loki::AssocVector< int, _T > >( REPS / i, i, "erase_increasing.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_erase_increasing< std::map< int, _T > >( REPS / i, i, "erase_increasing.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_erase_increasing< boost::unordered_map< int, _T > >( REPS / i, i, "erase_increasing.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void erase_decreasing()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        test_erase_decreasing< AssocVector< int, _T > >( REPS / i, i, "erase_decreasing.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_erase_decreasing< Loki::AssocVector< int, _T > >( REPS / i, i, "erase_decreasing.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_erase_decreasing< std::map< int, _T > >( REPS / i, i, "erase_decreasing.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_erase_decreasing< boost::unordered_map< int, _T > >( REPS / i, i, "erase_decreasing.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void erase_random()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< int > array;

        for( unsigned j = 0 ; j < i ; ++ j )
            array.push_back( rand() + rand() - rand() );

        test_erase_random< AssocVector< int, _T > >( REPS / i, array, "erase_random.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_erase_random< Loki::AssocVector< int, _T > >( REPS / i, array, "erase_random.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_erase_random< std::map< int, _T > >( REPS / i, array, "erase_random.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_erase_random< boost::unordered_map< int, _T > >( REPS / i, array, "erase_random.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

template< typename _T >
void random_operations()
{
    for( unsigned i = 100 ; i <= REPS ; i *= 10 )
    {
        std::vector< std::pair< int, int > > array;

        for( unsigned j = 0 ; j < i/2 ; ++ j )
            array.push_back( std::make_pair( 0, rand() + rand() - rand() ) );

        for( unsigned j = i/2 ; j < i ; ++ j )
            array.push_back( std::make_pair( rand() % 3, rand() + rand() - rand() ) );

        test_random_operations< AssocVector< int, _T > >( REPS / i, array, "test_random_operations.AssocVector< int, " + name< _T >() + " >" );

#ifdef AV_TEST_LOKI
        test_random_operations< Loki::AssocVector< int, _T > >( REPS / i, array, "test_random_operations.Loki::AssocVector< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_STD_MAP
        test_random_operations< std::map< int, _T > >( REPS / i, array, "test_random_operations.std::map< int, " + name< _T >() + " >" );
#endif

#ifdef AV_TEST_BOOST_HASH
        test_random_operations< boost::unordered_map< int, _T > >( REPS / i, array, "test_random_operations.boost::map< int, " + name< _T >() + " >" );
#endif

        std::cout << std::endl;
    }
}

std::string getTestMode()
{
    std::string mode;

    #ifdef AV_UNIT_TESTS
        mode = "Unit Tests";
    #else
        mode = "Perf Tests";
    #endif

    #ifdef AV_CXX11X_RVALUE_REFERENCE
        mode += " C++11";
    #endif

    return mode;
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

    insert_increasing< S1 >();
    insert_increasing< S2 >();
    insert_increasing< S3 >();

    insert_decreasing< S1 >();
    insert_decreasing< S2 >();
    insert_decreasing< S3 >();

    insert_random< S1 >();
    insert_random< S2 >();
    insert_random< S3 >();

    index_operator_increasing< S1 >();
    index_operator_increasing< S2 >();
    index_operator_increasing< S3 >();

    index_operator_decreasing< S1 >();
    index_operator_decreasing< S2 >();
    index_operator_decreasing< S3 >();

    index_operator_random< S1 >();
    index_operator_random< S2 >();
    index_operator_random< S3 >();

    find< S1 >();
    find< S2 >();
    find< S3 >();

    erase_increasing< S1 >();
    erase_increasing< S2 >();
    erase_increasing< S3 >();

    erase_decreasing< S1 >();
    erase_decreasing< S2 >();
    erase_decreasing< S3 >();

    erase_random< S1 >();
    erase_random< S2 >();
    erase_random< S3 >();

    random_operations< S1 >();
    random_operations< S2 >();
    random_operations< S3 >();

    std::cout << "OK" << std::endl;

    return 0;
}
