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
  p.option("a", "awe");
  p.option("f*", "file");

  char ** argv = new char*;

  argv[0] = (char *)"afd";

  opt_info info;
  
  try {
    info = p.parse(argv, 1);
  }
  catch (const parse_error& e) {
    std::cout << e.what() << std::endl;

    return 0;
  }

  delete argv;

  plan(3);

  ok(info.has("file"), "found file argument");
  ok(info.count("awe") == 1, "found awe argument once");
  ok(info.count("due") == 1, "found due argument once");

  done_testing();

  return exit_status();
}
