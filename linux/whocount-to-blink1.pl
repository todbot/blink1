#!/usr/bin/env perl

# if more than this many users, raise alert
$maxusercount = 1;
# 
$blink1_basecmd = "sudo ~/projects/blink1/commandline/blink1-tool";

while( 1 ) {
  $usercount = `who | wc -l`;
  if( $usercount > $maxusercount ) {
    `$blink1_basecmd --rgb 255,0,0`;
  }
  else { 
    `$blink1_basecmd --rgb 0,0,0`;
  }
  sleep 1;
}
