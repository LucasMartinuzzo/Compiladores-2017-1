#!/bin/bash

DATA=/home/martinuzzo/Códigos/Compiladores/Trab3
IN=$DATA/in
OUT=$DATA/out11

EXE=./trab3

for infile in `ls $IN/*.cm`; do
    base=$(basename $infile)
    dotfile=$OUT/${base/.cm/.dot}
    pdffile=$OUT/${base/.cm/.pdf}
    $EXE < $infile > $dotfile
    dot -Tpdf $dotfile -o $pdffile
done
