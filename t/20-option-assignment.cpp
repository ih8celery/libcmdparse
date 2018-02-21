/*
 * option-assignment.cpp
 *
 * test the definition of the option_t::assignment property by
 * option declarations, and the parsing of options where that
 * property is of interest
 */

#include "options_parsing.h"
#include <tap++.h>

#define ARGC 7

using namespace TAP;

int main() {
  plan(8);

  util::option_parser parser;
  std::shared_ptr<util::option_t> opt;

  opt = parser.option("-humanity=s"); // eq_required
  ok(opt->assignment == util::Assign_Prop::EQ_REQUIRED, "equals sign is required in argument assignment");
  
  opt = parser.option("-age=?i"); // eq_maybe
  ok(opt->assignment == util::Assign_Prop::EQ_MAYBE, "equals sign is optional in argument assignment");

  opt = parser.option("--is-stupid"); // no_assign
  ok(opt->assignment == util::Assign_Prop::NO_ASSIGN, "no assignment, so equals sign is not allowed");

  opt = parser.option("-wife=!s"); // eq_never
  ok(opt->assignment == util::Assign_Prop::EQ_NEVER, "equals sign not allowed in argument assignment");

  char * args[] = { "data", "-wife", "ellen", "-age=42", "other", "-humanity=yes", "finally"};
  auto info = parser.parse(args, ARGC); 

  ok(info.rem.size() == 3, "found three non-options");

  is(info.arg("wife"), "ellen", "wife's name is ellen");

  is(info.arg("age"), "42", "age is 42");

  is(info.arg("humanity"), "yes", "type is human");

  done_testing();

  return exit_status();
}
