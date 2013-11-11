#!/bin/sh
xml c14n < tb.gdml | xml fo -s 2 > tb_single.gdml
wc -l tb_single.gdml
