#!/bin/bash
#
# Copyright 2010 Google Inc. All Rights Reserved.
# Author: tpw@google.com (Tamsyn Waterhouse)

function Die {
  echo "$ME: $1" >&2
  exit 1
}

function TimeString {
  # Try two different implementations of the date function, or default to the
  # raw integer value:
  date -d @$1 2> /dev/null || date -j -f "%s" "$1" || echo $1
}

ME=$(basename $0)
GNUPLOT=gnuplot
MENCODER=mencoder

which "$GNUPLOT" > /dev/null || Die "$GNUPLOT not found"

DATA_NAME=$1

PLOTS_LIST="flux_plots_list.txt"
rm -f $PLOTS_LIST

TIME_STEPS=$(cut -d ' ' -f 1 $DATA_NAME |sort -g |sed '/^$/d' |uniq)
MAXIMUM_VALUE=$(cut -d ' ' -f 4 $DATA_NAME |sort -g |tail -1)
UNITS=$(head -1 $DATA_NAME |cut -d '"' -f 2)

INDEX=0
for TIME in $TIME_STEPS; do
  PLOT_NAME="${DATA_NAME}_${TIME}.png"
  # Try two different invocations of date (for different implementations):
  TIME_STRING=$(TimeString $TIME)
  GPCOMMAND="
    set pm3d map;
    set terminal png size 1000,1000;
    set output \"${PLOT_NAME}\";
    set xlabel \"x (m)\";
    set ylabel \"y (m)\";
    set cblabel \"$UNITS\";
    unset key;
    set title \"${DATA_NAME} at ${TIME_STRING}\";
    set size ratio -1;
    set cbrange [0:$MAXIMUM_VALUE];
    splot '$1' index $INDEX using 2:3:4;
  "
  echo "Plotting frame $INDEX, time $TIME"
  echo $GPCOMMAND | $GNUPLOT
  echo $PLOT_NAME >> $PLOTS_LIST
  INDEX=$(expr $INDEX + 1)
done

if [[ "$INDEX" -gt 5 ]]; then
  which "$MENCODER" > /dev/null || \
    Die "$MENCODER not found; can't create animated plot"
  $MENCODER mf://@$PLOTS_LIST -mf \
    fps=10:type=png -ovc lavc -lavcopts vcodec=mjpeg -oac copy -o $DATA_NAME.avi
fi
