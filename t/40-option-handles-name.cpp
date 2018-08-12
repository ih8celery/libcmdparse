/**
 * \file 40-option-handles-names.cpp
 *
 * \author Adam Marshall (ih8celery)
 * 
 * \brief test the creation of handles and option names
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "cmdparse.h"

constexpr int ARGC = 8;

using namespace TAP;

int main () {
  plan(11);

  cli::Command cmd;

  cmd.configure("ignore_case");

  // definition of option/name
  cmd.option("--has-opt*", "name");
  ok(cmd.handle_has_name("--has-opt", "name"),
      "a handle is a string used to identify the option to use in parsing");

  // extended definition of option/name 
  cmd.option("-repeat*", "name");
  ok(cmd.handle_has_name("-repeat", "name"),
      "option may be specified in multiple calls so long as name is same");

  TRY_NOT_OK(cmd.option("-repeat", "other"),
      "repeated handles are not allowed");

  TRY_NOT_OK(cmd.option("-!!"), "handles must contain 'word' characters");
  note("handles may begin with digits, letters, or underscores.");
  note("after the beginning, handles may also contain hyphens");

  TRY_NOT_OK(cmd.option("---a"), "after prefix, handle must begin with [a-zA-Z0-9_]");

  // option may have multiple handles attached to it at once
  cmd.option("--is-opt|--is-option|-NAME");
  ok(cmd.handle_has_name("--is-opt", "NAME"), "multiple handles separated by '|'");
  ok(cmd.handle_has_name("--is-option", "NAME"), "multiple handles separated by '|'");
  ok(cmd.handle_has_name("-NAME", "NAME"), "multiple handles separated by '|'");

  char ** args = new char*[ARGC];
  args[0] = (char*)"data";
  args[1] = (char*)"--is-option";
  args[2] = (char*)"--has-opt";
  args[3] = (char*)"-repeat";
  args[4] = (char*)"-REPEAT";
  args[5] = (char*)"-";
  args[6] = (char*)"-repeat";
  args[7] = (char*)"--is-option";

  cli::Info info = cmd.parse(args, ARGC);

  ok(info.has("NAME"), "--is-option handle used");

  ok(info.count("name") == 3, "option with name 'name' used thrice");

  ok(info.rest.size() == 3, "EOI detected");

  delete [] args;

  done_testing();

  return exit_status();
}
