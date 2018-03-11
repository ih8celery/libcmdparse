/**
 * \file 60-merged-option.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test the use of merged options
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

using namespace TAP;
using namespace util;

int main() {
  opt_parser p;

  p.set(config_constants::merged_opt);

  p.option("d|-due?", "due");
  p.option("a", "awe");
  p.option("--afd", "AFD");
  auto opt = p.option("f*", "file");

  plan(6);

  char ** argv = new char*[6];

  argv[0] = (char *)"--afd";
  argv[1] = (char *)"f";
  argv[2] = (char *)":d";
  argv[3] = (char *)"-due";
  argv[4] = (char *)"--afd";
  argv[5] = (char *)"--afd";

  opt_info info = p.parse(argv, 2);

  ok(info.has("file"), "found file argument");
  ok(info.count("file") == 2, "file argument twice");
  ok(info.count("awe") == 1, "found awe argument once");
  ok(info.count("due") == 1, "found due argument once");

  argv += 2;
  TRY_NOT_OK(p.parse(argv, 2), "merged option may not be repeated excessively");

  argv += 2;
  info = p.parse(argv, 2);

  ok(info.count("AFD"), "option named AFD found once");

  argv -= 4;

  delete [] argv;

  done_testing();

  return exit_status();
}
