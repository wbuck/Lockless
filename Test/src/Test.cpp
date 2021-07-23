#include "ConcurrentStack.hpp"
#include "Lock.hpp"
#include "Queue.hpp"
#include <iostream>
#include <type_traits>
#include <thread>
#include <random>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <format>

struct Test {
    std::uint32_t v1;
    std::uint32_t v2;
};

int main( ) {     
    
    auto [p, c] = sync::mpsc::create_channel<int>( 10 );

    using namespace std::chrono_literals;
    static std::mutex lock{ };
       
    ConcurrentStack<int> list{ };

    {
        std::jthread t1{ [ &list ] { 
            std::random_device device{ };
            std::mt19937 rng{ device( ) };
            std::uniform_int_distribution<std::mt19937::result_type> dist{ 1, 500 };

            for ( int i = 0; i < 10; ++i ) {
                list.push_front( i );
                std::this_thread::sleep_for( std::chrono::milliseconds{ dist( rng ) } );
            }
        } };

        std::jthread t2{ [ &list ] {
            std::random_device device{ };
            std::mt19937 rng{ device( ) };
            std::uniform_int_distribution<std::mt19937::result_type> dist{ 1, 500 };

            for ( int i = 10; i < 20; ++i ) {
                list.push_front( i );
                std::this_thread::sleep_for( std::chrono::milliseconds{ dist( rng ) } );
            }
        } };

        std::jthread t3{ [ &list ] {
            long count{ 0 };
            while ( count < 10 ) {
                if ( auto value{ list.pop_front( ) }; value.has_value( ) ) {                        
                    std::scoped_lock l{ lock };
                    std::cout << std::format( "[t3] Found: {}\n", *value.value( ) );                    
                }
                else { 
                    ++count; 
                    std::this_thread::sleep_for( 500ms );
                }
            }
        } };

        std::jthread t4{ [ &list ] {
            long count{ 0 };
            while ( count < 10 ) {
                if ( auto value{ list.pop_front( ) }; value.has_value( ) ) {
                    std::scoped_lock l{ lock };
                    //lock.~Lock( );
                    std::cout << "[t4] Found: " << *value.value( ) << '\n';                    
                }
                else { 
                    ++count; 
                    std::this_thread::sleep_for( 500ms );
                }
            }
        } };

    }

    
}

