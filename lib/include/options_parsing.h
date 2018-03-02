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
    struct case_sensitive_t;
    struct bsd_opt_t;
    struct merged_opt_t;

    extern const case_sensitive_t case_sensitive;
    extern const bsd_opt_t bsd_opt;
    extern const merged_opt_t merged_opt;
  }

  enum class Num_Prop { ZERO_ONE, ZERO_MANY };
  enum class Assign_Prop { NO_ASSIGN, EQ_REQUIRED, EQ_MAYBE, EQ_NEVER };
  enum class Collect_Prop { SCALAR, LIST };
  enum class Data_Prop { STRING, INTEGER, FLOAT };

  struct option_t {
    public:
      option_t() : number(Num_Prop::ZERO_ONE),
                   assignment(Assign_Prop::NO_ASSIGN),
                   collection(Collect_Prop::SCALAR),
                   data_type(Data_Prop::STRING) {}

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
      option_t option(const std::string&, const std::string& = "");

      void set(const config_constants::case_sensitive_t&, bool = true);
      void set(const config_constants::bsd_opt_t&, bool = true);
      void set(const config_constants::merged_opt_t&, bool = true);

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
