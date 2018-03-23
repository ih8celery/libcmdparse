/**
 * \file 80-subcommands.cpp
 * \author Adam Marshall
 * \brief test the use of subcommands in libcmdparse
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

using namespace TAP;
using namespace util;

int main() {
  opt_parser p;

  p.set(config_constants::sub);

  plan(6);

  auto opt = p.option("[&]start");
  
  ok(opt.name == "start", "option named 'start'");
  ok(opt.mod  == Mod_Prop::SUB, "'start' is subcommand");

  opt = p.option("[&]edit", "Edit");

  ok(opt.name == "Edit", "option named 'Edit'");
  ok(opt.mod  == Mod_Prop::SUB, "'Edit' is subcommand");
  
  char ** argv = new char*[3];
  argv[0] = (char *)("edit");
  argv[1] = (char *)("start");
  argv[2] = (char *)("edit");

  auto info = p.parse(argv, 1);

  ok(info.has("Edit"), "subcommand found");

  argv++;
  TRY_NOT_OK(p.parse(argv, 2), "cannot use subcommand after first command line argument");
  argv--;

  delete [] argv;

  done_testing();

  return exit_status();
}
