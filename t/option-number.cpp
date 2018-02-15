/*
 * option-number.cpp
 *
 * test the number property of option selected during 
 * calls to option_parser::option
 */

#include <tap++/tap++.h>
#include "include/options_parsing.h"

using namespace TAP;

int main () {
  plan(3);

  util::option_parser parser;
  std::shared_ptr<util::option_t> opt;

  // definition of option/name has default number
  opt = parser.option("--has-opt");
  ok(opt->number == util::Num_Prop::ZERO_ONE, "options have a default number of ZERO_ONE");

  // explicit zero_one option
  opt = parser.option("-single?");
  ok(opt->number == util::Num_Prop::ZERO_ONE, "options explicitly assigned ZERO_ONE number with '?'");

  // explicit zero_many option
  opt = parser.option("-verbose*");
  ok(opt->number == util::Num_Prop::ZERO_MANY, "option explicitly assigned ZERO_MANY number with '*'");

  // TODO add calls to parse, etc

  done_testing();

  return exit_status();
}
