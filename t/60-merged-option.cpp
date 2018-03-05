/*
 * @file 60-merged-option.cpp
 * @author Adam Marshall (ih8celery)
 * @brief test the use of merged options
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

using namespace TAP;
using namespace util;

int main() {
  opt_parser p;

  p.set(config_constants::merged_opt);

  p.option("d?", "due");
  p.option("a", "awe");
  auto opt = p.option("f*", "file");

  char ** argv = new char*[2];

  argv[0] = (char *)"--afd";
  argv[1] = (char *)"f";

  opt_info info;
  
  try {
    info = p.parse(argv, 2);
  }
  catch (const parse_error& e) {
    std::cout << e.what() << std::endl;

    return 0;
  }

  delete [] argv;

  plan(4);

  ok(info.has("file"), "found file argument");
  ok(info.count("file") == 2, "file argument twice");
  ok(info.count("awe") == 1, "found awe argument once");
  ok(info.count("due") == 1, "found due argument once");

  done_testing();

  return exit_status();
}
