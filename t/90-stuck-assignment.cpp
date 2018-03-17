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

int main() {
  // setup for test

  option_t opt;
  opt_parser p;
  opt_info info;

  plan(8);

  opt = p.option("-S=|s", "stuck");
  ok(opt.assignment == Assign_Prop::STUCK, "stuck has STUCK assignment");
  ok(opt.data_type == Data_Prop::STRING, "stuck has data type STRING");

  TRY_NOT_OK(p.option("-Ab=|i", "forore"), "cannot create multiletter STUCK option");
  TRY_NOT_OK(p.option("-c=|s", "clan"), "cannot create lowercase handle to STUCK option");
  TRY_NOT_OK(p.option("+G=|f", "gooday"), "STUCK option must use '-' as prefix to its handle");
  TRY_NOT_OK(p.option("-L=|s", "stuck"), "may not declare more than one handle to STUCK option");

  char ** argv = new char*[1];
  argv[0] = (char *)"-Similar";

  info = p.parse(argv, 1);

  ok(info.has("stuck"), "found 'stuck' in argv");
  ok(info.arg("stuck", "none") == "imilar", "found 'stuck' argument");

  delete [] argv;

  done_testing();

  return exit_status();
}
