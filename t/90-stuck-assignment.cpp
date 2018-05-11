/**
 * \file 90-stuck-assignment.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test the declaration and use of options with STUCK assignment
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "cmdparse.h"

using namespace TAP;
using namespace cli;

constexpr std::size_t ARGC = 2;

int main() {
  std::shared_ptr<Option> opt;
  Command cmd;
  Info info;

  plan(9);

  opt = cmd.option("-S=|s", "stuck");
  ok(opt->assignment == Property::Assignment::STUCK, "stuck has STUCK assignment");
  ok(opt->type == Property::Arg_Type::STRING, "stuck has data type STRING");

  opt = cmd.option("-W=|[s]", "stucklist");
  ok(opt->assignment == Property::Assignment::STUCK, "stucklist has STUCK assignment");

  TRY_NOT_OK(cmd.option("-Ab=|i", "forore"), "cannot create multiletter STUCK option");
  TRY_NOT_OK(cmd.option("-c=|s", "clan"), "cannot create lowercase handle to STUCK option");
  TRY_NOT_OK(cmd.option("+G=|f", "gooday"), "STUCK option must use '-' as prefix to its handle");

  char ** argv = new char*[ARGC];
  argv[0] = (char*)"-Similar";
  argv[1] = (char*)"-Wall,some,none";

  info = cmd.parse(argv, 2);

  ok(info.has("stuck"), "found 'stuck' in argv");
  ok(info.get("stuck").second == "imilar", "found 'stuck' argument");
  ok(info.count("stucklist") == 3, "found list of stuck arguments");

  delete [] argv;

  done_testing();

  return exit_status();
}
