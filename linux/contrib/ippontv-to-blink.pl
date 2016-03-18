#!/usr/bin/env perl

# Simple perl script to provide an "On Air" light for the http://Ippon.TV Judo  Youtube live stream.
# Author: Lance Wicks ( lw@judocoach.com )
use strict;
use warnings;

my $url
    = 'http://gdata.youtube.com/feeds/api/users/judo/live/events?v=2&status=active';

while () {
    my $txt = `curl -s "$url"`;

    unless ( $txt =~ m/openSearch:totalResults>0/ ) {
        `blink1-tool --red`;
        unless ( $txt =~ m/status>active/ ) {
            `blink1-tool --yellow --blink 3`;
        }
    }
    else {
        print `blink1-tool --green`;
    }
    sleep 5;
}
