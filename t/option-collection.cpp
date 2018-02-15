/*
 * option-collection.cpp
 *
 * test definition of option_t::collection by calls to
 * option_parser::option, and the parsing of options
 * created thereby
 */

#include <tap++/tap++.h>
#include "include/options_parsing.h"

using namespace TAP;

int main() {
  plan(2);

  util::option_parser parser;
  std::shared_ptr<util::option_t> opt;

  opt = parser.option("--age=i");
  ok(opt->collection == util::Collect_Prop::SCALAR, "collection type is SCALAR by default");
  
  opt = parser.option("--friends=[s]");
  ok(opt->collection == util::Collect_Prop::LIST, "LIST collection type");

  // TODO add calls to parse, etc

  done_testing();

  return exit_status();
}
