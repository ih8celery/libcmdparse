/**
 * \file 40-option-handles-names.cpp
 *
 * \author Adam Marshall (ih8celery)
 * 
 * \brief test the creation of handles and option names
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

constexpr int ARGC = 8;

using namespace TAP;

int main () {
  plan(13);

  util::opt_parser parser;
  util::option_t opt;

  parser.set(util::config_constants::case_sensitive, false);

  // definition of option/name
  opt = parser.option("--has-opt*", "name");
  ok(opt.name == "name", "the second argument to option is the name");
  ok(parser.handle_has_name("--has-opt", "name"),
      "a handle is a string used to identify the option to use in parsing");

  // extended definition of option/name 
  opt = parser.option("-repeat*", "name");
  ok(parser.handle_has_name("-repeat", "name"),
      "option may be specified in multiple calls so long as name is same");

  TRY_NOT_OK(parser.option("-repeat", "other"),
      "repeated handles are not allowed");

  TRY_NOT_OK(parser.option("-age=i", "name"),
      "options with same name must have compatible properties");

  TRY_NOT_OK(parser.option("-!!"), "handles must contain 'word' characters");
  note("handles may begin with digits, letters, or underscores.");
  note("after the beginning, handles may also contain hyphens");

  // option may have multiple handles attached to it at once
  opt = parser.option("--is-opt|--is-option|-NAME");
  ok(parser.handle_has_name("--is-opt", "NAME"), "multiple handles separated by '|'");
  ok(parser.handle_has_name("--is-option", "NAME"), "multiple handles separated by '|'");
  ok(parser.handle_has_name("-NAME", "NAME"), "multiple handles separated by '|'");
  ok(opt.name == "NAME", "when no name provided, option deduces name from last handle");

  char ** args = new char*[ARGC];
  args[0] = (char*)"data";
  args[1] = (char*)"--is-option";
  args[2] = (char*)"--has-opt";
  args[3] = (char*)"-repeat";
  args[4] = (char*)"-REPEAT";
  args[5] = (char*)"-";
  args[6] = (char*)"-repeat";
  args[7] = (char*)"--is-option";

  util::opt_info info = parser.parse(args, ARGC);

  ok(info.has("NAME"), "--is-option handle used");

  ok(info.count("name") == 3, "option with name 'name' used thrice");

  ok(info.rest.size() == 3, "EOI detected");

  delete [] args;

  done_testing();

  return exit_status();
}
