#!/bin/bash

DATA=/home/martinuzzo/Códigos/Compiladores/Trab3
IN=$DATA/in
OUT=$DATA/out11

EXE=./trab3

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    outfile=$OUT/${base/.cm/.out}
    $EXE < $infile > $outfile
done
