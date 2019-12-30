#!/bin/bash

DATA=/home/martinuzzo/Códigos/Compiladores/Trab4
IN=$DATA/in
OUT=$DATA/out4

EXE=./trab4

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    $EXE < $infile | diff $outfile -
done
