#ifndef _MOD_CPP_OPTIONS_PARSING

#define _MOD_CPP_OPTIONS_PARSING

#include <map>
#include <set>
#include <string>
#include <vector>
#include <exception>

namespace util {
  /* 
   * the type of the data structure used to store data collected
   * from parsing argv
   */
  using opt_data_t = std::multimap<std::string, std::string>;

  namespace config_constants {
    struct case_sensitive_t {};
    struct bsd_opt_t {};
    struct merged_opt_t {};
    struct error_if_unknown_t {};
    struct subcommand_t {};

    const case_sensitive_t case_sensitive = case_sensitive_t();
    const bsd_opt_t bsd_opt = bsd_opt_t();
    const merged_opt_t merged_opt = merged_opt_t();
    const error_if_unknown_t error_unknown = error_if_unknown_t();
    const subcommand_t sub = subcommand_t();
  }

  enum class Mod_Prop { NONE, BEFORE, AFTER, SUB, NOT_WITH };
  enum class Num_Prop { ZERO_ONE, ZERO_MANY };
  enum class Assign_Prop { NO_ASSIGN, EQ_REQUIRED, EQ_MAYBE, EQ_NEVER };
  enum class Collect_Prop { SCALAR, LIST };
  enum class Data_Prop { STRING, INTEGER, FLOAT };

  struct option_t {
    public:
      option_t() : mod(Mod_Prop::NONE),
                   number(Num_Prop::ZERO_ONE),
                   assignment(Assign_Prop::NO_ASSIGN),
                   collection(Collect_Prop::SCALAR),
                   data_type(Data_Prop::STRING) {}

      Mod_Prop mod;
      std::string mod_arg;
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

      bool operator==(const option_t& opt) const {
        return (name == opt.name);
      }

      bool operator<(const option_t& opt) const {
        return (name < opt.name);
      }
  };

  class opt_info {
    friend class opt_parser;

    public:
      opt_info() = default;

      bool has(const std::string& name);
      size_t count(const std::string& name);
      const std::string arg(const std::string& name, const std::string& d = "");
      const std::pair<opt_data_t::const_iterator, opt_data_t::const_iterator>
        list(const std::string& name);

      std::vector<std::string> rem;
    private:
      opt_data_t opt_data;
  };

  class opt_parser {
    public:
      opt_parser() : is_case_sensitive(true),
                     is_bsd_opt_enabled(false),
                     is_merged_opt_enabled(false),
                     is_subcommand_enabled(false),
                     is_error_unknown_enabled(true),
                     is_mod_found(false) {}

      option_t option(const std::string&, const std::string& = "");

      /* 
       * configure opt_parser's case sensitivity when parsing ARGV.
       * case sensitivity of option names and stored handles is NOT affected
       *
       * default: true
       */
      void set(const config_constants::case_sensitive_t& c, bool val = true) {
        if (this->empty()) {
          is_case_sensitive = val;
        }
      }
      
      /* 
       * configure opt_parser's ability to detect bsd-style options in argv.
       * zero or more bsd-style options may then appear at the beginning of argv,
       * under the following conditions:
       *   1. option prefixes (-, --, +, etc) are not allowed in these options
       *   2. option handles must be exactly one letter long without a prefix
       *   3. options must be concatenated into one string, stored in argv[0]
       *   4. the first invalid or incomplete option causes parsing to fail
       *
       * default: false
       *
       * NOTE: "BSD-style" options conflict with subcommands, so enabling
       *   this setting implicitly disables subcommands
       */
      void set(const config_constants::bsd_opt_t& c, bool val = true) {
        if (this->empty()) {
          is_bsd_opt_enabled = val;
          
          if (val) {
            is_subcommand_enabled = false;
          }
        }
      }
      
      /*
       * configure opt_parser's ability to detect merged options in argv.
       * merged options are identical to bsd-style options except in one
       * respect: the first option detected in argv[0] must include its
       * prefix
       *
       * default: false
       *
       * NOTE: "merged" options conflict with subcommands, so enabling
       *   this setting implicitly disables subcommands
       */
      void set(const config_constants::merged_opt_t& c, bool val = true) {
        if (this->empty()) {
          is_merged_opt_enabled = val;

          if (val) {
            is_subcommand_enabled = false;
          }
        }
      }

      /*
       * configure opt_parser's response to a char* in argv that resembles
       * a valid option but is not known to the parser. true causes error
       * to be thrown
       *
       * default: true
       */
      void set(const config_constants::error_if_unknown_t& c, bool val = true) {
        if (this->empty()) {
          is_error_unknown_enabled = val;
        }
      }

      /*
       * configure opt_parser's ability to detect subcommands. opt_parser
       * may not support subcommands and merged options or bsd options
       * simultaneously. a subcommand may be any valid handle, but it must
       * appear in argv[0]. if subcommands are enabled, it is an error
       * NOT to provide one. subcommands may still be declared in the
       * option spec, but will be ignored if this setting is false.
       *
       * default: false
       *
       * NOTE: subcommands conflict with "BSD-style" and "merged" options,
       *   so enabling this setting implicitly disables those settings
       */
      void set(const config_constants::subcommand_t& c, bool val = true) {
        if (this->empty()) {
          is_subcommand_enabled = val;

          if (val) {
            is_merged_opt_enabled = false;
            is_bsd_opt_enabled    = false;
          }
        }
      }

      /*
       * tests whether the parser has any registered options
       */
      bool empty() const;

      std::pair<bool, std::string>
      get_opt(std::string spec, char ** &argv, int argc);

      std::pair<bool, std::vector<std::string>>
      get_opt_list(std::string spec, char ** &argv, int argc);

      opt_info parse(char ** &argv, int argc);

      void clear_options() {
        handle_map.clear();
        name_set.clear();
      }

      bool handle_has_name(const std::string& handle,
                           const std::string& name) {
        const std::map<std::string, option_t>::const_iterator
          iter = handle_map.find(handle);

        if (iter == handle_map.cend()) {
          return false;
        }
        else {
          return (name == iter->second.name);
        }
      }

    private:
      std::map<std::string, option_t> handle_map;
      std::set<option_t> name_set;
      bool is_case_sensitive;
      bool is_bsd_opt_enabled;
      bool is_merged_opt_enabled;
      bool is_error_unknown_enabled;
      bool is_subcommand_enabled;
      bool is_mod_found;
  };

  class parse_error : std::exception {
  public:
    parse_error(const std::string msg): data(msg) {}

    const char * what() const noexcept override {
      return data.c_str();
    }
  private:
    std::string data;
  };

  class option_language_error : std::exception {
  public:
    option_language_error(const std::string msg): data(msg) {}

    const char * what() const noexcept override {
      return data.c_str();
    }

  private:
    std::string data;
  };
}

#endif
