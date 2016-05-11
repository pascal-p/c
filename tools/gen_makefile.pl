#!/usr/bin/env perl

use Modern::Perl;
use Template;
use Data::Dumper;


## SUB 
sub usage() {
  my $mess = << "_EOM_";
usage: 
  $0 <def_file.pl>

aim:
  generate a makefile given a list of definition (def_file.pl) expressed in  Perl

_EOM_
  print $mess ."\n";
  exit 0;    
}


## MAIN 
my %data;

usage unless (@ARGV >= 1);
usage if ($ARGV[0] =~ m/\-h|\-\-h|\-\-help/i);

chomp(my $incfile = $ARGV[0]);
( ! -r $incfile) && die("could NOT read file $incfile");

# require ${incfile};
%data = do ${incfile};

my $tt = Template->new();

$tt->process('makefile.mk.templ', \%data)
    || die $tt->error();

exit(0);
