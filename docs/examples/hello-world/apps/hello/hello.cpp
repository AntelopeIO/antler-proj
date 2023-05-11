#include <hello.hpp>

[[eosio::action]]
void hello::hi( name nm ) {
   /* generated example action */
   print_f("Hello, world : %", nm);
}

