#include <test.hpp>


[[eosio::action]] void test::hi(name nm)
{
   /* generated example action */
   print_f("Hello : %", nm);
}


extern const std::string GREETING;
std::string              greeting()
{
   return GREETING;
}
