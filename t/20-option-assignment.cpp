/**
 * \file 20-option-assignment.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test the definition of the Option::assignment property
 */

#include "cmdparse.h"
#include <tap++.h>

constexpr int ARGC = 7;

using namespace TAP;

int main() {
  plan(8);

  cli::Command cmd;
  std::shared_ptr<cli::Option> opt;

  opt = cmd.option("-humanity=s"); // eq_required
  ok(opt->assignment == cli::Property::Assignment::EQ_REQUIRED, "equals sign is required in argument assignment");
  
  opt = cmd.option("-age=?i"); // eq_maybe
  ok(opt->assignment == cli::Property::Assignment::EQ_MAYBE, "equals sign is optional in argument assignment");

  opt = cmd.option("--is-stupid"); // no_assign
  ok(opt->assignment == cli::Property::Assignment::NO_ASSIGN, "no assignment, so equals sign is not allowed");

  opt = cmd.option("-wife=!s"); // eq_never
  ok(opt->assignment == cli::Property::Assignment::EQ_NEVER, "equals sign not allowed in argument assignment");

  char ** args = new char*[ARGC];
  args[0] = (char*)"data";
  args[1] = (char*)"-wife";
  args[2] = (char*)"ellen";
  args[3] = (char*)"-age=42";
  args[4] = (char*)"other";
  args[5] = (char*)"-humanity=yes";
  args[6] = (char *)"finally";

  cli::Info info = cmd.parse(args, ARGC); 
  ok(info.rest.size() == 3, "found three non-options");

  is(info.get("wife").second, "ellen", "wife's name is ellen");

  is(info.get("age").second, "42", "age is 42");

  is(info.get("humanity").second, "yes", "type is human");

  done_testing();

  delete [] args;
  return exit_status();
}
