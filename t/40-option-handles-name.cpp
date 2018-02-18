/*
 * option-handles-names.cpp
 *
 * test the creation of handles to options and the
 * selection of option_t::name during calls to
 * option_parser::option; also test the parsing of
 * the options
 */

#define WANT_TEST_EXTRAS
#include "tap++/tap++.h"
#include "options_parsing.h"

#define ARGC 4

using namespace TAP;

int main () {
  plan(12);

  util::option_parser parser;
  std::shared_ptr<util::option_t> opt;

  // definition of option/name
  opt = parser.option("--has-opt*", "name");
  ok(opt->name == "name", "the second argument to option is the name");
  ok(parser.handle_has_name("--has-opt", "name"), "a handle is a string used to identify the option to use in parsing");

  // extended definition of option/name 
  opt = parser.option("-repeat*", "name");
  ok(opt->name == "name", "option may be specified in multiple calls so long as name is same"); // test name

  // TRY repeating handle
  TRY(parser.option("-repeat", "other"), "repeated handles are not allowed");

  // TRY incompatible options
  TRY(parser.option("-age=i", "name"), "options with same name must have compatible properties");

  // TRY non-word characters in handle name
  TRY(parser.option(), "handles must contain 'word' characters");
  note("handles may begin with digits, letters, or underscores.");
  note("after the beginning, handles may also contain hyphens");

  // option may have multiple handles attached to it at once
  opt = parser.option("--is-opt|--is-option|-NAME");
  ok(parser.handle_has_name("--is-opt", "NAME"), "multiple handles separated by '|'");
  ok(parser.handle_has_name("--is-option", "NAME"), "multiple handles separated by '|'");
  ok(parser.handle_has_name("-NAME", "NAME"), "multiple handles separated by '|'");
  ok(opt->name == "NAME", "when no name provided, option deduces name from last handle");

  char * args[] = { "data", "--is-option", "--has-opt", "-repeat" };
  auto info = parser.parse(args, ARGC);

  ok(info.has("NAME"), "--is-option handle used");

  ok(info.count("name") == 2, "option with name 'name' used twice");

  done_testing();

  return exit_status();
}
