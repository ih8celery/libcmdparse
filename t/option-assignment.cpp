/*
 * option-assignment.cpp
 *
 * test the definition of the option_t::assignment property by
 * option declarations, and the parsing of options where that
 * property is of interest
 */

#include "include/options_parsing.h"
#include <tap++/tap++.h>

using namespace TAP;

int main() {
  plan(4);

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

  // TODO add calls to parse, etc

  done_testing();

  return exit_status();
}
