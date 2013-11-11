#!/bin/sh

#
# Script to test command-line parsing.
#

$SLIC_BASE/scripts/run-log.sh \
    -h \
    --help \
    --version \
    -v \
    -g $SLIC_BASE/examples/template.lcdd \
    -i muons \
    -l LHEP \
    -o test_ops_lcio_output_file \
    -p `pwd` \
    -m $SLIC_BASE/macros/mandatory.mac \
    -z