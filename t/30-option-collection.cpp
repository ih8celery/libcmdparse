/**
 * \file 30-option-collection.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test definition of option_t::collection
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "options_parsing.h"

constexpr int ARGC = 3;

using namespace TAP;

int main() {
  plan(10);

  util::opt_parser parser;
  util::option_t opt;
  util::opt_info info;

  opt = parser.option("--age=i");
  ok(opt.collection == util::Collect_Prop::SCALAR, "collection type is SCALAR by default");
  
  opt = parser.option(":some-option");
  ok(opt.collection == util::Collect_Prop::SCALAR, "collection type is SCALAR even with no arguments");

  opt = parser.option(".other-option=s", "other");

  opt = parser.option("--friends=[s]");
  ok(opt.collection == util::Collect_Prop::LIST, "LIST collection type");

  char ** args = new char*[ARGC];
  args[0] = (char*)"--age=";

  TRY_NOT_OK(parser.parse(args, 1), "empty string is not an argument");

  args[0] = (char*)"--friends";
  TRY_NOT_OK(parser.parse(args, 1), "LIST collection type requires an argument");

  args[0] = (char*)"--friend=";
  TRY_NOT_OK(parser.parse(args, 1), "empty string is invalid argument under LIST collection");

  args[0] = (char*)"--friends=adamu";
  TRY_OK(parser.parse(args, 1), "scalars allowed under LIST collection type");

  args[0] = (char*)"--age=10";
  args[1] = (char*)"--friends=james,mark,sylvester";
  args[2] = (char*)".other-option=some,none,any";
  info = parser.parse(args, ARGC);

  ok(info.count("age") == 1, "age may occur at most once and store integer");

  is(info.count("friends"), 3, "friends may occur once, but processes arg into comma-separated list");

  ok(info.count("other") == 1, "comma-separated strings are not split under scalar collection");

  delete [] args;

  done_testing();

  return exit_status();
}
