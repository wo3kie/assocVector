//#define AV_UNIT_TESTS

#ifdef AV_UNIT_TESTS
    unsigned const REPS = 1;
#else
    #define AV_USE_VECTOR
    #define AV_USE_LOKI
    #define AV_USE_STD_MAP

    unsigned const REPS = 10;
#endif

#ifdef AV_USE_LOKI
    #include <loki/AssocVector.h>
#endif
#ifdef AV_USE_STD_MAP
    #include <map>
#endif

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#include "AssocVector.hpp"

struct S
{
    S & operator=( S const & other )
    {
        ++counter;

        return * this;
    }

    static int counter;
};

int S::counter = 0;

template< typename _Storage >
void test_push_increasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_push_decreasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_push_decreasing_push_back_reverse( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

        for( int i = 0 ; i < rep ; ++i ){
            av.push_back( std::make_pair( i, S() ) );
        }

        std::reverse( av.begin(), av.end() );
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

template< typename _Storage >
void test_push_random_push_back_sort( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

        for( int i = 0 ; i < array.size() ; ++i ){
            av.push_back( std::make_pair( array[ i ], S() ) );
        }

        std::sort(
              av.begin()
            , av.end()
            , util::CmpByFirst< std::pair< int, S >, std::less< int > >()
        );
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

template< typename _Storage >
void test_push_random( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_erase_increasing( int tests, int rep, std::string const & message )
{
    std::clock_t total_time = 0;

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_erase_decreasing( int tests, int rep, std::string const & message )
{
    std::clock_t total_time = 0;

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_erase_random( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t total_time = 0;

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

#ifdef AV_USE_LOKI
    template< typename T, typename K >
    void merge( Loki::AssocVector< T, K > & ){}
#endif

#ifdef AV_USE_STD_MAP
    template< typename T, typename K >
    void merge( std::map< int, S > & ){}
#endif

template< typename T, typename K >
void merge( AssocVector< T, K > & av ){ av.merge(); }

template< typename _Storage >
void test_find( int tests, int rep, std::string const & message )
{
    _Storage av;

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

template< typename _Storage >
void test_index_operator_increasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_index_operator_decreasing( int tests, int rep, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_index_operator_random( int tests, std::vector< int > const & array, std::string const & message )
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

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

template< typename _Storage >
void test_random_operations(
      int tests
    , std::vector< std::pair< int, int > > const & array
    , std::string const & message
)
{
    std::clock_t const start( std::clock() );

    for( int j = 0 ; j < tests ; ++ j )
    {
        _Storage av;

        for( int i = 0 ; i < array.size() ; ++ i ){
            int const operation = array[ i ].first;
            int const value = array[ i ].second;
            
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
    test_push_increasing< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.push_increasing" );
#ifdef AV_USE_LOKI
    test_push_increasing< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.push_increasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_push_increasing< std::map< int, S > >( 100 * REPS, 1000, "std::map.push_increasing" );
#endif

    std::cout << std::endl;

    test_push_increasing< AssocVector< int, S > >( REPS, 1000000, "AssocVector.push_increasing" );
#ifdef AV_USE_LOKI
    test_push_increasing< Loki::AssocVector< int, S > >( REPS, 1000000, "Loki::AssocVector.push_increasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_push_increasing< std::map< int, S > >( REPS, 1000000, "std::map.push_increasing" );
#endif

    std::cout << std::endl;
}

void push_decreasing()
{
    test_push_decreasing< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.push_decreasing" );
#ifdef AV_USE_VECTOR
    test_push_decreasing_push_back_reverse< std::vector< std::pair< int, S > > >( 100 * REPS, 1000, "  std::vector.push_decreasing.push_back.reverse" );
#endif
#ifdef AV_USE_LOKI
    test_push_decreasing< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.push_decreasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_push_decreasing< std::map< int, S > >( 100 * REPS, 1000, "std::map.push_decreasing" );
#endif

    std::cout << std::endl;

    test_push_decreasing< AssocVector< int, S > >( REPS, 1000000, "AssocVector.push_decreasing" );
#ifdef AV_USE_VECTOR
    test_push_decreasing_push_back_reverse< std::vector< std::pair< int, S > > >( REPS, 1000000, "  std::vector.push_decreasing.push_back.reverse" );
#endif
#ifdef AV_USE_LOKI
    std::cout << "Loki::AssocVector.push_decreasing: " << REPS <<"x1000000 times: -s," << std::endl;
#endif
#ifdef AV_USE_STD_MAP
    test_push_decreasing< std::map< int, S > >( REPS, 1000000, "std::map.push_decreasing" );
#endif

    std::cout << std::endl;
}

void push_random()
{
    std::vector< int > array;

    for( int i = 0 ; i < 1000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_push_random< AssocVector< int, S > >( 100 * REPS, array, "AssocVector.push_random" );
#ifdef AV_USE_VECTOR
    test_push_random_push_back_sort< std::vector< std::pair< int, S > > >( 100 * REPS, array, "  std::vector.push_random.push_back.sort" );
#endif
#ifdef AV_USE_LOKI
    test_push_random< Loki::AssocVector< int, S > >( 100 * REPS, array, "Loki::AssocVector.push_random" );
#endif
#ifdef AV_USE_STD_MAP
    test_push_random< std::map< int, S > >( 100 * REPS, array, "std::map.push_random" );
#endif

    std::cout << std::endl;

    for( int i = 1000 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() + rand() - rand() );

    test_push_random< AssocVector< int, S > >( REPS, array, "AssocVector.push_random" );
#ifdef AV_USE_VECTOR
    test_push_random_push_back_sort< std::vector< std::pair< int, S > > >( REPS, array, "  std::vector.push_random.push_back.sort" );
#endif
#ifdef AV_USE_LOKI
    test_push_random< Loki::AssocVector< int, S > >( REPS, array, "Loki::AssocVector.push_random" );
#endif
#ifdef AV_USE_STD_MAP
    test_push_random< std::map< int, S > >( REPS, array, "std::map.push_random" );
#endif

    std::cout << std::endl;
}

void index_operator_increasing()
{
    test_index_operator_increasing< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.index_operator_increasing" );
#ifdef AV_USE_LOKI
    test_index_operator_increasing< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.index_operator_increasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_index_operator_increasing< std::map< int, S > >( 100 * REPS, 1000, "std::map.index_operator_increasing" );
#endif

    std::cout << std::endl;

    test_index_operator_increasing< AssocVector< int, S > >( REPS, 1000000, "AssocVector.index_operator_increasing" );
#ifdef AV_USE_LOKI
    test_index_operator_increasing< Loki::AssocVector< int, S > >( REPS, 1000000, "Loki::AssocVector.index_operator_increasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_index_operator_increasing< std::map< int, S > >( REPS, 1000000, "std::map.index_operator_increasing" );
#endif

    std::cout << std::endl;
}

void index_operator_decreasing()
{
    test_index_operator_decreasing< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.index_operator_decreasing" );
#ifdef AV_USE_LOKI
    test_index_operator_decreasing< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.index_operator_decreasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_index_operator_decreasing< std::map< int, S > >( 100 * REPS, 1000, "std::map.index_operator_decreasing" );
#endif

    std::cout << std::endl;

    test_index_operator_decreasing< AssocVector< int, S > >( REPS, 1000000, "AssocVector.index_operator_decreasing" );
#ifdef AV_USE_LOKI
    std::cout << "Loki::AssocVector.index_operator_decreasing: " << REPS << "x1000000 times: -s," << std::endl;
#endif
#ifdef AV_USE_STD_MAP
    test_index_operator_decreasing< std::map< int, S > >( REPS, 1000000, "std::map.index_operator_decreasing" );
#endif

    std::cout << std::endl;
}

void index_operator_random()
{
    std::vector< int > array;

    for( int i = 0 ; i < 1000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_index_operator_random< AssocVector< int, S > >( 100 * REPS, array, "AssocVector.index_operator_random" );
#ifdef AV_USE_LOKI
    test_index_operator_random< Loki::AssocVector< int, S > >( 100 * REPS, array, "Loki::AssocVector.index_operator_random" );
#endif
#ifdef AV_USE_STD_MAP
    test_index_operator_random< std::map< int, S > >( 100 * REPS, array, "std::map.index_operator_random" );
#endif

    std::cout << std::endl;

    for( int i = 1000 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() + rand() - rand() );

    test_index_operator_random< AssocVector< int, S > >( REPS, array, "AssocVector.index_operator_random" );
#ifdef AV_USE_LOKI
    test_index_operator_random< Loki::AssocVector< int, S > >( REPS, array, "Loki::AssocVector.index_operator_random" );
#endif
#ifdef AV_USE_STD_MAP
    test_index_operator_random< std::map< int, S > >( REPS, array, "std::map.index_operator_random" );
#endif

    std::cout << std::endl;
}

void find()
{
    test_find< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.find" );
#ifdef AV_USE_LOKI
    test_find< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.find" );
#endif
#ifdef AV_USE_STD_MAP
    test_find< std::map< int, S > >( 100 * REPS, 1000, "std::map.find" );
#endif

    std::cout << std::endl;

    test_find< AssocVector< int, S > >( REPS, 1000000, "AssocVector.find" );
#ifdef AV_USE_LOKI
    test_find< Loki::AssocVector< int, S > >( REPS, 1000000, "Loki::AssocVector.find" );
#endif
#ifdef AV_USE_STD_MAP
    test_find< std::map< int, S > >( REPS, 1000000, "std::map.find" );
#endif

    std::cout << std::endl;
}

void erase_increasing()
{
    test_erase_increasing< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.erase_increasing" );
#ifdef AV_USE_LOKI
    test_erase_increasing< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.erase_increasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_erase_increasing< std::map< int, S > >( 100 * REPS, 1000, "std::map.erase_increasing" );
#endif

    std::cout << std::endl;

    test_erase_increasing< AssocVector< int, S > >( REPS, 1000000, "AssocVector.erase_increasing" );
#ifdef AV_USE_LOKI
    std::cout << "Loki::AssocVector.erase_increasing: " << REPS << "x1000000 times: -s," << std::endl;
#endif
#ifdef AV_USE_STD_MAP
    test_erase_increasing< std::map< int, S > >( REPS, 1000000, "std::map.erase_increasing" );
#endif

    std::cout << std::endl;
}

void erase_decreasing()
{
    test_erase_decreasing< AssocVector< int, S > >( 100 * REPS, 1000, "AssocVector.erase_decreasing" );
#ifdef AV_USE_LOKI
    test_erase_decreasing< Loki::AssocVector< int, S > >( 100 * REPS, 1000, "Loki::AssocVector.erase_decreasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_erase_decreasing< std::map< int, S > >( 100 * REPS, 1000, "std::map.erase_decreasing" );
#endif

    std::cout << std::endl;

    test_erase_decreasing< AssocVector< int, S > >( REPS, 1000000, "AssocVector.erase_decreasing" );
#ifdef AV_USE_LOKI
    test_erase_decreasing< Loki::AssocVector< int, S > >( REPS, 1000000, "Loki::AssocVector.erase_decreasing" );
#endif
#ifdef AV_USE_STD_MAP
    test_erase_decreasing< std::map< int, S > >( REPS, 1000000, "std::map.erase_decreasing" );
#endif

    std::cout << std::endl;
}

void erase_random()
{
    std::vector< int > array;

    for( int i = 0 ; i < 1000 ; ++ i )
        array.push_back( rand() + rand() - rand() );

    test_erase_random< AssocVector< int, S > >( 100 * REPS, array, "AssocVector.erase_random" );
#ifdef AV_USE_LOKI
    test_erase_random< Loki::AssocVector< int, S > >( 100 * REPS, array, "Loki::AssocVector.erase_random" );
#endif
#ifdef AV_USE_STD_MAP
    test_erase_random< std::map< int, S > >( 100 * REPS, array, "std::map.erase_random" );
#endif

    std::cout << std::endl;

    for( int i = 1000 ; i < 1000000 ; ++ i )
        array.push_back( rand() + rand() + rand() - rand() );

    test_erase_random< AssocVector< int, S > >( REPS, array, "AssocVector.erase_random" );
#ifdef AV_USE_LOKI
    test_erase_random< Loki::AssocVector< int, S > >( REPS, array, "Loki::AssocVector.erase_random" );
#endif
#ifdef AV_USE_STD_MAP
    test_erase_random< std::map< int, S > >( REPS, array, "std::map.erase_random" );
#endif

    std::cout << std::endl;
}

void random_operations()
{
    std::vector< std::pair< int, int > > array;

    for( int i = 0 ; i < 500 ; ++ i )
        array.push_back( std::make_pair( 0, rand() + rand() - rand() ) );

    for( int i = 500 ; i < 1000 ; ++ i )
        array.push_back( std::make_pair( rand() % 3, rand() + rand() - rand() ) );
        
    test_random_operations< AssocVector< int, S > >( 100 * REPS, array, "AssocVector.test_random_operations" );
#ifdef AV_USE_LOKI
    test_random_operations< Loki::AssocVector< int, S > >( 100 * REPS, array, "Loki::AssocVector.test_random_operations" );
#endif
#ifdef AV_USE_STD_MAP
    test_random_operations< std::map< int, S > >( 100 * REPS, array, "std::map.test_random_operations" );
#endif

    std::cout << std::endl;
    
    for( int i = 1000 ; i < 500000 ; ++ i )
        array.push_back( std::make_pair( 0, rand() + rand() - rand() ) );

    for( int i = 500000 ; i < 1000000 ; ++ i )
        array.push_back( std::make_pair( rand() % 3, rand() + rand() - rand() ) );
        
    test_random_operations< AssocVector< int, S > >( REPS, array, "AssocVector.test_random_operations" );
#ifdef AV_USE_LOKI
    test_random_operations< Loki::AssocVector< int, S > >( REPS, array, "Loki::AssocVector.test_random_operations" );
#endif
#ifdef AV_USE_STD_MAP
    test_random_operations< std::map< int, S > >( REPS, array, "std::map.test_random_operations" );
#endif

    std::cout << std::endl;
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
