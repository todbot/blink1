#! /bin/bash

for i in {1..1000}; do blink1-tool  --rgb=$(($RANDOM % 100)),$(($RANDOM % 100)),$(($RANDOM % 100)) -l$((($RANDOM % 3)+1)); sleep 0.05;  done
