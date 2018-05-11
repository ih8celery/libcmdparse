/**
 * \file 80-subcommands.cpp
 * \author Adam Marshall
 * \brief test the use of subcommands in libcmdparse
 */

#define WANT_TEST_EXTRAS
#include <tap++.h>
#include "cmdparse.h"

using namespace TAP;
using namespace cli;

int main() {
  Command cmd;

  std::shared_ptr<Command> cmd1, cmd2;

  cmd1 = cmd.command("remote");
  cmd2 = cmd1->command("add");

  char ** argv = new char*[2];
  argv[0] = (char*)("remote");
  argv[1] = (char*)("add");

  auto info = cmd.parse(argv, 2);

  plan(2);

  ok(info.has_command("remote"), "subcommand 'remote'");
  ok(info.has_command("add"), "subcommand 'add'");

  done_testing();

  delete [] argv;

  return exit_status();
}
