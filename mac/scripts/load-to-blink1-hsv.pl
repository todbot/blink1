#!/usr/bin/env perl
#
# load-to-blink1.pl -- fetch a hex color code (like "#CC00FF") and 
#                     turn blink(1) that color.
# Example:
#  ./url-to-blink1.pl http://todbot.com/tst/color.txt ../command-line/blink1-tool
#

use warnings;
use strict;
use POSIX;

die "usage: load-to-blink1.pl [redlevel] [blink1-tool-path]\n" if( @ARGV<1 );

my $verbose = 1;

my $redlevel = $ARGV[0]; 
chomp( my $blink1toolpath = `which blink1-tool` );
if( $ARGV[1] ) { 
    $blink1toolpath = $ARGV[1];
} elsif( $blink1toolpath =~ /not found/ ) {
    $blink1toolpath = './blink1-tool';
}
 
die "no blink1-tool at $blink1toolpath\n" if( ! -e $blink1toolpath );

if( $redlevel !~ /\d+/ ) { # redlevel is not numeric
    $redlevel = 2.0;
}


# testing it out
if( 0 ) {
for ( my $h=0; $h< 1; $h+=0.025 ) { 
    my ($r,$g,$b) = hsv2rgb($h*360, 0.99, 0.99);
    printf("h: %2.1f => %2.1f,%2.1f,%2.1f\n", $h, $r,$g,$b);
}
}

while ( 1 ) { 
    my $loadstr = `uptime`;            # get uptime numbers
    my @vals = split( / /, $loadstr); # split on whitespace
    my $load1m = $vals[11];  # load is 12th val
    my $h = (($load1m / $redlevel));
    printf( "load1m: %2.2f/$redlevel h:%2.2f\n", $load1m, $h);
    my ($r,$g,$b) = hsv2rgb( $h*360, 1.0, 1.0 );
    $r = int( $r * 255 ); $g = int( $g * 255 ); $b = int( $b * 255 );
    print "rgb: $r,$g,$b\n";
    #print `$blink1toolpath --rgb 0x$r,0x$g,0x$b`;
    sleep 1;
}



# stolen and bugfixed from: http://www.perlmonks.org/index.pl?node_id=139486
# and: http://www.cs.rit.edu/~ncs/color/t_convert.html
# h = [0,360], s = [0,1], v = [0,1]
sub hsv2rgb {
    my ( $h, $s, $v ) = @_;

    if ( $s == 0 ) { return ($v, $v, $v);  }  # grey

    $h /= 60;
    my $i = floor( $h );
    my $f = $h - $i;
    my $p = $v * ( 1 - $s );
    my $q = $v * ( 1 - $s * $f );
    my $t = $v * ( 1 - $s * ( 1 - $f ) );

    if ( $i == 0 ) {       return ($v, $t, $p);
    } elsif ( $i == 1 ) {  return ($q, $v, $p);
    } elsif ( $i == 2 ) {  return ($p, $v, $t);
    } elsif ( $i == 3 ) {  return ($p, $q, $v);
    } elsif ( $i == 4 ) {  return ($t, $p, $v);
    } else {               return ($v, $p, $q);
    }
}

