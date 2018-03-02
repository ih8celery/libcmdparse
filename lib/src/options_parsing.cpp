#include "options_parsing.h"
#include <iostream>
#include <sstream>
#include <cctype>

namespace util {
  namespace config_constants {
    struct case_sensitive_t {};
    struct bsd_opt_t {};
    struct merged_opt_t {};

    const case_sensitive_t case_sensitive = case_sensitive_t();
    const bsd_opt_t bsd_opt = bsd_opt_t();
    const merged_opt_t merged_opt = merged_opt_t();
  }

  /*
   * retrieve an option argument, returning d if the option
   * not found
   */
  const std::string opt_info::arg(const std::string& name, const std::string& d) {
    opt_data_t::const_iterator data = opt_data.find(name);

    if (data == opt_data.cend()) {
      return d;
    }

    return data->second;
  }

  /*
   * return a range of iterators denoting the values attached to an option 
   */
  const std::pair<opt_data_t::const_iterator, opt_data_t::const_iterator>
  opt_info::list(const std::string& name) {
    
    const std::pair<opt_data_t::const_iterator,
                    opt_data_t::const_iterator>
      rng = opt_data.equal_range(name);

    return rng;
  }

  /* 
   * count the occurrences of an option. except for count and list options, 
   * the count will be 1 if option is present and 0 if not
   */
  size_t opt_info::count(const std::string& name) {
    return opt_data.count(name);
  }

  /* predicate to check whether an option is present in the result of parsing */
  bool opt_info::has(const std::string& name) {
    return (opt_data.find(name) != opt_data.cend());
  }

  /* 
   * configure opt_parser's case sensitivity when parsing ARGV.
   * case sensitivity of option names and stored handles is NOT affected 
   */
  void opt_parser::set(const config_constants::case_sensitive_t& c, bool val) {
    is_case_sensitive = val;
  }

  /* 
   * configure opt_parser's ability to detect bsd-style options in argv.
   * zero or more bsd-style options may then appear at the beginning of argv,
   * under the following conditions:
   *   1. option prefixes (-, --, +, etc) are not allowed in these options
   *   2. option handles must be exactly one letter long without a prefix
   *   3. options must be concatenated into one string, stored in argv[0]
   *   4. the first invalid or incomplete option causes parsing to fail
   */
  void opt_parser::set(const config_constants::bsd_opt_t& c, bool val) {
    is_bsd_opt_enabled = val;
  }

  /*
   * configure opt_parser's ability to detect merged options in argv.
   * merged options are identical to bsd-style options except in one
   * respect: the first option detected in argv[0] must include its
   * prefix
   */
  void opt_parser::set(const config_constants::merged_opt_t& c, bool val) {
    is_merged_opt_enabled = val;
  }

  /*
   * declare an option to the parser where option name is
   * the second argument to the function
   * see doc/option/spec.md and doc/option/properties.md for full 
   * description of options
   *
   * throws an option_language_error if something goes wrong
   */
   option_t opt_parser::option(const std::string& spec, const std::string& name) {
    enum Option_State { NONE, END_PREFIX, PLUS_PREFIX, MINUS_PREFIX, NAME,
                        NUMBER, EQ, ARG, ARGLIST, ARGLIST_END, DONE } state = NONE;

    option_t opt;
    std::vector<std::string> handles;
    std::ostringstream buf;

    bool has_input = true;
    std::string::size_type index = 0;
    char ch;
    while (true) {
      if (index >= spec.size()) {
        has_input = false;
      }
      else {
        ch = spec[index];
      }
      index++;

      if (state == NONE) {
        if (!has_input) {
          state = DONE;

          continue;
        }

        switch (ch) {
          case '/':
          case '.':
          case ':':
            state = END_PREFIX;

            break;
          case '+':
            state = PLUS_PREFIX;

            break;
          case '-':
            state = MINUS_PREFIX;

            break;
          default:
            if (isalpha(ch) || isdigit(ch) || ch == '_') {
              state = NAME;
            }
            else {
              throw option_language_error(std::string("expected prefix or word character"));
            }

            break;
        }

        buf << ch;

        continue;
      }
      if (state == MINUS_PREFIX) {
        if (!has_input) {
	  throw option_language_error(std::string("input ended before handle complete"));
        }

        if (ch == '-') {
          state = END_PREFIX;
        }
        else if (isalpha(ch) || isdigit(ch) || ch == '_') {
          state = NAME;
        }
        else {
	  throw option_language_error(std::string("input ended before handle complete"));
        }

        buf << ch;

        continue;
      }
      if (state == PLUS_PREFIX) {
        if (!has_input) {
	  throw option_language_error(std::string("input ended before handle complete"));
        }

        if (ch == '+') {
          state = END_PREFIX;
        }
        else if (isdigit(ch) || isalpha(ch) || ch == '_') {
          state = NAME;
        }
        else {
	  throw option_language_error(std::string("input ended before handle complete"));
        }

        continue;
      }
      if (state == END_PREFIX) {
        if (!has_input) {
	  throw option_language_error(std::string("input ended before hande complete"));
        }

        if (isdigit(ch) || isalpha(ch) || ch == '_') {
          buf << ch;

          state = NAME;
        }
        else {
	  throw option_language_error(std::to_string(ch) + std::string(" invalid character for handle name: can only take word characters and '-'"));
        }

        continue;
      }
      if (state == NAME) {
        if (!has_input) {
          if (!buf.str().empty()) {
            handles.push_back(buf.str());
            buf.str("");
          }

          state = DONE;

          continue;
        }

        if (ch == '|') { // reached the end of a handle
          handles.push_back(buf.str());
          buf.str("");

          state = NONE;
        }
        else if (ch == '=') {
          handles.push_back(buf.str());
          buf.str("");

          state = EQ;

          opt.assignment = Assign_Prop::EQ_REQUIRED;
          opt.collection = Collect_Prop::SCALAR;
        }
	else if (ch == '?') {
	  handles.push_back(buf.str());
	  buf.str("");
	  
	  state = NUMBER;
	  
	  opt.number = Num_Prop::ZERO_ONE;
	}
	else if (ch == '*') {
	  handles.push_back(buf.str());
	  buf.str("");

	  state = NUMBER;

	  opt.number = Num_Prop::ZERO_MANY;
	}	
        else if (isdigit(ch) || isalpha(ch) || ch == '_' || ch == '-') {
          buf << ch;
        }
        else {
	  throw option_language_error(std::string("invalid character for handle name: can only take word characters and '-'"));
        }

        continue;
      }
      if (state == NUMBER) {
        if (!has_input) {
          state = DONE;

          continue;
        }

        handles.push_back(buf.str());
        buf.str("");

        if (ch == '=') {
          state = EQ;

          opt.assignment = Assign_Prop::EQ_REQUIRED;
          opt.collection = Collect_Prop::SCALAR;
        }
        else if (ch == '[') {
          state = ARGLIST;

          opt.assignment = Assign_Prop::NO_ASSIGN;
          opt.collection = Collect_Prop::LIST;
        }
        else {
          throw option_language_error(std::string("expected '=' or '[' after number"));
        }

        continue;
      }
      if (state == EQ) {
        if (!has_input) {
          state = DONE;

          continue;
        }

        switch (ch) {
          case '?':
            opt.assignment = Assign_Prop::EQ_MAYBE;
            state = ARG;

            break;
          case '!':
            opt.assignment = Assign_Prop::EQ_NEVER;
            state = ARG;

            break;
          case '[':
            state = ARGLIST;
            opt.collection = Collect_Prop::LIST;
            opt.assignment = Assign_Prop::EQ_REQUIRED;

            break;
          case 's':
            opt.data_type = Data_Prop::STRING;
            state = DONE;

            break;
          case 'i':
            opt.data_type = Data_Prop::INTEGER;
            state = DONE;

            break;
          case 'f':
            opt.data_type = Data_Prop::FLOAT;
            state = DONE;

            break;
          default:
	    throw option_language_error(std::string("expected eq modifier or arg spec"));
            break;
        }

        continue;
      }
      if (state == ARG) {
        if (!has_input) {
          opt.data_type = Data_Prop::STRING;
          state = DONE;

          continue;
        }

        switch (ch) {
          case '[':
            state = ARGLIST;
            opt.collection = Collect_Prop::LIST;

            break;
          case 's':
            opt.data_type = Data_Prop::STRING;
            state = DONE;

            break;
          case 'i':
            opt.data_type = Data_Prop::INTEGER;
            state = DONE;

            break;
          case 'f':
            opt.data_type = Data_Prop::FLOAT;
            state = DONE;

            break;
          default:
	    throw option_language_error(std::string("expected arg type or start of arg list"));
            break;
        }

        continue;
      }
      if (state == ARGLIST) {
        if (!has_input) {
	  throw option_language_error(std::string("input ended in arg list"));
        }

        switch (ch) {
          case 's':
            opt.data_type = Data_Prop::STRING;
            state = ARGLIST_END;

            break;
          case 'i':
            opt.data_type = Data_Prop::INTEGER;
            state = ARGLIST_END;

            break;
          case 'f':
            opt.data_type = Data_Prop::FLOAT;
            state = ARGLIST_END;

            break;
          case ']':
            opt.data_type = Data_Prop::STRING; // string is default data type of option arguments
            state = DONE;

            break;
          default:
	    throw option_language_error(std::string("expected arg type or end of arg list"));
            break;
        }

        continue;
      }
      if (state == ARGLIST_END) {
        if (!has_input) {
	  throw option_language_error(std::string("input ended before arg list finished"));
        }

        if (ch == ']') {
          state = DONE;
        }
        else {
	  throw option_language_error(std::string("expected ']' to conclude arg list"));
        }

        continue;
      }
      if (state == DONE) {
        if (has_input) {
	  throw option_language_error(std::string("input found after option spec parsed"));
        }
        else {
          if (handles.empty()) {
	    throw option_language_error(std::string("no handles found in option spec"));
          }

          if (name.empty()) {
	    std::string& maybe_name = handles.back();
	    std::string::size_type ind = 0;
            if (isalpha(maybe_name[0]) || isdigit(maybe_name[0]) || maybe_name[0] == '_') {
	      ind = 0;
	    }
	    else if (isalpha(maybe_name[1]) || isdigit(maybe_name[1]) || maybe_name[1] == '_') {
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

          if (name_iter == name_set.cend()) { // name not found
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

        break;
      }
    }

    return opt;
  }

  /*
   * extract options and non-options from argv into opt_info
   * object. any options or data belonging to options is removed from
   * argv and replaced with nullptr TODO(and discard opt_info::rem)
   *
   * throws a parse_error if something goes wrong
   */
  opt_info opt_parser::parse(char ** &argv, int argc) {
    opt_info info;
    std::string::size_type eq_loc;
    option_t opt;
    std::string args;
    const std::string default_data = "1"; // TODO move to higher scope
    std::istringstream strim;

    for (int i = 0; i < argc; ++i) { // loop over all the words in argument list
      std::string handle = argv[i];
      eq_loc = handle.find_first_of('=');
      std::map<std::string, option_t>::const_iterator iter;

      // get the handle
      if (eq_loc == std::string::npos) { // no equals sign, so entire word is handle
        iter = handle_map.find(handle);
      }
      else { // found equals sign, so only the substring up to the equals sign 
        iter = handle_map.find(handle.substr(0, eq_loc));
      }
 
      // handle is unknown, so it is either a malformed option or a non-option
      if (iter == handle_map.cend()) {
        // check for a prefix
        if (handle[0] == '-' || handle[0] == '+' || handle[0] == ':' || handle[0] == '.' || handle[0] == '/') { // malformed option
          throw parse_error(std::string("unknown option with handle: ") + handle); 
        }
        else { // non-option
          info.rem.push_back(handle);
          continue;
        }
      }
      else { // found handle of a known option
        opt = iter->second;

	// compare option requirements with data and insert into opt_data mmap
        if (opt.number == Num_Prop::ZERO_ONE && info.opt_data.find(opt.name) != info.opt_data.cend()) {
          throw parse_error(std::string("no-repeat option with handle '") + handle + "' found more than once");
        }
        else {
          if (opt.assignment == Assign_Prop::NO_ASSIGN) {
            if (eq_loc == std::string::npos) {
              info.opt_data.insert(std::make_pair(opt.name, default_data));
              continue;
            }
            else {
              throw parse_error(std::string("option with handle '") + handle + "' should not have an argument");
            }
          }
          else if (opt.assignment == Assign_Prop::EQ_REQUIRED) {
            if (eq_loc == std::string::npos) {
              throw parse_error(std::string("option with handle '") + handle + "' is missing equals sign");
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
                throw parse_error(std::string("option with handle '") + handle + "' missing an argument");
              }
            }
            else {
              args = handle.substr(eq_loc+1);
            }
          }
          else {
            if (eq_loc == std::string::npos) {
              if (++i < argc) {
                args = argv[i];
              }
              else {
                throw parse_error(std::string("option with handle '") + handle + "' missing an argument");
              }
            }
            else {
              throw parse_error(std::string("option with handle '") + handle + "' should not use an equals sign");
            }
          }

	  bool parse_failed(false);
          enum { START, PRE, DOT, POST } state = START;
          if (opt.collection == Collect_Prop::SCALAR) {
              if (info.opt_data.find(opt.name) == info.opt_data.cend()) {
                if (opt.data_type == Data_Prop::STRING); 

                else if (opt.data_type == Data_Prop::INTEGER) {
                  for (const char& ch : args) {
                    if (!isdigit(ch)) {
                      parse_failed = true;
                      break;
                    }
                  }

                  if (parse_failed) {
                    throw parse_error(std::string("data '") + args + "' is not an integer argument");
                  }
                }
                else {
                  for (const char& ch : args) {
                    if (isdigit(ch) && state == START) {
                      state = PRE;
                    }
                    else if (isdigit(ch) && state == DOT) {
                      state = POST;
                    }
                    else if (ch == '.' && state == PRE) {
                      state = DOT;
                    }
                    else if (isdigit(ch) && state == POST) {
                      continue;
                    }
                    else {
                      break;
                    }
                  }

                  if (state != PRE && state != POST) {
                    throw parse_error(std::string("data '") + args + "' is not a float argument");
                  }
                }

                info.opt_data.insert(std::make_pair(opt.name, args));
              }
              else {
                throw parse_error(std::string("handle repeated: ") + handle);
              }
          }
          else {
            std::istringstream src(args);
            std::string data;

            while (std::getline(src, data, ',')) {
              if (opt.data_type == Data_Prop::STRING);
              else if (opt.data_type == Data_Prop::INTEGER) {
		for (const char& ch : data) {
                  if (!isdigit(ch)) {
                    parse_failed = true;
                    break;
                  }
                }

                if (parse_failed) {
                  throw parse_error(std::string("data '") + args + "' is not an integer argument");
                }
              }
              else {
                for (const char& ch : data) {
                  if (isdigit(ch) && state == START) {
                    state = PRE;
                  }
                  else if (isdigit(ch) && state == DOT) {
                    state = POST;
                  }
                  else if (ch == '.' && state == PRE) {
                    state = DOT;
                  }
                  else if (isdigit(ch) && state == POST) {
                    continue;
                  }
                  else {
                    break;
                  }
                }

                if (state != PRE && state != POST) {
                  throw parse_error(std::string("data '") + data + "' is not a float argument");
                }
              }

              info.opt_data.insert(std::make_pair(opt.name, data));
            }
          }
        }
      }
    }

    return info;
  }
}
