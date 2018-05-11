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
  plan(16);

  cli::Command cmd;
  std::shared_ptr<cli::Option> opt;
  cli::Info info;

  opt = cmd.option("--name=");
  ok(opt->type == cli::Property::Arg_Type::STRING, "data type is STRING by default");

  opt = cmd.option("--friend-list=[]");
  ok(opt->type == cli::Property::Arg_Type::STRING, "data type is STRING by default for lists");
  
  opt = cmd.option("--friend-name=s");
  ok(opt->type == cli::Property::Arg_Type::STRING, "STRING type can be given explicitly with 's'");

  opt = cmd.option("--age=!i");
  ok(opt->type == cli::Property::Arg_Type::INTEGER, "INTEGER type can be given explicitly with 'i'");

  opt = cmd.option("-pi=?f");
  ok(opt->type == cli::Property::Arg_Type::FLOAT, "FLOATING POINT type can be given explicitly with 'f'");

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

  ok(info.get("pi").second == "3.14", "pi is approximately 3.14");

  ok(info.get("age").second == "7", "age is 7");

  ok(info.get("friend-name").second == "xavier", "my friend's name is xavier");

  ok(info.get("name").second == "george", "my name is george");

  ok(info.rest.size() == 1, "EOI detected, remaining input moved to rest member");

  delete [] args;

  done_testing();

  return exit_status();
}
