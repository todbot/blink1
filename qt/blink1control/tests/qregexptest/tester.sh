#!/bin/sh

./qregexptest 'pattern:todbot'
./qregexptest 'pattern:"todbot"'
./qregexptest 'pattern:"todbot is here"'
./qregexptest '{ "pattern":"todbot" }'

