#!/usr/bin/env perl
#
# cheerlights-to-blink1.pl -- Set blink1 to current Cheerlights color
#
# Based on url-to-blink1.pl
#
my $blink1toolpath = "./blink1-tool";
$blink1toolpath = $ARGV[0] if( $ARGV[0] );

die "no blink1-tool at $blink1toolpath\n" if( ! -f $blink1toolpath );

my $txt = `curl -s http://api.thingspeak.com/channels/1417/field/1/last.txt`; 

print "Cheerlights response: $txt\n";

my ($r,$g,$b) = "00", "00", "00";

if( $txt eq "white" ) {
    ($r,$g,$b) = (ff,ff,ff);
} elsif( $txt eq "red" ) {
    ($r,$g,$b) = (ff,00,00);
} elsif( $txt eq "blue" ) {
    ($r,$g,$b) = (00,00,ff);
} elsif( $txt eq "purple" ) {    
    ($r,$g,$b) = (80,00,80);
} elsif( $txt eq "green" ) {
    ($r,$g,$b) = (00,ff,00);
} elsif( $txt eq "cyan" ) {
    ($r,$g,$b) = (00,ff,ff);
} elsif( $txt eq "magenta" ) {
    ($r,$g,$b) = (ff,00,ff);
} elsif( $txt eq "yellow" ) {
    ($r,$g,$b) = (ff,ff,00);
} elsif( $txt eq "orange" ) {
    ($r,$g,$b) = (ff,a5,00);
}

print `$blink1toolpath --rgb 0x$r,0x$g,0x$b`;

