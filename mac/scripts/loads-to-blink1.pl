#!/usr/bin/env perl
#
# cpudisk-to-blink1.pl -- CPU & Disk usage monitor to blink(1)
#                         blink(1) red brightness maps to CPU use
#                         blink(1) blue brightness maps to Disk use
# Example:
#  ./load-to-blink1.pl 
#
# 2013, Tod E. Kurt, http://todbot.com/blog, ThingM
#

use strict;
use warnings;
use POSIX;
use Getopt::Long;

sub usage() {
    print <<EOF;
Usage: load-to-blink1.pl [options]

Options:
  -level <level>        cpu usage alert level (e.g. "90")
  -color <hexcolor>     color to turn at that alert (e.g. "#FF0000")
  -verbose <n>          more verbose output
  -toolpath <path>      path to blink1-tool
EOF
   exit;
}

my $interval_secs = 1;
my $verbose = 0;
my $alertlevel = 90;
my $alertcolor = '#FF0000';
my $blink1toolpath = './blink1-tool';
my $help=0;
my $rc = GetOptions( 'level=i' => \$alertlevel,
                     'color=s' => \$alertcolor,
                     'verbose:i' => \$verbose,
                     'toolpath' => \$blink1toolpath,
                     'interval=i' => \$interval_secs,
                     'help' => \$help,
                     );
usage() if( $rc==0 or @ARGV>0 or $help );

if( !defined($blink1toolpath) || ! -e $blink1toolpath ) { # no user-spec'd path
    chomp( $blink1toolpath = `which blink1-tool` );       # try to find it
    if( $blink1toolpath =~ /not found/ ) {                # if not found
        $blink1toolpath = './blink1-tool';                # path of last resort
    }
}
die "no blink1-tool '$blink1toolpath'\n" if( ! -e $blink1toolpath );

$interval_secs = 1 if( $interval_secs < 1 );

my $iostatcmd = "iostat -w $interval_secs";
open(PH,'-|',$iostatcmd) || die "Unable to open command '$iostatcmd': $!";

while (local $_ = <PH>) {      # read a line from iostat
    next if( /cpu/ || /id/ );  # skip non-data lines
    print "iostat: $_" if($verbose>1);
    my @loadvals = split(/\s+/, $_ );
    my $cpuidle_percent = $loadvals[6];   # 7th value is CPU idle percent
    my $disktps = $loadvals[2];           # 3rd value is disk transactions/sec
    my $load_pct =  (100 - $cpuidle_percent); # load is inverse of idle
    my $disk_pct = 100 * ($disktps / 1000); # faking it, define 1000 to be max tps
  
    my ($r,$g,$b) = ( int($load_pct*255/100), 0, int($disk_pct*255/100)) ;
    print "cpu: $load_pct %  disk: $disk_pct %   rgb: $r,$g,$b\n" if($verbose);

    my $fade_time = ($interval_secs/2.0) * 1000;
    my $quiet = ($verbose>0) ? '' : '-q';
    print "$blink1toolpath $quiet --rgb $r,$g,$b -m $fade_time\n" if( $verbose > 1 );
    my $out = `$blink1toolpath $quiet --rgb $r,$g,$b -m $fade_time`;
    print $out if( $verbose );
    sleep $interval_secs;
}

#    if( $load > $alertlevel ) {
#        ($r,$g,$b) = @alertrgb;
#    }
#    if( $load > $alertlevel2 ) {
#        ($r,$g,$b) = @alertrgb2;
#    }

# convert a hex color code like "#ff334a" into a numberical array (255,51,74)
# kinda from: http://stackoverflow.com/questions/3264165/perl-parse-hex-encoded-string-into-array-with-regex
sub parsehexcolor {
    my ($hexstr) = @_;
    $hexstr =~ s/(#|0x)//g;  # remove leading "#" or "0x"
    my @nums = map eval "0x$_", $hexstr =~ /(..)/g;
    return @nums;
}


# these are unused because they either don't work or work poorly
# but are left as an example to other code that it better shape up
#
sub loadfromiostat {
    my $loadstr = `iostat`;
    my @loadlines = split("\n",$loadstr);
    my @loadvals = split(/\s+/, $loadlines[2] );
    my $cpuidle_percent = $loadvals[9];
    return (100 - $cpuidle_percent);  # convert to percent cpu use
}

#
sub loadfromuptime {
    my $loadstr = `uptime`;            # get uptime numbers
    my @vals = split( / /, $loadstr); # split on whitespace
    my $load1m = $vals[11];  # load is 12th val
    return $load1m;
}

