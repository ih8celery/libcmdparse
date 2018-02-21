#!/usr/bin/env perl
#
# tap
# author: Adam Marshall
#  
# tap is a test driver for libtap++, primarily for use with cmake
#

use 5.008;
use strict;
use warnings;

use Cwd qw/abs_path/;

my $VERSION  = '1.0.0';
my $HELP_MSG = <<EOM;
you are using tap, the test driver for libtap++.
usage: tap [options] [files]
options:
  -h|-help    -> print this help and exit
  -q|-quiet   -> turn off subtest output reporting
  -v|-version -> print the version number and exit
EOM
my $subtest_count = 1;

# setting controlled by the command line option -q|-quiet
my $want_subtest_report = 1;

# take a file name. if it is executable, run it and return the result;
# if a directory, run this sub on each executable in the directory
sub run_test {
  my $fname = shift @_;
  my $tests_failed;

  print "running test name $fname\n";

  #convert $fname to an absolute path if it is not one already
  my $file = abs_path $fname;

  $tests_failed = system($file);

  if ($tests_failed == -1) {
    die "a problem occurred executing an argument: $!\n";
  }

  $tests_failed = $tests_failed >> 8;

  if ($want_subtest_report) {
    print "\ntests failed in subtest: .................... $tests_failed\n";
  }

  print "\n";
    
  return 0;
}

my $in_options = 1;
for my $arg (@ARGV) {
  if ($in_options) {
    if ($arg eq '-v' || $arg eq '-version') {
      print "tap v$VERSION\n";
      exit;
    }
    elsif ($arg eq '-h' || $arg eq '-help') {
      print $HELP_MSG, "\n";
      exit;
    }
    elsif ($arg eq '-q' || $arg eq '-quiet') {
      $want_subtest_report = 0;
      next;
    }
    else {
      $in_options = 0;
    }
  }

  if (-f $arg && -x $arg) {
    run_test($arg);
  }
  elsif (-d $arg) {
    my $dir_name = abs_path $arg;
    opendir my ($dir), $dir_name;

    while (my $dir_file = readdir $dir) {
      $dir_file = "$dir_name/$dir_file";
      if (-f $dir_file && -x $dir_file) {
        run_test($dir_file);
      }
    }

    closedir $dir;
  }
  else {
    die "$arg is neither an executable nor a directory";
  }
}
