#!/usr/bin/env perl
#
# cpudisk-to-blink1.pl -- CPU & Disk usage monitor to blink(1)
#                         blink(1) red brightness maps to CPU use
#                         blink(1) blue brightness maps to Disk use
#
# 2013, Tod E. Kurt, http://todbot.com/blog, ThingM
#

use strict;
use warnings;
use POSIX;
use Getopt::Long;


my $interval_secs = 1;
my $verbose = 0;
my $alertlevel = 90;
my $alertcolor = '#FF0000';
my $blink1toolpath = './blink1-tool';
my $help=0;
my $cpu_scale = 1.5;
my $disk_scale = 5.0;

sub usage() {
    print <<EOF;
Usage: load-to-blink1.pl [options]

Options:
  -interval <n>         update interval in seconds (default $interval_secs)
  -cpu_scale <num>    multipler when mapping CPU % (default $cpu_scale)
  -disk_scale <num>    mulipler when mapping Disk % (default $disk_scale)
  -toolpath <path>      path to blink1-tool (if not in PATH or same directory)
  -verbose <n>          more verbose output

Example:
  cpudisk-to-blink1.pl -v 1 -interval 3 -cpu_scale 2.1

EOF
   exit;
}


my $rc = GetOptions( #'level=i' => \$alertlevel,
                     #'color=s' => \$alertcolor,
                     'verbose:i' => \$verbose,
                     'toolpath' => \$blink1toolpath,
                     'interval=i' => \$interval_secs,
                     'cpu_scale=f' => \$cpu_scale,
                     'disk_scale=f' => \$disk_scale,
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

print "Watching CPU & Disk stats every $interval_secs seconds\n";
print "CPU  usage maps to blink(1) red brightness, cpu_scale $cpu_scale\n";
print "Disk usage maps to blink(1) green brightness, disk_scale $disk_scale\n";

my $iostatcmd = "iostat -w $interval_secs";
open(PH,'-|',$iostatcmd) || die "Unable to open command '$iostatcmd': $!";

while (local $_ = <PH>) {      # read a line from iostat
    print "iostat: $_" if($verbose);
    next if( /cpu/ || /id/ );  # skip non-data lines
    my @loadvals = split(/\s+/, $_ );
    my $valcnt = @loadvals;  # 
    my $cpuidle_percent = $loadvals[$valcnt-4];  # 4th from right is CPU idle pct
    my $disktps = $loadvals[2];                  # 3rd from left is main disk trans/s

    my $cpu_pct = 100 - $cpuidle_percent;  # load is inverse of idle
    my $disk_pct = 100 * ($disktps / 700);  # faking it, define 700 to be max tps

    my $cpu_pctf = $cpu_pct * $cpu_scale;  # add fudge factor
    my $disk_pctf = $disk_pct * $disk_scale;

    $cpu_pctf = ($cpu_pctf>100) ? 100 : $cpu_pctf;  # clip to 100
    $disk_pctf = ($disk_pctf>100) ? 100 : $disk_pctf;
   
    my ($r,$g,$b) = ( int($cpu_pctf*255/100), 0, int($disk_pctf*255/100)) ;
    print "cpu: $cpu_pct %  disk: $disk_pct %   rgb: $r,$g,$b\n" if($verbose);

    my $fade_time = ($interval_secs/2.0) * 1000;
    my $quiet = ($verbose>0) ? '' : '-q';
    print "$blink1toolpath $quiet --rgb $r,$g,$b -m $fade_time\n" if( $verbose > 1 );
    my $out = `$blink1toolpath $quiet --rgb $r,$g,$b -m $fade_time`;
    print "blink1-tool: $out" if( $verbose>1);
    sleep $interval_secs;
}

