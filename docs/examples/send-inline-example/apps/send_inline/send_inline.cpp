#include <send_inline.hpp>

#include <hello.hpp>

[[eosio::action]]
void send_inline::test( name user, name inline_code ) {
   // the code the contract is deployed on and a set of permissions
   hello::hi_action hi (inline_code, {_self, "active"_n});
   
   hi.send(user);
}

