/**
 * \file 30-option-collection.cpp
 * \author Adam Marshall (ih8celery)
 * \brief test definition of option_t::collection
 */

#include <tap++.h>
#include "options_parsing.h"

constexpr int ARGC = 2;

using namespace TAP;

int main() {
  plan(4);

  util::opt_parser parser;
  util::option_t opt;

  opt = parser.option("--age=i");
  ok(opt.collection == util::Collect_Prop::SCALAR, "collection type is SCALAR by default");
  
  opt = parser.option("--friends=[s]");
  ok(opt.collection == util::Collect_Prop::LIST, "LIST collection type");

  char ** args = new char*[ARGC];
  args[0] = (char*)"--age=10";
  args[1] = (char*)"--friends=james,mark,sylvester";
  auto info = parser.parse(args, ARGC);

  ok(info.count("age") == 1, "age may occur at most once and store integer");

  is(info.count("friends"), 3, "friends may occur once, but processes arg into comma-separated list");

  done_testing();

  delete [] args;
  return exit_status();
}
