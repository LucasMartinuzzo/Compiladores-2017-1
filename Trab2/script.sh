#!/bin/bash

DATA=.
IN=$DATA/in
OUT=$DATA/out2

EXE=./trab2

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    $EXE < $infile | diff $outfile -
done
