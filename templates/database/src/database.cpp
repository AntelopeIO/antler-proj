#include <eosio/eosio.hpp>
#include <{{APROJ_PROJECT_NAME}}.hpp>

using namespace eosio;

class [[eosio::contract("{{APROJ_PROJECT_NAME}}")]] {{APROJ_PROJECT_NAME}} : public eosio::contract {

private:
  // This is a declaration of data which will contain a table placed in our database 
  struct [[eosio::table]] data_t {
    // each table must contain at least one key index field for access to the data
    // please declare at least one variable which will contain an unique index for each row in the table  
    unsigned int prim_key; 
    // Here you may declare any number of any data fields for your table
    std::vector<unsigned int> data;
    // each data structure must implement this method for access to the primary key
    int primary_key() const { return prim_key; }
  };

  // declaration of a type of a table which will contain our data
  using table_t = eosio::multi_index<"my_table"_n, data_t>;

public:

  {{APROJ_PROJECT_NAME}}(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  // Action method which adds a new row to the table
  [[eosio::action]]
  void insert() {
    // Declare a counter for generation of unique values
    static std::atomic<unsigned int> uid { 0 };
    // Create an instance of a class for work with the table
    table_t table( get_self(), get_first_receiver().value);
    // Adding a new row to the table
    table.emplace(get_self(), [&]( auto& row ) {
       row.prim_key = ++uid; // unique value for the primary key
       row.data.push_back(row.prim_key); // add initialization of your data row here.
    });
  }

  // Action method which updates an existing data row with the new data
  [[eosio::action]]
  void update(unsigned int prim_key, data_t d) {
    table_t table( get_self(), get_first_receiver().value);
    auto iterator = table.find(prim_key);
    // modify method has undefined behavior if the data is not exists
    if( iterator != table.end() ) {
      table.modify(iterator, get_self(), [&]( auto& row ) {
        row.data = d.data;
    }
  }

  // Action method which erases given data row
  [[eosio::action]]
  void erase(unsigned int prim_key) {
    table_t table( get_self(), get_first_receiver().value);
    auto iterator = table.find(prim_key);
    check(iterator != table.end(), "Record does not exist");
    table.erase(iterator);
  }
};