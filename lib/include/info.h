/**
 * \file info.h
 * 
 * \author Adam Marshall (ih8celery)
 *
 */
#ifndef _MOD_CPP_COMMAND_PARSE_INFO

#define _MOD_CPP_COMMAND_PARSE_INFO

#include "option.h"
//#include "cmdparse.h"

#include <unordered_map>
#include <set>
#include <string>
#include <optional>
#include <vector>

namespace cli {
    using opt_data_t = std::unordered_multimap<std::string, std::string>;
  /**
   * \class Info
   * \brief represent data collected during parsing
   */
  class Info {
    friend class Command;

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
       * \fn optional<string> find(const string&)
       * \brief retrieve value of option
       */
      std::optional<std::string> find(const std::string&) const;
      std::optional<std::string> operator[] (const std::string&) const;
      
      /**
       * \fn optional<vector<string>> find_all(const string&)
       * \brief retrieve the vector of values under name
       */
      std::optional<std::vector<std::string>> find_all(const std::string&) const;

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
}

#endif
