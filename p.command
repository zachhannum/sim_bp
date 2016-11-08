#!/bin/bash
for h in {2..12}
do
  for g in {7..12}
  do
    if [ $(( $h % 2)) -eq 0 ]
      then
        if [ $g -ge $h ]
          then
      ~/Documents/sim_bp/sim_bp gshare $g $h 0 0 ~/Documents/sim_bp/traces/trace_jpeg.txt
        fi
      fi
  done
done
