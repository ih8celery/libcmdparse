liboptparse version 0.3.1-0

# What is liboptparse?
  liboptparse is a simple library for command line argument parsing.
options may be declared in a domain specific language resembling
regular expressions.

# Why does liboptparse exist?
  this project may seem to imitate existing tools. that
impression would be correct. the purpose of liboptparse is to
experiment with C++ while doing something which is (suprisingly)
useful in that same language: command-line argument processing.
this project is done solely for my educational benefit, as I am new to
C++, so all constructive criticism is welcomed.

# Details
## What is an option?
options are represented by the option\_t struct, which specifies the
properties of the option as well as its uniquely identifying name.
option\_t objects are considered equal if they have equal names.
options are identified in two key ways: through the name, which is
used internally while defining options to maintain consistency, and
externally by the user as the single access point to the results of
parsing; and through the handles which appear in unparsed input,
such as "--version" or "-V". the user provides handles at the time
options are defined; an option's name is either the second argument
to option\_parser's option member or is deduced from the list of
handles.
options have four properties: number, assignment, collection, and 
data type, which are used to express the full range of possible option
implementations. a typical boolean option, for instance,
might be implemented as
  ```c++
  parser.option("--histexpand?")
  ```
here the handle is "--histexpand", and the "?" following the name
indicates that the option is allowed to appear zero or one time
(i.e. it has number ZERO\_ONE). this number is the default setting,
so the question mark could be left out without changing the meaning.
since no name is provided to the option method, the name of this
option is deduced to be simply "histexpand".

the following lists enumerate all of the values of the four properties,
their meanings, and the syntax used to set them. the parse input is the
first argument passed to option\_parser's parse member, which has type
char \*\*. a handle list is the group of handles separated in the
option spec by pipes (|).

Num\_Prop:
1. ZERO\_ONE | "?" at the end of the handle list (default) | option may occur zero or one time
2. ZERO\_MANY | "\*" at the end of the handle list | option may occur any number of times

Assign\_Prop:
1. NO\_ASSIGN | option does not take arguments (default) | no syntax needed
2. EQ\_REQUIRED | option takes argument, but must use an equal sign | "=" after handle list and number modifiers, if any
3. EQ\_MAYBE | option takes arguments, but may use an equals sign or grab the next value in the parse input | "=?"
4. EQ\_NEVER | option takes arguments, but may not use an equals sign | "=!"
5. STUCK | option takes an argument that is appended to the handle | "=|" (not yet implemented)

Collect\_Prop:
1. SCALAR | argument treated as a single value for storage (default) | no syntax needed
2. LIST | argument treated as comma-separated list, each element of which is stored separately | "[]" following assignment property, if any

Data\_Type:
1. STRING | argument is a string (default) | "s" after assignment property or in "[]"
2. INTEGER | argument must contain only digits | "i" ...
3. FLOAT | argument must contain only digits or a period surrounded by digits | "f" ...

## API
  the library uses the util namespace. functionality is distributed
in three classes: option\_t, option\_parser, option\_info. in most
cases, option\_t need not be used directly. the other two classes
are essential to the library and will be discussed here.

### option\_parser:
option\_parser is the class used to define all the possible options to
your command line application and to initiate parsing. it privately
uses a map to store pairs of "handles" and option\_t objects, and a set
to store the names of option\_t objects already registered in the
handle map. this set allows the parser to ensure that no equal but
incompatible options are registered. see above for clarification of
this scenario.
 
  `std::shared_ptr<option_t> option(std::string spec, std::string name = "")`

    declare an option

  `option_info parse(char** argv, int argc)`

    parse all known options from argc words in argv

  `void clear_options()`

    free memory used to store options
### option\_info:
  `bool has(std::string name)`

    check whether an option was found

  `int count(std::string name)`

    check the number of occurrences of an option

  `std::string arg(std::string name, std::string default)`

    return the first argument found under option name, or the default

  `IterPair list(std::string name)`

    return iterators denoting range of values stored under option name

  `std::vector<std::string> rest`

    contains the non-option strings from the parsing source

## Dependencies
  liboptparse depends only on the standard library of C++11

  building it requires CMake >= 3.0, Make, and a C++ compiler
that supports C++11.

  all tests depend on libtap++.

# Installation

## Ubuntu/Debian
Clone the project and `cd` to its directory.
Run CMake on the current directory:
```shell
cmake .
```

Then use make to build, test, and install:
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
liboptparse doesn't support Windows

# License

Copyright (C) 2018 Adam Marshall

Made available under the MIT license, which is distributed with the
project itself
