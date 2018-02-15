#define WANT_TEST_EXTRAS

#include "include/options_parsing.h"
#include <tap++/tap++.h>

using namespace util;

int main(int argc, char ** argv) {
  TEST_START(3);

  TRY_DECL(option_parser parser, "create a default initialized option_parser");

  TRY_DECL(option_info info = parser.parse(argv, argc), "parse argv without any registered options");

  is(info.rem.size(), argc, "all arguments should be passed to rem");
  
  // TODO add calls to parse, etc

  END_TESTING;

  return 0;  
}
