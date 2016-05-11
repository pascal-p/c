
my @deplst;
my $i = 1;

foreach my $dep (qw|sllist  my_malloc graph|) {
  push(@deplst,  
    { 
        src  => "SRC${i}", 
        slib => "${dep}.c", 
        obj  => "OBJ${i}",
        tobj => "${dep}.o",
        inc  => "${dep}.h", 
    });
  $i++;
}

my $target = 'graph';

( myname => "test_${target}",
  mkname => 'make-${target}.mk',
  deplst => \@deplst );


