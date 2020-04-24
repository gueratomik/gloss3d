#!/bin/bash

tmpout="/tmp/signatures.tmp"
header="signatures.h"

echo "#ifndef _SIGNATURES_H_" > $header
echo "#define _SIGNATURES_H_" >> $header
echo >> $header
echo "/* This file is auto generated */" >> $header
echo "/* Chunk IDs are created by using cksum on signature names */" >> $header
echo "/* Hence, please do not change signature names */" >> $header
echo >> $header

for i in `cat signatures | grep "SIG" | sed 's/-//g'`
do
    LBL=`echo $i | awk '{ print $1 }'`
    SIG=`echo $LBL | cksum  | awk '{ printf "0x%08X\n", $1}'`
    echo "#define $LBL $SIG"
done  | sort -u > $tmpout

cat $tmpout | column -t >> $header

echo >> $header
echo "#endif" >> $header

