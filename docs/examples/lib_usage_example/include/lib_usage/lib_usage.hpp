#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] lib_usage: public contract {
   public:
      using contract::contract;

      [[eosio::action]]
      void exec(int, float);
      using exec_action = action_wrapper<"exec"_n, &lib_usage::exec>;
};

