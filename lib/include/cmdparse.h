/**
 * \file cmdparse.h
 * 
 * \author Adam Marshall (ih8celery)
 *
 * \brief interface to parse command line arguments/options
 */
#ifndef _MOD_CPP_COMMAND_PARSE

#define _MOD_CPP_COMMAND_PARSE

#include "option.h"
#include "info.h"

#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <exception>
#include <memory>
#include <tuple>

namespace cli {
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
       * \fn Command(const std::string&*)
       * \brief create a Command object using string argument as name
       */
      Command(const std::string&);

      /**
       * \fn std::shared_ptr<Option> option(const std::string&*, const std::string&* = "")
       * \brief declare an option to the parser
       *
       * option name is the second argument to the function <br>
       * see doc/option/spec.md for full description of options. <br>
       * throws an option_language_error if something goes wrong <br>
       */
      std::shared_ptr<Option> option(const std::string&, const std::string& = "");

      /**
       * \fn std::shared_ptr<Command> command(const std::string&*)
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
      Info operator()(char**, int);

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
       * \fn void configure(const std::string&*)
       * \brief toggle boolean private members of Command class
       */
      void configure(const std::string&);

      /**
       * \fn bool handle_has_name(const std::string&*, const std::string&*)
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

      const char* what() const noexcept override {
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
