/**
 * \file options_parsing.cpp
 *
 * \author Adam Marshall (ih8celery)
 *
 * \brief parse command line arguments/options
 */
#include "options_parsing.h"
#include <sstream>
#include <cctype>
#include <iostream>

namespace {
  inline bool is_prefix_char(char ch) {
    return (ch == ':' || ch == '.' || ch == '-'
              || ch == '+' || ch == '/');
  }

  inline bool is_name_start_char(char ch) {
    return (isalpha(ch) || isdigit(ch) || ch == '_');
  }

  inline bool is_name_rest_char(char ch) {
    return (is_name_start_char(ch) || ch == '-');
  }

  std::string strtolower(const std::string& str) {
    std::string result(str);

    for (char& ch : result) {
      ch = tolower(ch);
    }

    return result;
  }

  int skip_prefix(const std::string& in) {
    enum Prefix_State { NONE, MINUS, PLUS, END } state = NONE;

    for (int i = 0; i < in.size(); ++i) {
      if (!is_prefix_char(in[i])) {
        return i;
      }

      switch (state) {
      case NONE:
        if (in[i] == '-') {
          state = MINUS;
          break;
        }
        else if (in[i] == '+') {
          state = PLUS;
          break;
        }
        else {
          state = END;
          break;
        }
      case MINUS:
        if (in[i] == '-') {
          state = END;
          break;
        }
        else {
          throw util::parse_error(std::string("invalid prefix"));
        }
      case PLUS:
        if (in[i] == '+') {
          state = END;
          break;
        }
        else {
          throw util::parse_error(std::string("invalid prefix"));
        }
      case END:
        throw util::parse_error(std::string("invalid prefix"));
      }
    }

    return 0;
  }
}

namespace util {
  option_t::option_t() : mod(Mod_Prop::NONE),
                         number(Num_Prop::ZERO_ONE),
                         assignment(Assign_Prop::NO_ASSIGN),
                         collection(Collect_Prop::SCALAR),
                         data_type(Data_Prop::STRING) {}

  std::string opt_info::arg(const std::string& name, const std::string& d) const {
    opt_data_t::const_iterator data = opt_data.find(name);

    if (data == opt_data.cend()) {
      return d;
    }

    return data->second;
  }

  RangePair opt_info::list(const std::string& name) const {
    const std::pair<opt_data_t::const_iterator, opt_data_t::const_iterator>
      rng = opt_data.equal_range(name);

    return rng;
  }

  size_t opt_info::count(const std::string& name) const {
    return opt_data.count(name);
  }

  bool opt_info::has(const std::string& name) const {
    return (opt_data.find(name) != opt_data.cend());
  }

  /*
   * definitions of opt_parser's helpers
   */
  namespace {
    enum class DT_STATE {
      START,
      LDIGIT,
      RDIGIT,
      DOT,
      END
    };

    enum class Option_State { MOD, HANDLES, EQ, ARG, ARGLIST,
                              ARGLIST_END, DONE };

    bool verify_arg_type(const std::string& arg, Data_Prop prop) {
      DT_STATE state = DT_STATE::START;

      switch (prop) {
      case Data_Prop::STRING:
        break;
      case Data_Prop::INTEGER:
        for (auto i = std::begin(arg); i != std::end(arg); ++i) {
          switch (state) {
            case DT_STATE::START:
              if (i + 1 == std::end(arg) && !isdigit(*i)) {
                return false;
              }
              else if (isdigit(*i)) {
                state = DT_STATE::LDIGIT;
              }
              else if (!isspace(*i)) {
                return false;
              }

              break;
            case DT_STATE::LDIGIT:
              if (isspace(*i)) {
                state = DT_STATE::END;
              }
              else if (!isdigit(*i)) {
                return false;
              }

              break;
            case DT_STATE::DOT:
            case DT_STATE::RDIGIT:
            case DT_STATE::END:
              if (!isspace(*i)) {
                return false;
              }

              break;
          }
        }
      case Data_Prop::FLOAT:
        for (auto i = std::begin(arg); i != std::end(arg); ++i) {
          switch (state) {
            case DT_STATE::START:
              if (i + 1 == std::end(arg) && !isdigit(*i)) {
                return false;
              }
              else if (isdigit(*i)) {
                state = DT_STATE::LDIGIT;
              }
              else if (!isspace(*i)) {
                return false;
              }

              break;
            case DT_STATE::LDIGIT:
              if (isspace(*i)) {
                state = DT_STATE::END;
              }
              else if (*i == '.') {
                state = DT_STATE::DOT;
              }
              else if (!isdigit(*i)) {
                return false;
              }

              break;
            case DT_STATE::DOT:
              if (i + 1 == std::end(arg) && !isdigit(*i)) {
                return false;
              }
              else if (isdigit(*i)) {
                state = DT_STATE::RDIGIT;
              }
              else {
                return false;
              }

              break;
            case DT_STATE::RDIGIT:
              if (isspace(*i)) {
                state = DT_STATE::END;
              }
              else if (!isdigit(*i)) {
                return false;
              }

              break;
            case DT_STATE::END:
              if (!isspace(*i)) {
                return false;
              }

              break;
          }
        }
      }

      return true;
    }
   }

  opt_parser::opt_parser() : is_case_sensitive(true),
                             is_bsd_opt_enabled(false),
                             is_merged_opt_enabled(false),
                             is_subcommand_enabled(false),
                             is_error_unknown_enabled(true),
                             is_mod_found(false) {}

  option_t opt_parser::option(const char * spec, const std::string& name) {
    Option_State state = Option_State::MOD;
    option_t opt;
    std::vector<std::string> handles;
    int spec_offset = 0;

    /* 
     * this loop processes the spec and name into an option_t object,
     * which is returned at the end of the switch case/DONE
     */
    while (true) {
      /* execute code on the state of the option parser */
      switch(state) {
      case Option_State::MOD:
        state = process_MOD(spec, spec_offset, opt);

        break;
      case Option_State::HANDLES:
        state = process_HANDLES(spec, spec_offset, opt, handles);

        break;
      case Option_State::EQ:
        if (spec[spec_offset] == '\0') {
          state = Option_State::DONE;

          break;
        }

        switch (spec[spec_offset]) {
          case '|':
            opt.assignment = Assign_Prop::STUCK;
            state = Option_State::ARG;

            break;
          case '?':
            opt.assignment = Assign_Prop::EQ_MAYBE;
            state = Option_State::ARG;

            break;
          case '!':
            opt.assignment = Assign_Prop::EQ_NEVER;
            state = Option_State::ARG;

            break;
          case '[':
            state = Option_State::ARGLIST;
            opt.collection = Collect_Prop::LIST;
            opt.assignment = Assign_Prop::EQ_REQUIRED;

            break;
          case 's':
            opt.data_type = Data_Prop::STRING;
            state = Option_State::DONE;

            break;
          case 'i':
            opt.data_type = Data_Prop::INTEGER;
            state = Option_State::DONE;

            break;
          case 'f':
            opt.data_type = Data_Prop::FLOAT;
            state = Option_State::DONE;

            break;
          default:
          throw option_language_error(std::string(
                        "expected eq modifier or arg spec"));
            break;
        }

        break;
      case Option_State::ARG:
        if (spec[spec_offset] == '\0') {
          opt.data_type = Data_Prop::STRING;
          state = Option_State::DONE;

          break;
        }

        switch (spec[spec_offset]) {
          case '[':
            state = Option_State::ARGLIST;
            opt.collection = Collect_Prop::LIST;

            break;
          case 's':
            opt.data_type = Data_Prop::STRING;
            state = Option_State::DONE;

            break;
          case 'i':
            opt.data_type = Data_Prop::INTEGER;
            state = Option_State::DONE;

            break;
          case 'f':
            opt.data_type = Data_Prop::FLOAT;
            state = Option_State::DONE;

            break;
          default:
          throw option_language_error(std::string(
                        "expected arg type or start of arg list"));
            break;
        }

        break;
      case Option_State::ARGLIST:
        if (spec[spec_offset] == '\0') {
        throw option_language_error(std::string(
                      "input ended in arg list"));
        }

        switch (spec[spec_offset]) {
          case 's':
            opt.data_type = Data_Prop::STRING;
            state = Option_State::ARGLIST_END;

            break;
          case 'i':
            opt.data_type = Data_Prop::INTEGER;
            state = Option_State::ARGLIST_END;

            break;
          case 'f':
            opt.data_type = Data_Prop::FLOAT;
            state = Option_State::ARGLIST_END;

            break;
          case ']':
            opt.data_type = Data_Prop::STRING;
            state = Option_State::DONE;

            break;
          default:
          throw option_language_error(std::string(
                        "expected arg type or end of arg list"));
            break;
        }

        break;
      case Option_State::ARGLIST_END:
        if (spec[spec_offset] == '\0') {
        throw option_language_error(std::string(
                      "input ended before arg list finished"));
        }

        if (spec[spec_offset] == ']') {
          state = Option_State::DONE;
        }
        else {
        throw option_language_error(std::string(
                      "expected ']' to conclude arg list"));
        }

        break;
      case Option_State::DONE:
        if (spec[spec_offset] != '\0') {
          throw option_language_error(std::string("input found after option spec parsed"));
        }
        else {
          if (handles.empty()) {
            throw option_language_error(std::string("no handles found in option spec"));
          }

          if (name.empty()) {
            std::string& maybe_name = handles.back();
            std::string::size_type ind = 0;
            
            if (is_name_start_char(maybe_name[0])) {
              ind = 0;
            }
            else if (is_name_start_char(maybe_name[1])) {
              ind = 1;
            }
            else {
              ind = 2;
            }
          
            if (maybe_name.size() == ind + 1) { // NOTE: this situation should never happen
              throw option_language_error(std::string("handle minus prefix is the empty string"));
            }
            else {
              opt.name = handles.back().substr(ind);
            }
          }
          else {
            opt.name = name;
          }

          /*
           * if name already exists, do not insert name -> option_t
           * and throw if existing option_t is not equal to *opt.
           */
          const std::set<option_t>::const_iterator
            name_iter = name_set.find(opt);

          if (name_iter == name_set.cend()) {
            if (opt.assignment == Assign_Prop::STUCK) {
              // no more than one handle
              if (handles.size() > 1) {
                throw option_language_error(std::string("option ")
                    + "declared with STUCK assignment cannot have "
                    + "multiple handles");
              }

              // handle must be of form /-[A-Z]/
              if (!(handles[0].size() == 2 && handles[0][0] == '-' && isupper(handles[0][1]))) {
                throw option_language_error(std::string("option declared ")
                    + "with STUCK assignment must have single handle "
                    + "of form /-[A-Z]/");
              }
            }

            // when name is not found, this option is being registered as new
            name_set.insert(opt);

            // insert handles known with the option
            for (const std::string handle : handles) {
              if (handle_map.find(handle) == handle_map.cend()) {
                handle_map.insert(std::make_pair(handle, opt));
              }
              else {
                throw option_language_error(std::string("handle repeated: ") + handle);
              }
            }
          }
          else { // found the name
            /* may not declare multiple instances of a stuck argument */
            if (opt.assignment == Assign_Prop::STUCK) {
              throw option_language_error(std::string("option declared ")
                  + "with STUCK assignment cannot have multiple handles");
            }

            if (opt.compatible(*name_iter)) {
              // insert handles known with the option
              for (const std::string handle : handles) {
                if (handle_map.find(handle) == handle_map.cend()) {
                  handle_map.insert(std::make_pair(handle, opt));
                }
                else {
                  throw option_language_error(std::string("handle repeated: ") + handle);
                }
              }
            }
            else {
              throw option_language_error(std::string("options with same name must be compatible: ") + opt.name);
            }
          }
        }

        return opt;
      }

      if (spec_offset < 0) {
        spec_offset = 0;
      }
      else if (spec[spec_offset] != '\0'){
        spec_offset++;
      }
    }
  }
    
  Option_State opt_parser::process_MOD(const char * spec, int& spec_offset, option_t& opt) {
    enum Mod_State { MOD_START, MOD_FN, MOD_ARG, MOD_END }
      state = MOD_START;

    while (true) {
      switch(state) {
      case MOD_START:
        if (spec[spec_offset] == '\0') {
          throw option_language_error(std::string("input ended before parsing finished"));
        }

        if (spec[spec_offset] == '[') {
          state = MOD_FN;
        }
        else {
          spec_offset--;

          return Option_State::HANDLES;
        }

        break;
      case MOD_FN:
        if (spec[spec_offset] == '\0') {
          throw option_language_error(std::string("input ended before parsing finished"));
        }

        switch (spec[spec_offset]) {
          case '&':
            /* 
             * declaring an option as subcommand has no effect if setting not enabled
             */
            if (is_subcommand_enabled) {
              opt.mod = Mod_Prop::SUB;

              is_mod_found = true;
            }
            state = MOD_END;

            break;
          case '<':
            throw option_language_error(std::string("unimplemented modifier"));
            state = MOD_ARG;
            opt.mod = Mod_Prop::BEFORE;
            break;
          case '>':
            throw option_language_error(std::string("unimplemented modifier"));
            state = MOD_ARG;
            opt.mod = Mod_Prop::AFTER;
            break;
          case '!':
            throw option_language_error(std::string("unimplemented modifier"));
            state = MOD_ARG;
            opt.mod = Mod_Prop::NOT_WITH;
            break;
        }

        break;
      case MOD_ARG:
      case MOD_END:
        if (spec[spec_offset] == '\0') {
          throw option_language_error(std::string("input ended before parsing finished"));
        }

        if (spec[spec_offset] == ']') {
          return Option_State::HANDLES;
        }
        else {
          throw option_language_error(std::string("expected ']' character"));
        }

        break;
      }

      if (spec[spec_offset] == '\0') {
        throw option_language_error(std::string("input finished before parsing finished"));
      }
      else {
        spec_offset++;
      }
    }
  }

  Option_State opt_parser::process_HANDLES(const char * spec,
                               int& spec_offset,
                               option_t& opt,
                               std::vector<std::string>& handles) {

    std::ostringstream buf;
    enum Handle_State { NAME, PREFIX_END, PLUS_PREFIX, MINUS_PREFIX,
                        HANDLES, NUMBER } state = HANDLES;

    while (true) {
      switch(state) {
      case HANDLES:
        if (spec[spec_offset] == '\0') {
          return Option_State::DONE;
        }

        switch (spec[spec_offset]) {
        case '/':
        case '.':
        case ':':
          state = PREFIX_END;

          break;
        case '+':
          state = PLUS_PREFIX;

          break;
        case '-':
          state = MINUS_PREFIX;

          break;
        default:
          if (is_name_start_char(spec[spec_offset])) {
            state = NAME;
          }
          else {
            throw option_language_error(std::string("expected prefix or word character"));
          }

          break;
        }

        buf << (spec[spec_offset]);

        break;
      case MINUS_PREFIX:
        if (spec[spec_offset] == '\0') {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        if (spec[spec_offset] == '-') {
          state = PREFIX_END;
        }
        else if (is_name_start_char(spec[spec_offset])) {
          state = NAME;
        }
        else {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        buf << (spec[spec_offset]);

        break;
      case PLUS_PREFIX:
        if (spec[spec_offset] == '\0') {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        if (spec[spec_offset] == '+') {
          state = PREFIX_END;
        }
        else if (is_name_start_char(spec[spec_offset])) {
          state = NAME;
        }
        else {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        break;
      case PREFIX_END:
        if (spec[spec_offset] == '\0') {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        if (is_name_start_char(spec[spec_offset])) {
          buf << (spec[spec_offset]);

          state = NAME;
        }
        else {
          throw option_language_error(std::to_string(spec[spec_offset])
            + std::string(" invalid character for handle name: can only take word characters and '-'"));
        }

        break;
      case NAME:
        if (spec[spec_offset] == '\0') {
          if (!buf.str().empty()) {
            handles.push_back(buf.str());
            buf.str("");
          }

          return Option_State::DONE;
        }

        switch (spec[spec_offset]) {
        case '|':
          handles.push_back(buf.str());
          buf.str("");

          state = HANDLES;
          break;
        case '=':
          handles.push_back(buf.str());
          buf.str("");

          opt.assignment = Assign_Prop::EQ_REQUIRED;
          opt.collection = Collect_Prop::SCALAR;

          return Option_State::EQ;
        case '?':
          handles.push_back(buf.str());
          buf.str("");
    
          state = NUMBER;
    
          opt.number = Num_Prop::ZERO_ONE;
          break;
        case '*':
          handles.push_back(buf.str());
          buf.str("");

          state = NUMBER;

          opt.number = Num_Prop::ZERO_MANY;
          break;
        default:
          if (is_name_rest_char(spec[spec_offset])) {
            buf << (spec[spec_offset]);
          }
          else {
            throw option_language_error(std::string("invalid character for handle name: can only take word characters and '-'"));
          }
          break;
        }

        break;
      case NUMBER:
        if (spec[spec_offset] == '\0') {
          return Option_State::DONE;
        }

        handles.push_back(buf.str());
        buf.str("");

        if (spec[spec_offset] == '=') {
          opt.assignment = Assign_Prop::EQ_REQUIRED;
          opt.collection = Collect_Prop::SCALAR;

          return Option_State::EQ;
        }
        else if (spec[spec_offset] == '[') {
          opt.assignment = Assign_Prop::NO_ASSIGN;
          opt.collection = Collect_Prop::LIST;

          return Option_State::ARGLIST;
        }
        else {
          throw option_language_error(std::string("expected '=' or '[' after number"));
        }

        break;
      }
      
      if (spec[spec_offset] != '\0') {
        spec_offset++;
      }
    }
  }

  opt_info opt_parser::parse(char ** &argv, int argc) {
    opt_info info;
    std::string::size_type eq_loc;
    option_t opt;
    std::string args("1");

    /** 
     * this is the only option_language_error thrown in the parse
     * member function because calling this function is the only way
     * to signify the end of option declarations. as stated in a
     * previous comment, it is an error for subcommands to be enabled
     * but not declared by calls to option()
     */
    if (is_subcommand_enabled && !is_mod_found) {
      throw option_language_error(std::string("no subcommands declared"));
    }

    /* loop over all the words in argv */
    for (int i = 0; i < argc; ++i) {
      std::string handle = argv[i];
      eq_loc = handle.find_first_of('=');
      std::unordered_map<std::string, option_t>::const_iterator iter;

      // get the handle
      if (eq_loc == std::string::npos) {
        if (i == 0 && (is_bsd_opt_enabled || is_merged_opt_enabled)) {
          bool accepted_first_special = false;

          char mini_handle;

          for (int j = 0; j < handle.size(); ++j) {
            // eliminate any prefix characters
            if (j == 0) {
              j = skip_prefix(handle);
              
              if (is_bsd_opt_enabled && j > 0) {
                throw parse_error(std::string("bsd-style options may ")
                    + "not use a prefix");
              }
            }

            /*
             * test this char; if it is not a handle, error
             * if it is a handle, record it
             */
            if (is_case_sensitive) {
              mini_handle = handle[j];
            }
            else {
              mini_handle = tolower(handle[j]);
            }

            iter = handle_map.find(std::string(1, mini_handle));
            opt  = iter->second;

            if (iter == handle_map.cend()) {
              if (accepted_first_special) {
                throw parse_error(std::string("all or none of the")
                    + " characters in the first argument must special");
              }
              else {
                // abandon processing if the first char is not special
                break; 
              }
            }
            else {
              if (opt.assignment == Assign_Prop::NO_ASSIGN) {
                // option repeated too many times
                if (opt.number == Num_Prop::ZERO_ONE
                    && info.opt_data.find(opt.name) != info.opt_data.cend()) {

                  throw parse_error(std::string("option repeated more than allowed"));
                }

                info.opt_data.insert(std::make_pair(opt.name, args));
                accepted_first_special = true;
              }
              else {
                throw parse_error(std::string("cannot assign to a bsd ")
                    + "or merged option");
              }
            }
          }

          if (accepted_first_special)
            continue;

          iter = handle_map.find(handle);
        }
        else {
          if (is_case_sensitive) {
            if (handle.size() >= 2 && handle[0] == '-' && isupper(handle[1])) {
              iter = handle_map.find(handle.substr(0, 2));

              if (iter == handle_map.cend()) {
                iter = handle_map.find(handle);
              }
            }
            else {
              iter = handle_map.find(handle);
            }
          }
          else {
            iter = handle_map.find(strtolower(handle));
          }
        }
      }
      else {
        if (i == 0 && (is_subcommand_enabled
            || is_bsd_opt_enabled
            || is_merged_opt_enabled)) {
        
          throw parse_error(std::string("special options may not take arguments"));
        }
        
        if (is_case_sensitive) {
          iter = handle_map.find(handle.substr(0, eq_loc));
        }
        else {
          iter = handle_map.find(strtolower(handle.substr(0, eq_loc)));
        }
      }
 
      // handle is unknown, so it is either a malformed option or a non-option
      if (iter == handle_map.cend()) {
        if (is_prefix_char(handle[0]) && is_error_unknown_enabled) {
          throw parse_error(std::string("unknown option with handle: ") + handle); 
        }
        else {
          info.rest.push_back(handle);
          continue;
        }
      }
      // handle seems to be an option
      else {
        opt = iter->second;

        if (i != 0 && opt.mod == Mod_Prop::SUB) {
          throw parse_error(std::string("subcommand found after first argument"));
        }

        // compare option requirements with data and insert into opt_data mmap
        if (opt.number == Num_Prop::ZERO_ONE
            && info.opt_data.find(opt.name) != info.opt_data.cend()) {
          throw parse_error(std::string("no-repeat option with handle '")
              + handle + "' found more than once");
        }
        else {
          if (opt.assignment == Assign_Prop::NO_ASSIGN) {
            if (eq_loc == std::string::npos) {
              info.opt_data.insert(std::make_pair(opt.name, args));
            }
            else {
              throw parse_error(std::string("option with handle '")
                  + handle + "' should not have an argument");
            }

            continue;
          }
          else if (opt.assignment == Assign_Prop::EQ_REQUIRED) {
            if (eq_loc == std::string::npos) {
              throw parse_error(std::string("option with handle '")
                  + handle + "' is missing equals sign");
            }
            else {
              args = handle.substr(eq_loc+1);
            }
          }
          else if (opt.assignment == Assign_Prop::EQ_MAYBE) {
            if (eq_loc == std::string::npos) {
              if (++i < argc) {
                args = argv[i];  
              }
              else {
                throw parse_error(std::string("option with handle '")
                    + handle + "' missing an argument");
              }
            }
            else {
              args = handle.substr(eq_loc+1);
            }
          }
          else if (opt.assignment == Assign_Prop::EQ_NEVER) {
            if (eq_loc == std::string::npos) {
              if (++i < argc) {
                args = argv[i];
              }
              else {
                throw parse_error(std::string("option with handle '")
                    + handle + "' missing an argument");
              }
            }
            else {
              throw parse_error(std::string("option with handle '")
                  + handle + "' should not use an equals sign");
            }
          }
          else {
            if (handle.size() < 3) {
              throw parse_error(std::string("option declared with ")
                  + "stuck assignment must have an argument");
            }

            args = handle.substr(2);
          }

          if (opt.collection == Collect_Prop::SCALAR) {
              if (info.opt_data.find(opt.name) == info.opt_data.cend()) {
                if (verify_arg_type(args, opt.data_type)) {
                  info.opt_data.insert(std::make_pair(opt.name, args));
                }
                else {
                  throw parse_error(std::string("data '")
                      + args + "' does not match its declared type");
                }
              }
              else {
                throw parse_error(std::string("handle repeated: ") + handle);
              }
          }
          else {
            std::istringstream src(args);
            std::string data;

            while (std::getline(src, data, ',')) {
              if (verify_arg_type(data, opt.data_type)) {
                info.opt_data.insert(std::make_pair(opt.name, data));
              }
              else {
                throw parse_error(std::string("data '")
                    + data + "' does not match declared type");
              }
            }
          }
        }
      }
    }

    return info;
  }
}
