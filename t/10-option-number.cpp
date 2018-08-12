/**
 * \file 10-option-number.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test the number property of option
 */

#include <tap++.h>
#include "cmdparse.h"

using namespace TAP;

constexpr int ARGC = 7;

int main () {
  plan(4);

  cli::Command cmd;

  // definition of option/name has default number
  cmd.option("--has-opt");

  // explicit zero_one option
  cmd.option("-single?");

  // explicit zero_many option
  cmd.option("-verbose*");

  char ** args = new char*[ARGC];
  args[0] = (char*)"-verbose";
  args[1] = (char*)"-single";
  args[2] = (char*)"data";
  args[3] = (char*)"--has-opt";
  args[4] = (char*)"other";
  args[5] = (char*)"-verbose";
  args[6] = (char*)"-verbose";

  auto info = cmd.parse(args, ARGC);
  
  ok(info.rest.size() == 2, "two non-options found");

  ok(info.count("verbose") == 3, "found three verbose options");

  ok(info.has("has-opt"), "has option 'has-opt'");

  ok(info.has("single"), "has option 'single'");

  done_testing();

  delete [] args;
  return exit_status();
}
