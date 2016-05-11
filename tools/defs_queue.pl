
my @deplst;
my $i = 1;

foreach my $dep (qw|queue sllist my_malloc|) {
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

my $target = 'queue';

( myname => "test_${target}",
  mkname => 'make-${target}.mk',
  deplst => \@deplst );
