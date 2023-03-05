#include <eosio/eosio.hpp>
using namespace eosio;

CONTRACT {{APROJ_PROJECT_NAME}} : public contract {
   public:
      using contract::contract;

      ACTION hi( name nm );

      using hi_action = action_wrapper<"hi"_n, &{{APROJ_PROJECT_NAME}}::hi>;
};