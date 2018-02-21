#define WANT_TEST_EXTRAS

#include "options_parsing.h"
#include <tap++.h>

using namespace util;
using namespace TAP;

int main(int argc, char ** argv) {
  TEST_START(1);

  TRY_DECL(option_parser parser, "create a default initialized option_parser");

  TEST_END;

  return exit_status();  
}
