/*
 * option-data.cpp
 *
 * test definition of option_t::data_type by calls to
 * option_parser::option, and the parsing of options
 * created thereby
 */

#include <tap++/tap++.h>
#include "include/options_parsing.h"

using namespace TAP;

int main() {
  plan(4);

  util::option_parser parser;
  std::shared_ptr<util::option_t> opt;

  opt = parser.option("--name=");
  ok(opt->data_type == util::Data_Prop::STRING, "data type is STRING by default");
  
  opt = parser.option("--friend-name=s");
  ok(opt->data_type == util::Data_Prop::STRING, "STRING type can be given explicitly with 's'");

  opt = parser.option("--age=!i");
  ok(opt->data_type == util::Data_Prop::INTEGER, "INTEGER type can be given explicitly with 'i'");

  opt = parser.option("-pi=?f");
  ok(opt->data_type == util::Data_Prop::FLOAT, "FLOATING POINT type can be given explicitly with 'f'");

  // TODO add calls to parse, etc

  done_testing();

  return exit_status();
}
