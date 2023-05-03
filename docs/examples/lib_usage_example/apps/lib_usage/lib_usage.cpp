#include <lib_usage.hpp>

#include <print_number.hpp>
#include <print_float.h>

[[eosio::action]]
void lib_usage::exec(int inum, float fnum) {
   print_num(inum); // C++ library call
   print_float(fnum); // C library call
}