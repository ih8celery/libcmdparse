/**
 * \file 30-option-collection.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test definition of Option::collection
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "cmdparse.h"

constexpr int ARGC = 3;

using namespace TAP;

int main() {
  plan(10);

  cli::Command cmd;
  std::shared_ptr<cli::Option> opt;
  cli::Info info;

  opt = cmd.option("--age=i");
  ok(opt->collection == cli::Property::Collection::SCALAR, "collection type is SCALAR by default");
  
  opt = cmd.option(":some-option");
  ok(opt->collection == cli::Property::Collection::SCALAR, "collection type is SCALAR even with no arguments");

  opt = cmd.option(".other-option=s", "other");

  opt = cmd.option("--friends=[s]");
  ok(opt->collection == cli::Property::Collection::LIST, "LIST collection type");

  char ** args = new char*[ARGC];
  args[0] = (char*)"--age=";

  TRY_NOT_OK(cmd.parse(args, 1), "empty string is not an argument");

  args[0] = (char*)"--friends";
  TRY_NOT_OK(cmd.parse(args, 1), "LIST collection type requires an argument");

  args[0] = (char*)"--friend=";
  TRY_NOT_OK(cmd.parse(args, 1), "empty string is invalid argument under LIST collection");

  args[0] = (char*)"--friends=adamu";
  TRY_OK(cmd.parse(args, 1), "scalars allowed under LIST collection type");

  args[0] = (char*)"--age=10";
  args[1] = (char*)"--friends=james,mark,sylvester";
  args[2] = (char*)".other-option=some,none,any";
  info = cmd.parse(args, ARGC);

  ok(info.count("age") == 1, "age may occur at most once and store integer");

  is(info.count("friends"), 3, "friends may occur once, but processes arg into comma-separated list");

  ok(info.count("other") == 1, "comma-separated strings are not split under scalar collection");

  delete [] args;

  done_testing();

  return exit_status();
}
