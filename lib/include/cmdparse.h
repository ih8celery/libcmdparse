/**
 * \file cmdparse.h
 * 
 * \author Adam Marshall (ih8celery)
 *
 * \brief interface to parse command line arguments/options
 */
#ifndef _MOD_CPP_COMMAND_PARSE

#define _MOD_CPP_COMMAND_PARSE

#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <exception>
#include <memory>
#include <tuple>

namespace cli {
  namespace Property {
    /**
     * \enum Number
     * \brief defines the number of times an option may appear
     *
     * <number> := <nil> // Number::ZERO_ONE <br>
     *          | '?'    // Number::ZERO_ONE <br>
     *          | '*'    // Number::ZERO_MANY <br>
     */
    enum class Number {
      ZERO_ONE, ZERO_MANY
    };

    /**
     * \enum Assignment
     * \brief defines the mode of assignment supported by option
     *
     * <assignment> := <nil> // Assignment::NONE <br>
     *              | '='   // Assignment::EQ_REQUIRED <br>
     *              | '=?'  // Assignment::EQ_MAYBE <br>
     *              | '=!'  // Assignment::EQ_NEVER <br>
     *              | '=|'  // Assignment::STUCK_ARG <br>
     */
    enum class Assignment {
      NO_ASSIGN, EQ_REQUIRED, EQ_MAYBE, EQ_NEVER, STUCK
    };
    
    /**
     * \enum Collection
     * \brief defines how arguments are interpreted
     *
     * <collection> := <nil>            // Collection::SCALAR <br>
     *              | '['<data_prop>']' // Collection::LIST <br>
     */
    enum class Collection {
      SCALAR, LIST
    };
    
    /**
     * \enum Arg_Type
     * \brief defines the type of data of an option's argument
     *
     * <data_type> := <nil> // Arg_Type::STRING <br>
     *             | 's'   // Arg_Type::STRING <br>
     *             | 'i'   // Arg_Type::INTEGER <br>
     *             | 'f'   // Arg_Type::FLOAT <br>
     */
    enum class Arg_Type {
      STRING, INTEGER, FLOAT
    };
  }

  /**
   * \class Info
   * \brief represent data collected during parsing
   */
  class Info {
    friend class Command;
    using opt_data_t = std::unordered_multimap<std::string, std::string>;

    public:
      /**
       * \fn bool has(const string&)
       * \brief test whether a particular option was found in parsing
       */
      bool has(const std::string&) const;

      /**
       * \fn bool has_command(const string&)
       * \brief test whether a command was found in argv
       */
      bool has_command(const std::string&) const;
      
      /**
       * \fn pair<bool, string> get(const string&)
       * \brief retrieve value of option
       */
      std::pair<bool, std::string> get(const std::string&) const;
      
      /**
       * \fn tuple<bool, opt_data_t::iterator, opt_data_t::iterator> get_all(const string&)
       * \brief retrieve the range of iterators with parameter as key
       */
      std::tuple<bool, opt_data_t::const_iterator, opt_data_t::const_iterator>
        get_all(const std::string&) const;

      /**
       * \fn opt_data_t::size_type count(const string&)
       * \brief count occurrences of option during parsing
       */
      opt_data_t::size_type count(const std::string&) const;

      /**
       * \var vector<string> rest
       * \brief contains non-options found during parsing
       */
      std::vector<std::string> rest;

    private:
      opt_data_t data;
      std::set<std::string> commands;
  };

  /**
   * \struct Option
   * \brief collect the properties and identity of option
   */
  class Option {
    public:
      /**
       * \fn Option()
       * \brief default construct Option
       */
      Option();

      Property::Number number;
      Property::Assignment assignment;
      Property::Collection collection;
      Property::Arg_Type type;
      std::string name;

      friend bool operator<(const Option&, const Option&) noexcept;
      friend bool operator==(const Option&, const Option&) noexcept;
  };

  /**
   * \class opt_parser
   * \brief class controlling option declaration and parsing
   */
  class Command {
    public:
      /**
       * \fn Command()
       * \brief create a Command object using the empty string as name
       */
      Command();

      /**
       * \fn Command(const string&)
       * \brief create a Command object using string argument as name
       */
      Command(const std::string&);

      /**
       * \fn shared_ptr<Option> option(const char *, const string& = "")
       * \brief declare an option to the parser
       *
       * option name is the second argument to the function <br>
       * see doc/option/spec.md for full description of options. <br>
       * throws an option_language_error if something goes wrong <br>
       */
      std::shared_ptr<Option> option(const char *, const std::string& = "");

      /**
       * \fn shared_ptr<Command> command(const char *)
       * \brief declare a command owned by this object
       */
      std::shared_ptr<Command> command(const std::string&);

      /**
       * \fn opt_info parse(char **&, int)
       * \brief extract options from argv into an opt_info object
       *
       * any options or data belonging to options is removed from <br>
       * argv and replaced with nullptr. <br>
       * throws a parse_error if something goes wrong <br>
       */
      Info parse(char **, int, Info * = nullptr) const;

      /**
       * \fn bool empty() const
       * \brief tests whether the parser has any registered options
       */
      bool empty() const noexcept;

      /**
       * \fn void clear()
       * \brief clear the data collected from calls to option()
       */
      void clear();

      /**
       * \fn void configure(const string&)
       * \brief toggle boolean private members of Command class
       */
      void configure(const std::string&);

      /**
       * \fn bool handle_has_name(const string&, const string&)
       * \brief test whether option attached to handle has a certain name
       */
      bool handle_has_name(const std::string&, const std::string&) const;

    private:
      std::string name;
      std::unordered_map<std::string, std::shared_ptr<Command>> commands;
      std::unordered_map<std::string, std::shared_ptr<Option>> handles;
      bool is_case_sensitive;
      bool is_bsd_opt_enabled;
      bool is_merged_opt_enabled;
      bool is_error_unknown_enabled;
  };

  /**
   * \class parse_error
   * \brief thrown during parsing
   */
  class parse_error: std::exception {
    public:
      parse_error(std::string&& msg): data(std::move(msg)) {}

      const char * what() const noexcept override {
        return data.c_str();
      }

    private:
      std::string data;
  };

  /**
   * \class option_language_error
   * \brief thrown when options declared
   */
  class option_language_error: std::exception {
    public:
      option_language_error(std::string&& msg): data(std::move(msg)) {}

      const char * what() const noexcept override {
        return data.c_str();
      }

    private:
      std::string data;
  };

  /**
   * \class command_error
   * \brief thrown during command parsing
   */
  class command_error: std::exception {
    public:
      command_error(std::string&& msg): data(std::move(msg)) {}

      const char * what() const noexcept override {
        return data.c_str();
      }

    private:
      std::string data;
  };
}

#endif
