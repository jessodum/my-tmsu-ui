#!/usr/bin/perl

# This script is used to edit the qmake-generated Makefile.
# It adds custom commands (namely echo commands) to the build
# recipes and outputs to STDOUT.  Therefore, the qmake-generated
# Makefile can be transformed by calling this script as such:
#
#	./FixMakefile.pl Makefile.qmake-generated > Makefile.fixed
#

use strict;
use warnings;

my $ECHO = '@echo';
my $TARGET = '$@';

while (<>)
{
   my $line = $_;
   chomp($line);
   if ( ($line =~ /^\t\$\(CXX\) /)
     || ($line =~ /^\t\$\(LINK\) /)
     || ($line =~ /^\t[\/\w]*g\+\+ /)
     || ($line =~ /^\t[\/\w]*uic /)
     || ($line =~ /^\t[\/\w]*rcc /)
     || ($line =~ /^\t[\/\w]*moc /) )
   {
      print "\t$ECHO ''\n";
      print "\t$ECHO '===== Building $TARGET ====='\n";
      print "\t$ECHO ''\n";
   }

   print "$line\n";
}

