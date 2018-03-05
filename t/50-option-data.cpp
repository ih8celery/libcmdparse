/*
 * @file option-data.cpp
 * @author Adam Marshall (ih8celery)
 * @brief test definition of option_t::data_type by calls to \
 * option_parser::option, and the parsing of options \
 * created thereby
 */

#include <tap++.h>
#include "options_parsing.h"

#define ARGC 5

using namespace TAP;

int main() {
  plan(9);

  util::opt_parser parser;
  util::option_t opt;

  opt = parser.option("--name=");
  ok(opt.data_type == util::Data_Prop::STRING, "data type is STRING by default");
  
  opt = parser.option("--friend-name=s");
  ok(opt.data_type == util::Data_Prop::STRING, "STRING type can be given explicitly with 's'");

  opt = parser.option("--age=!i");
  ok(opt.data_type == util::Data_Prop::INTEGER, "INTEGER type can be given explicitly with 'i'");

  opt = parser.option("-pi=?f");
  ok(opt.data_type == util::Data_Prop::FLOAT, "FLOATING POINT type can be given explicitly with 'f'");

  char ** args = new char*[ARGC];
  args[0] = (char*)"-pi=3.14";
  args[1] = (char*)"--age";
  args[2] = (char*)"7";
  args[3] = (char*)"--friend-name=xavier";
  args[4] = (char*)"--name=george";
  auto info = parser.parse(args, ARGC);

  ok(info.has("pi"), "option pi received");

  ok(info.arg("pi") == "3.14", "pi is approximately 3.14");

  ok(info.arg("age") == "7", "age is 7");

  ok(info.arg("friend-name") == "xavier", "my friend's name is xavier");

  ok(info.arg("name") == "george", "my name is george");

  done_testing();

  delete [] args;
  return exit_status();
}
