#!/bin/bash

DATA=.
IN=$DATA/in
OUT=$DATA/out3

EXE=./trab3

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    $EXE < $infile | diff $outfile -
done
