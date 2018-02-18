liboptparse version 0.1.0

#What is liboptparse?
  liboptparse is a simple library for command line argument parsing.
options may be declared in a domain specific language resembling
regular expressions. the parsing target will be passed manually

#Why does liboptparse exist?
  this project may seem to imitate existing tools. that
impression would be correct. the purpose of liboptparse is to
experiment with C++ while doing something which is (suprisingly)
useful in that same language: command-line argument processing.
this project is done solely for my educational benefit, but all 
constructive criticism is welcomed.

# Details
##What is an option?
// TODO

##Dependencies
  liboptparse depends only on the standard library of C++11

##API
  the library uses the util namespace. functionality is distributed
in three classes: option\_t, option\_parser, option\_info. in most
cases, option\_t need not be used directly. the other two classes
are essential to the library and will be discussed here.

###option\_parser:
  std::shared\_ptr\<option\_t\> option(std::string spec, std::string name = "")

    create an option

  option\_info parse(char\*\* argv, int argc)

    parse all known options from argc words in argv

  void clear\_options()

    free all memory used to store options
###option\_info:
  bool has(std::string name)

    check whether an option was found

  int count(std::string name)

    check the number of occurrences of an option

  std::string arg(std::string name, std::string default)

    return the first argument found under option name, or the default

  IterPair list(std::string name)

    return iterators denoting range of values stored under option name

  std::vector<std::string> rem

    contains the non-option strings from the parsing source
##Usage

#Installation

#Issues

#License
