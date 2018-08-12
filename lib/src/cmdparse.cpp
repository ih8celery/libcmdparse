/**
 * \file cmdparse.cpp
 *
 * \author Adam Marshall (ih8celery)
 *
 * \brief parse command line arguments/options
 */
#include "cmdparse.h"
#include <sstream>
#include <cctype>
#include <iostream>

namespace cli {
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
            throw cli::parse_error(std::string("invalid prefix"));
          }
        case PLUS:
          if (in[i] == '+') {
            state = END;
            break;
          }
          else {
            throw cli::parse_error(std::string("invalid prefix"));
          }
        case END:
          throw cli::parse_error(std::string("invalid prefix"));
        }
      }

      return 0;
    }
    
    enum class DT_STATE {
      START,
      LDIGIT,
      RDIGIT,
      DOT,
      END
    };

    bool verify_arg_type(const std::string& arg, Property::Arg_Type prop) {
      DT_STATE state = DT_STATE::START;

      if (arg.empty()) {
        return false;
      }

      switch (prop) {
      case Property::Arg_Type::STRING:
        break;
      case Property::Arg_Type::INTEGER:
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
      case Property::Arg_Type::FLOAT:
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

  Command::Command(): is_case_sensitive(true),
                      is_bsd_opt_enabled(false),
                      is_merged_opt_enabled(false),
                      is_error_unknown_enabled(true),
                      name(std::string("")) {}

  Command::Command(const std::string& name): is_case_sensitive(true),
                                             is_bsd_opt_enabled(false),
                                             is_merged_opt_enabled(false),
                                             is_error_unknown_enabled(true),
                                             name(name) {}

  bool Command::empty() const noexcept {
    return this->handles.empty();
  }

  void Command::clear() {
    this->handles.clear();
    this->commands.clear();
  }

  std::shared_ptr<Command> Command::command(const std::string& spec) {
    if (spec == std::string("")) {
      throw command_error("subcommand may not be named after the empty string");
    }
    else {
      auto cmd = std::make_shared<Command>(spec);

      commands.insert(std::make_pair(spec, cmd));

      return cmd;
    }
  }

  std::shared_ptr<Option> Command::option(const std::string& spec, const std::string& name) {
    enum class Option_State {
      HANDLES, EQ, ARG,
      ARGLIST, ARGLIST_END, DONE,
      NAME, PREFIX_END, PLUS_PREFIX,
      MINUS_PREFIX, NUMBER
    };

    Option_State state = Option_State::HANDLES;
    auto opt = std::make_shared<Option>();
    std::vector<std::string> handle_vec;
    int index = 0;
    std::ostringstream buf;

    /* 
     * this loop processes the spec and name into an Option object,
     * which is returned at the end of the switch case/DONE
     */
    while (true) {
      switch(state) {
      case Option_State::HANDLES:
        if (index >= spec.size()) {
          state = Option_State::DONE;
        }

        switch (spec[index]) {
        case '/':
        case '.':
        case ':':
          state = Option_State::PREFIX_END;

          break;
        case '+':
          state = Option_State::PLUS_PREFIX;

          break;
        case '-':
          state = Option_State::MINUS_PREFIX;

          break;
        default:
          if (is_name_start_char(spec[index])) {
            state = Option_State::NAME;
          }
          else {
            throw option_language_error(std::string("expected prefix or word character"));
          }

          break;
        }

        buf << (spec[index]);

        break;
      case Option_State::MINUS_PREFIX:
        if (index >= spec.size()) {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        if (spec[index] == '-') {
          state = Option_State::PREFIX_END;
        }
        else if (is_name_start_char(spec[index])) {
          state = Option_State::NAME;
        }
        else {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        buf << (spec[index]);

        break;
      case Option_State::PLUS_PREFIX:
        if (index >= spec.size()) {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        if (spec[index] == '+') {
          state = Option_State::PREFIX_END;
        }
        else if (is_name_start_char(spec[index])) {
          state = Option_State::NAME;
        }
        else {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        break;
      case Option_State::PREFIX_END:
        if (index >= spec.size()) {
          throw option_language_error(std::string("input ended before handle complete"));
        }

        if (is_name_start_char(spec[index])) {
          buf << (spec[index]);

          state = Option_State::NAME;
        }
        else {
          throw option_language_error(std::to_string(spec[index])
            + std::string(" invalid character for handle name: can only take word characters and '-'"));
        }

        break;
      case Option_State::NAME:
        if (index >= spec.size()) {
          if (!buf.str().empty()) {
            handle_vec.push_back(buf.str());
            buf.str("");
          }

          state = Option_State::DONE;
          break;
        }

        switch (spec[index]) {
        case '|':
          handle_vec.push_back(buf.str());
          buf.str("");

          state = Option_State::HANDLES;
          break;
        case '=':
          handle_vec.push_back(buf.str());
          buf.str("");

          opt->assignment = Property::Assignment::EQ_REQUIRED;
          opt->collection = Property::Collection::SCALAR;

          state = Option_State::EQ;
          break;
        case '?':
          handle_vec.push_back(buf.str());
          buf.str("");
    
          state = Option_State::NUMBER;
    
          opt->number = Property::Number::ZERO_ONE;
          break;
        case '*':
          handle_vec.push_back(buf.str());
          buf.str("");

          state = Option_State::NUMBER;

          opt->number = Property::Number::ZERO_MANY;
          break;
        default:
          if (is_name_rest_char(spec[index])) {
            buf << (spec[index]);
          }
          else {
            throw option_language_error(std::string("invalid character for handle name: can only take word characters and '-'"));
          }
          break;
        }

        break;
      case Option_State::NUMBER:
        if (index >= spec.size()) {
          state = Option_State::DONE;
          break;
        }

        handle_vec.push_back(buf.str());
        buf.str("");

        if (spec[index] == '=') {
          opt->assignment = Property::Assignment::EQ_REQUIRED;
          opt->collection = Property::Collection::SCALAR;

          state = Option_State::EQ;
        }
        else if (spec[index] == '[') {
          opt->assignment = Property::Assignment::NO_ASSIGN;
          opt->collection = Property::Collection::LIST;

          state = Option_State::ARGLIST;
        }
        else {
          throw option_language_error(std::string("expected '=' or '[' after number"));
        }

        break;
      case Option_State::EQ:
        if (index >= spec.size()) {
          state = Option_State::DONE;
          break;
        }

        switch (spec[index]) {
          case '|':
            opt->assignment = Property::Assignment::STUCK;
            state = Option_State::ARG;

            break;
          case '?':
            opt->assignment = Property::Assignment::EQ_MAYBE;
            state = Option_State::ARG;

            break;
          case '!':
            opt->assignment = Property::Assignment::EQ_NEVER;
            state = Option_State::ARG;

            break;
          case '[':
            state = Option_State::ARGLIST;
            opt->collection = Property::Collection::LIST;
            opt->assignment = Property::Assignment::EQ_REQUIRED;

            break;
          case 's':
            opt->type = Property::Arg_Type::STRING;
            state = Option_State::DONE;

            break;
          case 'i':
            opt->type = Property::Arg_Type::INTEGER;
            state = Option_State::DONE;

            break;
          case 'f':
            opt->type = Property::Arg_Type::FLOAT;
            state = Option_State::DONE;

            break;
          default:
            throw option_language_error(std::string(
                        "expected eq modifier or arg spec"));
            break;
        }

        break;
      case Option_State::ARG:
        if (index >= spec.size()) {
          opt->type = Property::Arg_Type::STRING;
          state = Option_State::DONE;

          break;
        }

        switch (spec[index]) {
          case '[':
            state = Option_State::ARGLIST;
            opt->collection = Property::Collection::LIST;

            break;
          case 's':
            opt->type = Property::Arg_Type::STRING;
            state = Option_State::DONE;

            break;
          case 'i':
            opt->type = Property::Arg_Type::INTEGER;
            state = Option_State::DONE;

            break;
          case 'f':
            opt->type = Property::Arg_Type::FLOAT;
            state = Option_State::DONE;

            break;
          default:
          throw option_language_error(std::string(
                        "expected arg type or start of arg list"));
            break;
        }

        break;
      case Option_State::ARGLIST:
        if (index >= spec.size()) {
          throw option_language_error(std::string(
                      "input ended in arg list"));
        }

        switch (spec[index]) {
          case 's':
            opt->type = Property::Arg_Type::STRING;
            state = Option_State::ARGLIST_END;

            break;
          case 'i':
            opt->type = Property::Arg_Type::INTEGER;
            state = Option_State::ARGLIST_END;

            break;
          case 'f':
            opt->type = Property::Arg_Type::FLOAT;
            state = Option_State::ARGLIST_END;

            break;
          case ']':
            opt->type = Property::Arg_Type::STRING;
            state = Option_State::DONE;

            break;
          default:
            throw option_language_error(std::string(
                        "expected arg type or end of arg list"));
            break;
        }

        break;
      case Option_State::ARGLIST_END:
        if (index >= spec.size()) {
          throw option_language_error(std::string(
                      "input ended before arg list finished"));
        }

        if (spec[index] == ']') {
          state = Option_State::DONE;
        }
        else {
          throw option_language_error(std::string(
                      "expected ']' to conclude arg list"));
        }

        break;
      case Option_State::DONE:
        if (index < spec.size()) {
          throw option_language_error(std::string("input found after option spec parsed"));
        }
        else {
          if (handle_vec.empty()) {
            throw option_language_error(std::string("no handles found in option spec"));
          }

          if (name.empty()) {
            std::string& maybe_name    = handle_vec.back();
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
              opt->name = handle_vec.back().substr(ind);
            }
          }
          else {
            opt->name = name;
          }

          if (opt->assignment == Property::Assignment::STUCK) {
            // no more than one handle
            if (handle_vec.size() > 1) {
              throw option_language_error(std::string("option ")
                  + "declared with STUCK assignment cannot have "
                  + "multiple handles");
            }

            // handle must be of form /-[A-Z]/
            if (!(handle_vec[0].size() == 2 && handle_vec[0][0] == '-' && isupper(handle_vec[0][1]))) {
              throw option_language_error(std::string("option declared ")
                  + "with STUCK assignment must have single handle "
                  + "of form /-[A-Z]/");
            }
          }

            // insert handles known with the option
            for (const std::string handle : handle_vec) {
              if (this->handles.find(handle) == this->handles.cend()) {
                this->handles.insert(std::make_pair(handle, opt));
              }
              else {
                throw option_language_error(std::string("handle repeated: ") + handle);
              }
            }
        }

        return opt;
      }

      if (index < 0) {
        index = 0;
      }
      else if (index < spec.size()){
        index++;
      }
    }
  }
    
  Info Command::parse(char ** argv, int argc, Info * d) const {
    Info info;
    Info * infop = (d == nullptr) ? &info : d;
    std::shared_ptr<Option> opt;
    std::string args("");
    int index = 0;

    if (index > argc - 1) {
      return *infop;
    }

    // try to get this command's name unless it is empty string
    if (!this->name.empty()) {
      if (this->name == argv[index]) {
        argv[index++] = (char*)"";
        infop->commands.insert(this->name);
      }
      else {
        throw parse_error(std::string("command not found"));
      }
    }

    /* BLOCK: delegate to command if this command owns any */
    if (!commands.empty()) {
      auto cmd_iter = commands.find(argv[index]);
      if (cmd_iter == commands.cend()) {
        throw parse_error(std::string("initial argument does not match any command"));
      }
      else {
        cmd_iter->second->parse(argv + index, argc - index, infop);
      }
    }

    /* BLOCK: parse the rest of the args not handled by sub commands */
    for (; index < argc; ++index) {
      std::string handle = argv[index];

      if (handle.empty()) continue;

      if (handle == "-" || handle == "--") {
        argv[index] = (char*)"";

        ++index;
        while (index < argc) {
          infop->rest.emplace_back(argv[index]);
          argv[index++] = (char*)"";
        }

        return *infop;
      }

      auto eq_loc = handle.find_first_of('=');
      std::unordered_map<std::string, std::shared_ptr<Option>>::const_iterator iter;

      /* BLOCK: get the option.
       * when no '=' is present, the entire string is presumed to be an
       * option. otherwise, the string is split on the '=' and the first
       * substring is presumed to be an option
       */
      if (eq_loc == std::string::npos) {
        if (this->name == "" && index == 0
            && (is_bsd_opt_enabled || is_merged_opt_enabled)) {

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

            iter = this->handles.find(std::string(1, mini_handle));
            opt  = iter->second;

            if (iter == this->handles.cend()) {
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
              if (opt->assignment == Property::Assignment::NO_ASSIGN) {
                // option repeated too many times
                if (opt->number == Property::Number::ZERO_ONE
                    && infop->data.find(opt->name) != infop->data.cend()) {

                  throw parse_error(std::string("option repeated more than allowed"));
                }

                infop->data.insert(std::make_pair(opt->name, args));
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

          iter = this->handles.find(handle);
        }
        else {
          if (is_case_sensitive) {
            if (handle.size() >= 2 && handle[0] == '-' && isupper(handle[1])) {
              iter = this->handles.find(handle.substr(0, 2));

              if (iter == this->handles.cend()) {
                iter = this->handles.find(handle);
              }
            }
            else {
              iter = this->handles.find(handle);
            }
          }
          else {
            iter = this->handles.find(strtolower(handle));
          }
        }
      }
      else {
        if (index == 0 && (is_bsd_opt_enabled || is_merged_opt_enabled)) {
          throw parse_error(std::string("special options may not take arguments"));
        }
        
        if (is_case_sensitive) {
          iter = this->handles.find(handle.substr(0, eq_loc));
        }
        else {
          iter = this->handles.find(strtolower(handle.substr(0, eq_loc)));
        }
      }
 
      /* BLOCK: decide what to do with potential option.
       * if not found in handles, probably an error.
       * otherwise, verify properties
       */
      if (iter == this->handles.cend()) {
        if (is_prefix_char(handle[0]) && is_error_unknown_enabled) {
          if (this->name.empty()) {
            throw parse_error(std::string("unknown option with handle: ") + handle);
          }
        }
        else {
          infop->rest.push_back(handle);
          continue;
        }
      }
      else {
        opt = iter->second;

        // compare option requirements with data and insert into map
        if (opt->number == Property::Number::ZERO_ONE
            && infop->data.find(opt->name) != infop->data.cend()) {

          throw parse_error(std::string("no-repeat option with handle '")
              + handle + "' found more than once");
        }
        else {
          switch (opt->assignment) {
          case Property::Assignment::NO_ASSIGN:
            if (eq_loc == std::string::npos) {
              infop->data.insert(std::make_pair(opt->name, args));
            }
            else {
              throw parse_error(std::string("option with handle '")
                  + handle + "' should not have an argument");
            }

            break;
          case Property::Assignment::EQ_REQUIRED:
            if (eq_loc == std::string::npos) {
              throw parse_error(std::string("option with handle '")
                  + handle + "' is missing equals sign");
            }
            else {
              args = handle.substr(eq_loc+1);
            }

            break;
          case Property::Assignment::EQ_MAYBE:
            if (eq_loc == std::string::npos) {
              if (++index < argc) {
                args = argv[index];  
              }
              else {
                throw parse_error(std::string("option with handle '")
                    + handle + "' missing an argument");
              }
            }
            else {
              args = handle.substr(eq_loc+1);
            }

            break;
          case Property::Assignment::EQ_NEVER:
            if (eq_loc == std::string::npos) {
              if (++index < argc) {
                args = argv[index];
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

            break;
          default:
            if (handle.size() < 3) {
              throw parse_error(std::string("option declared with ")
                  + "stuck assignment must have an argument");
            }

            args = handle.substr(2);

            break;
          }

          if (opt->assignment == Property::Assignment::NO_ASSIGN) {
            continue;
          }

          if (opt->collection == Property::Collection::SCALAR) {
            if (infop->data.find(opt->name) == infop->data.cend()) {
              if (verify_arg_type(args, opt->type)) {
                infop->data.insert(std::make_pair(opt->name, args));
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
              if (verify_arg_type(data, opt->type)) {
                infop->data.insert(std::make_pair(opt->name, data));
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
  
    return *infop;
  }

  Info Command::operator()(char ** argv, int argc) {
    return parse(argv, argc, nullptr);
  }

  void Command::configure(const std::string& spec) {
    if (!this->name.empty()) {
      throw command_error(std::string("special options cannot be enabled on named commands"));
    }

    if (spec == std::string("ignore_case")) {
      this->is_case_sensitive = false;
    }
    else if (spec == std::string("no_ignore_case")) {
      this->is_case_sensitive = true;
    }
    else if (spec == std::string("bsd_opt")) {
      this->is_bsd_opt_enabled = true;
    }
    else if (spec == std::string("no_bsd_opt")) {
      this->is_bsd_opt_enabled = false;
    }
    else if (spec == std::string("merged_opt")) {
      this->is_merged_opt_enabled = true;
    }
    else if (spec == std::string("no_merged_opt")) {
      this->is_merged_opt_enabled = false;
    }
    else {
      throw command_error("unrecognized configuration directive");
    }
  }

  bool Command::handle_has_name(const std::string& handle, const std::string& name) const {
    const std::unordered_map<std::string, std::shared_ptr<Option>>::const_iterator
      iter = this->handles.find(handle);

    if (iter == this->handles.cend()) {
      return false;
    }
    else {
      return (name == iter->second->name);
    }
  }
}
