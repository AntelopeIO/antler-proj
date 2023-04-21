#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] test: public contract {
   public:
      using contract::contract;

      [[eosio::action]]
      void hi( name nm );
      using hi_action = action_wrapper<"hi"_n, &test::hi>;
};

