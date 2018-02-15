#ifndef _MOD_CPP_OPTIONS_PARSING

#define _MOD_CPP_OPTIONS_PARSING

#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <exception>
#include <memory>

namespace util {
  using opt_data_t = std::multimap<std::string, std::string>;

  enum class Num_Prop { ZERO_ONE, ZERO_MANY };
  enum class Assign_Prop { NO_ASSIGN, EQ_REQUIRED, EQ_MAYBE, EQ_NEVER };
  enum class Collect_Prop { SCALAR, LIST };
  enum class Data_Prop { STRING, INTEGER, FLOAT };

  struct option_t {
    option_t(): number(Num_Prop::ZERO_ONE),
                assignment(Assign_Prop::NO_ASSIGN),
                collection(Collect_Prop::SCALAR),
                data_type(Data_Prop::STRING),
                name("") {}

    Num_Prop number;
    Assign_Prop assignment;
    Collect_Prop collection;
    Data_Prop data_type;

    std::string name;

    bool compatible(const option_t& opt) {
      return (number == opt.number
              && assignment == opt.assignment
              && data_type == opt.data_type
              && collection == opt.collection);
    }
    
    bool operator==(const option_t& opt) {
      return name == opt.name;
    }

    bool operator<(const option_t& opt) {
      return name < opt.name;
    }
  };

  class option_info {
    friend class option_parser;

  public:
    option_info() = default;

    // get argument for option
    // deft_arg is the default argument returned when no arg present
    const std::string& arg(const std::string& name, const std::string& deft_arg);

    // get argument list
    const std::pair<opt_data_t::const_iterator, opt_data_t::const_iterator> list(const std::string& name);

    // get number of occurrences of option
    size_t count(const std::string& name);

    // test for presence of option
    bool has(const std::string& name);

    // stores the strings from parsed input that could not be treated as options or
    // their arguments
    std::vector<std::string> rem;
  private:
    opt_data_t opt_data;
  };

  class option_parser {
  public:
    option_parser() = default;
    // TODO add a constructor for configuration constants

    // declare an option to the parser
    std::shared_ptr<option_t> option(const std::string& spec, const std::string& name = "");

    // erase all records of options created by calls to option
    void clear_options() { handle_map.clear(); name_set.clear(); }

    inline
    bool handle_has_name(const std::string& handle, const std::string& name) const noexcept {
      const std::map<std::string, std::shared_ptr<option_t>>::const_iterator iter = handle_map.find(handle);

      if (iter == handle_map.cend()) {
        return false;
      }

      return (name == iter->second->name);
    }

    // extract options into an option_info object
    // argv is modified during parsing
    option_info parse(char ** argv, const int& argc);
  private:
    std::map<std::string, std::shared_ptr<option_t>> handle_map;
    std::set<std::shared_ptr<option_t>> name_set;
  };

  class parse_error {
  public:
    parse_error(const std::string msg): data(msg) {}

    const char * what() const noexcept {
      return data.c_str();
    }
  private:
    std::string data;
  };

  class option_language_error {
  public:
    option_language_error(const std::string msg): data(msg) {}

    const char * what() const {
      return data.c_str();
    }

  private:
    std::string data;
  };
}

#endif
