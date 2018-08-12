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
  plan(4);

  cli::Command cmd;

  cmd.option("-humanity=s"); // eq_required
  
  cmd.option("-age=?i"); // eq_maybe

  cmd.option("--is-stupid"); // no_assign

  cmd.option("-wife=!s"); // eq_never

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

  is(*info.find("wife"), "ellen", "wife's name is ellen");

  is(*info.find("age"), "42", "age is 42");

  is(*info.find("humanity"), "yes", "type is human");

  done_testing();

  delete [] args;
  return exit_status();
}
