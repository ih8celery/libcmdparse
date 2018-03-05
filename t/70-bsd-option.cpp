/*
 * @file 70-bsd-option.cpp
 * @author Adam Marshall (ih8celery)
 * @brief test the use of bsd options
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

using namespace TAP;
using namespace util;

int main() {
  opt_parser p;

  p.set(config_constants::bsd_opt);

  p.option("d?", "due");
  p.option("a|-awe", "awe");
  p.option("f*", "file");

  char ** argv = new char*[4];

  argv[0] = (char *)"afd";
  argv[1] = (char *)"f";
  argv[2] = (char *)"af";
  argv[3] = (char *)"-awe";

  plan(5);

  opt_info info = p.parse(argv, 2);

  ok(info.has("file"), "found file argument");
  ok(info.count("file") == 2, "found file argument twice");
  ok(info.count("awe") == 1, "found awe argument once");
  ok(info.count("due") == 1, "found due argument once");

  argv += 2;
  TRY_NOT_OK(p.parse(argv, 2), "bsd options may not be excessively repeated");
  argv -= 2;

  delete [] argv;

  done_testing();

  return exit_status();
}
