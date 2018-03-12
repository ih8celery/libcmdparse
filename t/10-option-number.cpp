/**
 * \file 10-option-number.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test the number property of option
 */

#include <tap++.h>
#include "options_parsing.h"

using namespace TAP;

constexpr int ARGC = 7;

int main () {
  plan(7);

  util::opt_parser parser;
  util::option_t opt;

  // definition of option/name has default number
  opt = parser.option("--has-opt");
  ok(opt.number == util::Num_Prop::ZERO_ONE, "options have a default number of ZERO_ONE");

  // explicit zero_one option
  opt = parser.option("-single?");
  ok(opt.number == util::Num_Prop::ZERO_ONE, "options explicitly assigned ZERO_ONE number with '?'");

  // explicit zero_many option
  opt = parser.option("-verbose*");
  ok(opt.number == util::Num_Prop::ZERO_MANY, "option explicitly assigned ZERO_MANY number with '*'");

  char ** args = new char*[ARGC];
  args[0] = (char*)"-verbose";
  args[1] = (char*)"-single";
  args[2] = (char*)"data";
  args[3] = (char*)"--has-opt";
  args[4] = (char*)"other";
  args[5] = (char*)"-verbose";
  args[6] = (char*)"-verbose";

  auto info = parser.parse(args, ARGC);
  
  ok(info.rem.size() == 2, "two non-options found");

  ok(info.count("verbose") == 3, "found three verbose options");

  ok(info.has("has-opt"), "has option 'has-opt'");

  ok(info.has("single"), "has option 'single'");

  done_testing();

  delete [] args;
  return exit_status();
}
