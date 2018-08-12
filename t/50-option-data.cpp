/**
 * \file 50-option-data.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test definition of Option::data_type
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "cmdparse.h"

constexpr int ARGC = 7;

using namespace TAP;

int main() {
  plan(11);

  cli::Command cmd;
  cli::Info info;

  cmd.option("--name=");

  cmd.option("--friend-list=[]");
  
  cmd.option("--friend-name=s");

  cmd.option("--age=!i");

  cmd.option("-pi=?f");

  char ** args = new char*[ARGC];
  args[0] = (char*)"-pi=3";

  TRY_OK(cmd.parse(args, 1), "float arguments accept integers");

  args[0] = (char*)"--friend-name=22";

  TRY_OK(cmd.parse(args, 1), "string arguments accept numbers");

  args[0] = (char*)"--age";
  args[1] = (char*)"1s";

  TRY_NOT_OK(cmd.parse(args, 2), "integer option arguments cannot contain non-digits");

  args[0] = (char*)"-pi=3.14f";

  TRY_NOT_OK(cmd.parse(args, 1), "float option arguments may not contain letters");

  args[0] = (char*)"-pi=3.14,3.1416";

  TRY_NOT_OK(cmd.parse(args, 1), "under SCALAR collection, a list of correct type fails to parse");

  args[0] = (char*)"-pi=3.14";
  args[1] = (char*)"--age";
  args[2] = (char*)"7";
  args[3] = (char*)"--friend-name=xavier";
  args[4] = (char*)"--name=george";
  args[5] = (char*)"--";
  args[6] = (char*)"--name";

  info = cmd.parse(args, ARGC);

  ok(info.has("pi"), "option pi received");

  ok(*info.find("pi") == "3.14", "pi is approximately 3.14");

  ok(*info.find("age") == "7", "age is 7");

  ok(*info.find("friend-name") == "xavier", "my friend's name is xavier");

  ok(*info.find("name") == "george", "my name is george");

  ok(info.rest.size() == 1, "EOI detected, remaining input moved to rest member");

  delete [] args;

  done_testing();

  return exit_status();
}
