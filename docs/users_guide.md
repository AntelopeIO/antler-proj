# User's guide

##### For antler-proj Project Management System (AKA aproj) CLI

### Table of contents

[Prerequisites](#Prerequisites)

[Understanding of templates](#Understanding-of-templates)

[Hello World example](#Hello-World!)

[Adding of a new application](#Adding-of-a-new-Application)

[Building of a new application](#Building-of-a-new-Application)

[Deploying of a contract](#Deploying-of-a-contract)

[Using of a contract](#Using-of-a-contract)

[Addressbook example](#Addressbook-example)

[Tic-Tac-Toe example](#Tic-Tac-Toe-Application)

[Creation of a custom template](#Creating-of-a-custom-template)


In this guide we will learn how to use aproj to simplify and speedup of development of the contract projects. 
If you want to learn abilities of aporj, it's commands, how it works and how it designed please read this document https://github.com/AntelopeIO/antler-proj/blob/main/docs/usecases.md

In order to show why it make sense to use aproj we take examples from EOS tutorials and show how to create and manage these projects with aproj.

The general way of using of aptoj looks like this:

- ##### We ask aproj to create a new project for us.

Project is a complex of applications which implements the smart contracts and other kinds of programs, programming libraries and other dependencies which needed for building of the applications, and tests needed for testing of the applications and the libraries.

- ##### We ask aproj to add a new application, library or test to the project.

By default aproj creates a set of empty directories, project files and building files enough for start of the building process. 
But we may ask aproj add a new application, library or test from templates of the template library of aproj.
In this case it adds files where implemented frequently used project structures and functions. 
Or even you can create your own template which implements all needed features which you often use.

- ##### We add to the project descriptions of all dependencies needed building for our applications, libraries or tests

Like external libraries or/and data files.

- ##### We ask aproj to download all external dependencies and build all part of the project.

And here we have binaries of the smart contracts ready for deploying into the block chain and work



## Prerequisites

Lets say we have successfully installed EOS tools and the test network is already bootstrapped and ready for deploying of the user's contacts.

## Understanding of templates

Aproj has a library of templates. They allows to make a stub of any kind of application and use it as a good base for a new project where the frequently used code is already implemented. You won't need to copy it from one project to another and manually edit names of files, classes and variables which are different in different projects.

The library of templates is organized very simple. It is a set of directories. Each directory contains directory structure of a project with all needed files for this project. All the texts inside of the files which may be changed are described as macros. Creation of a new project from a template have following algorithm:
 
0. User runs a command: aproj ./project_path add --app --template template_name
 
1. Copy of a directories and files from the chosen template into the project directory; 
2. Change names of files and directories corresponding to name of the project 
3. Process each file with macro processor to substitute macros to real data in the files. Any user may  create his/her own templates by copying and editing of existing templates and adding of a new directory with the new template to the template storage.

Later in this guide we will learn this processes more details and create a new template from an existing template.

## Hello World!

Let's see how to create and manage a project in aproj on the example of smart contract Hello World, described in EOS Tutorial here https://docs.eosnetwork.com/docs/latest/getting-started/smart-contract-development/hello-world/

The tutorial has a detailed description of how to write, build and deploy the contract from scratch. Aproj is a more hight level tool designed to avoid manual work as much as possible. So here we will not create this project from scratch but create if from an existing template. This template calls "basic" and contains an implementation of a contract with one action. So after creation of the project from this template the user need only feel the method of the action by his own logic. We will write there the code which will prints "Hello <user>" as in the tutorial.

## The Creation

> _In the beginning was the Word, and the Word was with two bytes._

We need to create a directory with an empty project structure into this directory. So let's call

`aproj init --name project`

This call creates a new directory "project" in the current directory with an empty project structure.
Optional key `--ver` may be used to define an initial version of the project. 

For example: `aproj init --name project --ver 1.0`

If the initial version of the project is not explicitly defined it is set to 0.0. 

```
──project
    ├───apps
    ├───include  
    ├───ricardian
    ├───libs
    ├───tests
    └───project.yaml
```

This is the structure of an empty project. It contains a `project.yaml` file with description of the project and 
directories where will be created applications, programming libraries and test for these applications and
libraries.


## Adding of a new Application
#### Hello example

In order to add a new application we should call the following command:

`aproj add ./project --app --name hello --template basic`

This call creates an application's directory `hello` in the project's directory `apps`
 
```
apps/
└── hello
    ├── build
    ├── CMakeLists.txt
    ├── include
    │   └── hello.hpp
    ├── README.txt
    ├── ricardian
    │   └── hello.contracts.md
    └── src
        ├── CMakeLists.txt
        └── hello.cpp
```

with all needed files for building of the project. File `hello.cpp` has implementation of a contract with
and empty action:

```c++
#include <eosio/eosio.hpp>
#include "hello.hpp"

using namespace eosio;

class [[eosio::contract]] hello : public contract {
  public:
      using contract::contract;

      [[eosio::action]]
      void my_action() {
        // Write implementation of your action here 
      }
};
```

We may rename the action and add the printing of a message like in the example from EOS tutorials https://docs.eosnetwork.com/docs/latest/getting-started/smart-contract-development/hello-world/
                                                                                                  
```c++
#include <eosio/eosio.hpp>
#include "hello.hpp"

using namespace eosio;

class [[eosio::contract]] hello : public contract {
  public:
      using contract::contract;

      [[eosio::action]]
      void hi( name user ) {
         print( "Hello, ", user);
      }
};
```

Update the Ricardian file and describe there what your contact and it's action do.

Here is how generated Ricardian file looks like:
```html
<h1 class="contract">hello</h1>
---
spec-version: 0.0.1
title: my_action
summary: This action is an example of the action implementation. 
You should rename this action and add your own code which will do something useful. 
icon:
```

Let's update it with correct description of our action:

```html
<h1 class="contract">hello</h1>
---
spec-version: 0.0.1
title: hi
summary: This action prints text "Hello, user" to the terminal with name of a user given by the 
caller in the command line.
icon:
```

## Building of a new Application

It time to build our app.

Call `aproj build ./project`

builds all libraries applications and tests described in the project.

Call `aproj build ./project --app --name hello` 
builds only an application with name hello.

If you need to rebuild the app or whole project from scratch, call:

`aproj clean ./project && aproj build ./project` 

or 

`aproj clean ./project --app --name hello && aproj build ./project --app --name hello`

for the application `hello` only


## Deploying of a contract

When our contract is built we should deploy it to out block chain.
It is easy to do with `DUNE`

Call `dune --deploy ./projects/apps/hello/build/hello/ eosio`

## Using of a contract

Then let's call the action from the deployed contract.

Call `dune --send-action eosio hello hi '["bob"]' bob@active`

## Addressbook example
#### Adding of a new Application

Now let's make an example described in EOS Tutorial here: https://docs.eosnetwork.com/docs/latest/getting-started/smart-contract-development/data-persistence/
This is the implementation of an address book which shows how to use persistent data tables for saving data.

The good base for making of this example is template `database` which generates implementation of a 
simple database with one multi index table. In order to add a new application to the project let's call:

`aproj add ./project --app --name addressbook --template database`

This call creates an application's directory `addressbook` in the project's directory `apps`.
Now we have the following directory tree in the apps directory:
 
```
apps/
└── hello
    ├── build
    ├── CMakeLists.txt
    ├── include
    │   └── hello.hpp
    ├── README.txt
    ├── ricardian
    │   └── hello.contracts.md
    └── src
        ├── CMakeLists.txt
        └── hello.cpp
└── addressbook
    ├── build
    ├── CMakeLists.txt
    ├── include
    │   └── addressbook.hpp
    ├── README.txt
    ├── ricardian
    │   └── addressbook.contracts.md
    └── src
        ├── CMakeLists.txt
        └── addressbook.cpp

```

File `addressbook.cpp` has implementation of a contract with a multi index table and action for adding, 
changing and removing data:

```c++

#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract("addressbook")]] addressbook : public eosio::contract {

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

  addressbook(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

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

```

Its easy to rewrite the template to the code from the tutorial because it shows obvious example 
of how to work with the tables.

```c++
#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract("addressbook")]] addressbook : public eosio::contract {
private:
  struct [[eosio::table]] person {
    name key;
    std::string first_name;
    std::string last_name;
    std::string street;
    std::string city;
    std::string state;
    uint64_t primary_key() const { return key.value; }
  };

  using address_index = eosio::multi_index<"people"_n, person>;

public:

  addressbook(name receiver, name code,  datastream<const char*> ds): contract(receiver, code, ds) {}

  [[eosio::action]]
  void upsert(name user, std::string first_name, std::string last_name, std::string street, std::string city, std::string state) {
    require_auth( user );
    address_index addresses( get_self(), get_first_receiver().value );
    auto iterator = addresses.find(user.value);
    if( iterator == addresses.end() )
    {
      addresses.emplace(user, [&]( auto& row ) {
       row.key = user;
       row.first_name = first_name;
       row.last_name = last_name;
       row.street = street;
       row.city = city;
       row.state = state;
      });
    }
    else {
      addresses.modify(iterator, user, [&]( auto& row ) {
        row.key = user;
        row.first_name = first_name;
        row.last_name = last_name;
        row.street = street;
        row.city = city;
        row.state = state;
      });
    }
  }

  [[eosio::action]]
  void erase(name user) {
    require_auth(user);

    address_index addresses( get_self(), get_first_receiver().value);

    auto iterator = addresses.find(user.value);
    check(iterator != addresses.end(), "Record does not exist");
    addresses.erase(iterator);
  }
};    
```

## Building of the Addressbook application

Now we have two applications in our project

Call `aproj build ./project`

builds both of the applications.

In order to build only the Addressbook app

Call `aproj build ./project --app --name addressbook` 

The same for cleaning and rebuilding:

`aproj clean ./project --app --name addressbook && aproj build ./project --app --name addressbook`

## Deploying of the Addressbook contract

Call `dune --deploy ./projects/apps/addressbook/build/addressbook/ eosio`

## Using of the Addressbook contract

Then let's call the action from the deployed contract.

Call: `dune --send-action eosio addressbook upsert '["alice", "alice", "liddell", "123 drink me way", "wonderland", "amsterdam"]' alice@active`

## Tic-Tac-Toe Application

This is another example from EOS Tutorial described here: 
https://docs.eosnetwork.com/docs/latest/tutorials/tic-tac-toe-game-smart-contract-single-node/
This example shows how to work with the same data table from several accounts, therefore the only difference with the
previous example of the Addressbook is adding of accounts of the game players. It means that we can use for
start of this project the same template and put our implementation of the contract. 
 
So, in order to add a new application to the  project let's call:

`aproj add ./project --app --name tic-tac-toe --template database`

This call creates an application's directory `tic-tac-toe` in the project's directory `apps`. 
Now we have the following directory tree in the apps directory:

```
apps/
└── hello
    ...
└── addressbook
    ...
└── tic-tac-toe
    ├── build
    ├── CMakeLists.txt
    ├── include
    │   └── tic-tac-toe.hpp
    ├── README.txt
    ├── ricardian
    │   └── tic-tac-toe.contracts.md
    └── src
        ├── CMakeLists.txt
        └── tic-tac-toe.cpp
```

Add an implementation of the contract to file `tic-tac-toe.hpp`

```c++
#include <eosio/eosio.hpp>


// Use the [[eosio::contract(contract_name)]] attribute. Inherit from the base contract class. 
class[[eosio::contract("tictactoe")]] tictactoe : public contract {
public:
    
    // Introduce base class members. 
    using contract::contract;
    
    // Use the base class constructor.
    tictactoe(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds) {}

    // Declaration of actions of the contract
    [[eosio::action]]
    void create(const name &challenger, name &host);
    
    [[eosio::action]]
    void restart(const name &challenger, const name &host, const name &by);

    [[eosio::action]]
    void close(const name &challenger, const name &host);
    
    [[eosio::action]]   
    void move(const name &challenger, const name &host, const name &by, const uint16_t &row, const uint16_t &column);
};

// Declare game data structure.
struct [[eosio::table]] game {
    static constexpr uint16_t boardWidth = 3;
    static constexpr uint16_t boardHeight = boardWidth;
    
    game() : board(boardWidth * boardHeight, 0){}

    name challenger, host, turn; // = account name of host, challenger and turn to store whose turn it is.
    name winner = none; // = none/ draw/ name of host/ name of challenger

    std::vector<uint8_t> board;

    // Initialize the board with empty cell
    void initializeBoard() {
        board.assign(boardWidth * boardHeight, 0);
    }

    // Reset game
    void resetGame() {
        initializeBoard();
        turn = host;
        winner = "none"_n;
    }

    // primary key accessor
    auto primary_key() const { return challenger.value; }

    // EOSLIB_SERIALIZE macro defining how the abi serializes / deserializes  
    EOSLIB_SERIALIZE( game, (challenger)(host)(turn)(winner)(board))
};

// Define the game data structure using the multi-index table template.
typedef eosio::multi_index<"games"_n, game> games;

```

And to file `tic-tac-toe.cpp`

```c++
#include "tic-tac-toe.hpp"

using namespace eosio;

void tictactoe::create(const name &challenger, name &host) {
    require_auth(host);
    check(challenger != host, "Challenger should not be the same as the host.");

    // Check if game already exists
    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr == existingHostGames.end(), "Game already exists.");

    existingHostGames.emplace(host, [&](auto &g) {
        g.challenger = challenger;
        g.host = host;
        g.turn = host;
    });
}

void tictactoe::restart(const name &challenger, const name &host, const name &by){
    check(has_auth(by), "Only " + by.to_string() + "can restart the game.");

    // Check if game exists
    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr != existingHostGames.end(), "Game does not exist.");

    // Check if this game belongs to the action sender
    check(by == itr->host || by == itr->challenger, "This is not your game.");

    // Reset game
    existingHostGames.modify(itr, itr->host, [](auto &g) {
        g.resetGame();
    });
}

void tictactoe::close(const name &challenger, const name &host){
    check(has_auth(host), "Only the host can close the game.");

    require_auth(host);

    // Check if game exists
    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr != existingHostGames.end(), "Game does not exist.");

    // Remove game
    existingHostGames.erase(itr);
}

bool tictactoe::isEmptyCell(const uint8_t &cell) {
    return cell == 0;
}

bool tictactoe::isValidMove(const uint16_t &row, const uint16_t &column, const std::vector<uint8_t> &board){
    uint32_t movementLocation = row * game::boardWidth + column;
    bool isValid = movementLocation < board.size() && isEmptyCell(board[movementLocation]);
    return isValid;
}

name tictactoe::getWinner(const game &currentGame) {
    auto &board = currentGame.board;

    bool isBoardFull = true;

    // Use bitwise AND operator to determine the consecutive values of each column, row and diagonal
    // Since 3 == 0b11, 2 == 0b10, 1 = 0b01, 0 = 0b00
    std::vector<uint32_t> consecutiveColumn(game::boardWidth, 3);
    std::vector<uint32_t> consecutiveRow(game::boardHeight, 3);
    uint32_t consecutiveDiagonalBackslash = 3;
    uint32_t consecutiveDiagonalSlash = 3;

    for (uint32_t i = 0; i < board.size(); i++)
    {
        isBoardFull &= isEmptyCell(board[i]);
        uint16_t row = uint16_t(i / game::boardWidth);
        uint16_t column = uint16_t(i % game::boardWidth);

        // Calculate consecutive row and column value
        consecutiveRow[column] = consecutiveRow[column] & board[i];
        consecutiveColumn[row] = consecutiveColumn[row] & board[i];
        // Calculate consecutive diagonal \ value
        if (row == column)
        {
            consecutiveDiagonalBackslash = consecutiveDiagonalBackslash & board[i];
        }
        // Calculate consecutive diagonal / value
        if (row + column == game::boardWidth - 1)
        {
            consecutiveDiagonalSlash = consecutiveDiagonalSlash & board[i];
        }
    }

    // Inspect the value of all consecutive row, column, and diagonal and determine winner
    std::vector<uint32_t> aggregate = {consecutiveDiagonalBackslash, consecutiveDiagonalSlash};
    aggregate.insert(aggregate.end(), consecutiveColumn.begin(), consecutiveColumn.end());
    aggregate.insert(aggregate.end(), consecutiveRow.begin(), consecutiveRow.end());

    for (auto value : aggregate)
    {
        if (value == 1)
        {
            return currentGame.host;
        }
        else if (value == 2)
        {
            return currentGame.challenger;
        }
    }
    // Draw if the board is full, otherwise the winner is not determined yet
    return isBoardFull ? draw : none;
}

void tictactoe::move(const name &challenger, const name &host, const name &by, const uint16_t &row, const uint16_t &column){
    check(has_auth(by), "The next move should be made by " + by.to_string());

    // Check if game exists
    games existingHostGames(get_self(), host.value);
    auto itr = existingHostGames.find(challenger.value);
    check(itr != existingHostGames.end(), "Game does not exist.");

    // Check if this game hasn't ended yet
    check(itr->winner == none, "The game has ended.");
    
    // Check if this game belongs to the action sender
    check(by == itr->host || by == itr->challenger, "This is not your game.");
    // Check if this is the  action sender's turn
    check(by == itr->turn, "it's not your turn yet!");

    // Check if user makes a valid movement
    check(isValidMove(row, column, itr->board), "Not a valid movement.");

    // Fill the cell, 1 for host, 2 for challenger
    const uint8_t cellValue = itr->turn == itr->host ? 1 : 2;
    const auto turn = itr->turn == itr->host ? itr->challenger : itr->host;
    existingHostGames.modify(itr, itr->host, [&](auto &g) {
        g.board[row * game::boardWidth + column] = cellValue;
        g.turn = turn;
        g.winner = getWinner(g);
    });
}
```

Describe actions of the contract in `tic-tac-toe.contracts.md` file:

```html
<h1 class="contract">create</h1>
---
spec-version: 0.0.1
title: create
summary: This action launches a new game and creates a new game board array. The host may use this command.
icon:

<h1 class="contract">restart</h1>
---
spec-version: 0.0.1
title: restart
summary: This action clears data from an existing game board array. The host or the challenger may use this command.
icon:

<h1 class="contract">close</h1>
---
spec-version: 0.0.1
title: close
summary: This action deletes and removes existing game data and frees up any storage the game uses. No game data persists. The host may use this command.
icon:


<h1 class="contract">move</h1>
---
spec-version: 0.0.1
title: move
summary: This action sets a marker on the gameboard and updates the game board array. The host or the challenger may use this command.
icon:
```
#### Build Tic-Tac-Toe application

Call `aproj build ./project --app --name tic-tac-toe` 

#### Deploying of the Tic-Tac-Toe contract

Call `dune --deploy ./projects/apps/tic-tac-toe/build/tic-tac-toe/ tictactoe@active`

#### Play Tic-Tac-Toe game

#### Create a Game

`dune --send-action eosio tictactoe create '{"challenger":"challenger", "host":"host"}' host@active`

#### Making Game Moves

`dune --send-action eosio tictactoe move '{"challenger":"challenger", "host":"host", "by":"host", "row":0, "column":1}' host@active`

#### Check Game Status

`dune --get-table host tictactoe games`

This call prints current state of the table `games`

#### Restart the Game

`dune --send-action eosio tictactoe restart '{"challenger":"challenger", "host":"host", "by":"host"}' host@active`

##### Close the Game

`dune --send-action eosio tictactoe close '{"challenger":"challenger", "host":"host"}' host@active`

## Creating of a custom template

Template is a directory structure which contains all files needed for a project. 
The only difference with a project directory structure is that all names inside of the files
that dependent on name and type of the project are changed to macros. 

When Antler-proj creates a new project from a template it copy the directory structure of 
a chosen template to the project directory, rename files which contain the template name to the project name
and run a macro processor for each of the template file. As you see it is easy to convert to a template
any of your project.

Let's see at the simplest template `"basic"`. All templates are placed in `~/.config/aproj/templates` directory

Let's go there and find a directory with name `"basic"`. It has following structure:

```
templates/
└── basic
    ├── build
    ├── CMakeLists.txt
    ├── include
    │   └── basic.hpp
    ├── README.txt
    ├── ricardian
    │   └── basic.contracts.md
    └── src
        ├── CMakeLists.txt
        └── basic.cpp
```

Look inside `basic.cpp` file:

```c++
#include <eosio/eosio.hpp>
#include "PROJECT_NAME.hpp"

using namespace eosio;

class [[eosio::contract]] PROJECT_NAME : public contract {
  public:
      using contract::contract;

      [[eosio::action]]
      void my_action() {
        // Write implementation of your action here 
      }
};
```

Here PROJECT_NAME is a macro which will be changed to an actual name of a project which user declare when 
calls:
 
`aproj add ./project --app --name hello --template basic`

A project can't have the same name as a template. If the user calls:

`aproj add ./project --app --name basic --template basic`

He gets an error message:

> "Name of your project is the same as a name of a template. Please choose another name."

In process of creation of a new project after the call:
 
`aproj add ./project --app --name hello --template basic`

aproj will call a macro processor like:

`cpp -DPROJECT_NAME=hello $(TEMPLATES_DIR)/basic.cpp -o $(PROJECT_DIR)/apps/hello/src/hello.cpp` 

Other files use the same approach to generalize the project structure in a template.

