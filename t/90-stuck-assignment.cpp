/**
 * \file 90-stuck-assignment.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test the declaration and use of options with STUCK assignment
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

using namespace TAP;
using namespace util;

constexpr std::size_t ARGC = 2;

int main() {
  option_t opt;
  opt_parser p;
  opt_info info;

  plan(10);

  opt = p.option("-S=|s", "stuck");
  ok(opt.assignment == Assign_Prop::STUCK, "stuck has STUCK assignment");
  ok(opt.data_type == Data_Prop::STRING, "stuck has data type STRING");

  opt = p.option("-W=|[s]", "stucklist");
  ok(opt.assignment == Assign_Prop::STUCK, "stucklist has STUCK assignment");

  TRY_NOT_OK(p.option("-Ab=|i", "forore"), "cannot create multiletter STUCK option");
  TRY_NOT_OK(p.option("-c=|s", "clan"), "cannot create lowercase handle to STUCK option");
  TRY_NOT_OK(p.option("+G=|f", "gooday"), "STUCK option must use '-' as prefix to its handle");
  TRY_NOT_OK(p.option("-L=|s", "stuck"), "may not declare more than one handle to STUCK option");

  char ** argv = new char*[ARGC];
  argv[0] = (char*)"-Similar";
  argv[1] = (char*)"-Wall,some,none";

  info = p.parse(argv, 2);

  ok(info.has("stuck"), "found 'stuck' in argv");
  ok(info.arg("stuck", "none") == "imilar", "found 'stuck' argument");
  ok(info.count("stucklist") == 3, "found list of stuck arguments");

  delete [] argv;

  done_testing();

  return exit_status();
}
