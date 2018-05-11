libcmdparse version 0.3.2-0

# What is libcmdparse?
  libcmdparse is a simple library for command line argument parsing.
options may be declared in a domain specific language resembling
regular expressions. it is my own take on argument parsing rather
than a reimplementation of something else, which I intend not only
for conventional command-line argument parsing but also for highly
interactive applications that rely on structured user input.

# Details
## What is a command?
commands initiate parsing and own both options and other commands.
commands may be named or unnamed -- for command-line parsing, a root
unnamed command is probably appropriate, which can be created by the
default constructor of the Command class.

## What is an option?
options are represented by the Option class, which specifies the
properties of the option as well as its uniquely identifying name.
Option objects are considered equal if they have equal names.
options are identified in two key ways: through the name, which is
used internally while defining options to maintain consistency, and
externally by the user as the single access point to the results of
parsing; and through the handles which appear in unparsed input,
such as "--version" or "-V". the user provides handles at the time
options are defined; an option's name is either the second argument
to Command's option member or is deduced from the list of
handles.
options have four properties: number, assignment, collection, and 
arg type, which are used to express the full range of possible option
implementations. a typical boolean option, for instance,
might be implemented as
  ```c++
  command.option("--histexpand?")
  ```
here the handle is "--histexpand", and the "?" following the name
indicates that the option is allowed to appear zero or one time
(i.e. it has number ZERO\_ONE). this number is the default setting,
so the question mark could be left out without changing the meaning.
since no name is provided to the option method, the name of this
option is deduced to be simply "histexpand".

the following lists enumerate all of the values of the four properties,
their meanings, and the syntax used to set them. the parse input is the
first argument passed to Command's parse member, which has type
char \*\*. a handle list is the group of handles separated in the
option spec by pipes (|).

Number:
1. ZERO\_ONE | "?" at the end of the handle list (default) | option may occur zero or one time
2. ZERO\_MANY | "\*" at the end of the handle list | option may occur any number of times

Assignment:
1. NO\_ASSIGN | option does not take arguments (default) | no syntax needed
2. EQ\_REQUIRED | option takes argument, but must use an equal sign | "=" after handle list and number modifiers, if any
3. EQ\_MAYBE | option takes arguments, but may use an equals sign or grab the next value in the parse input | "=?"
4. EQ\_NEVER | option takes arguments, but may not use an equals sign | "=!"
5. STUCK | option takes an argument that is appended to the handle | "=|" (not yet implemented)

Collection:
1. SCALAR | argument treated as a single value for storage (default) | no syntax needed
2. LIST | argument treated as comma-separated list, each element of which is stored separately | "[]" following assignment property, if any

Arg\_Type:
1. STRING | argument is a string (default) | "s" after assignment property or in "[]"
2. INTEGER | argument must contain only digits | "i" ...
3. FLOAT | argument must contain only digits or a period surrounded by digits | "f" ...

## API
  the library uses the util namespace. functionality is distributed
in three classes: Option, Command, Info. in most
cases, Option need not be used directly. the other two classes
are essential to the library and will be discussed here.

more complete documentation is in html and man in the doc directory

### Command
Command is the class used to define all the possible options to
your command line application and to initiate parsing. it privately
uses a map to store pairs of "handles" and Option objects, and a set
to store the names of Option objects already registered in the
handle map. this set allows the parser to ensure that no equal but
incompatible options are registered. see above for clarification of
this scenario.
 
  `std::shared_ptr<Option> option(std::string spec, std::string name = "")`

    declare an option

  `std::shared_ptr<Command> command(std::string spec)`

    declare a command owned by *this

  `Info parse(char** argv, int argc, Info * d = nullptr)`

    parse all known options from argc words in argv

  `void clear()`

    free memory used to store options
### Info
  `bool has(std::string name)`

    check whether an option was found

  `bool has_command(std::string name)`

    check whether command was found anywhere (even if *this does not
    directly own it)

  `opt_data_t::size_type count(std::string name)`

    check the number of occurrences of an option

  `std::pair<bool, std::string> get(std::string name)`

    return a pair indicating whether option with name is present and
    its first value if true

  `std::tuple<bool,
              opt_data_t::const_iterator,
              opt_data_t::const_iterator> get_all(std::string name)`

    return boolean and iterators denoting range of values stored under
    option name

  `std::vector<std::string> rest`

    contains the non-option strings from the parsing source

## Dependencies
  libcmdparse depends only on the standard library of C++11

  building it requires CMake >= 3.0, Make, and a C++ compiler
that supports C++11.

  all tests depend on my version of libtap++, which is based on Leon
Timmermans' implementation.

# Examples
using namespace cli;
Command cmd; // create a new command
cmd.option("-h|--help", "help"); // add help option to command. only one
                                 // instance of -h or --help is allowed in argv
cmd.option("-v|--verbose*", "verbosity"); // add verbosity to command
                                          // any number of instances of this
                                          // option can appear
// argv looks like {"-h", "-v", "--verbose"}
Info info = cmd.parse(argv, 3);

info.has("help");        // returns true
info.count("help");      // returns 1
info.count("verbosity"); // returns 2

# Installation

## Ubuntu/Debian
after running cmake, use make to build, test, and install:
```shell
make
make test
sudo make install
```

make's `debug` target can be used in place of `test`. it runs uses a
shell script as a substitute for CTest, the test driver included with
CMake, because CTest does not play well with unit test libraries.
invoke ctest manually if you cannot or don't wish to run the script.
in that case, for better test output I recommend that you invoke ctest
with the `--verbose` option.

## Windows
libcmdparse doesn't support Windows

# License

Copyright (C) 2018 Adam Marshall

Made available under the MIT license, which is distributed with the
project itself
