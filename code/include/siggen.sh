#!/bin/bash

tmpout="/tmp/signatures.tmp"
header="signaturesv3.h"

echo "#ifndef _SIGNATURES_H_" > $header
echo "#define _SIGNATURES_H_" >> $header
echo >> $header
echo "/* This file is auto generated */" >> $header
echo "/* Chunk IDs are created by using cksum on signature names */" >> $header
echo "/* Hence, please do not change signature names */" >> $header
echo >> $header

for i in `cat signatures | grep "SIG" | sed 's/-//g'`
do
    LBL=`echo $i | awk -F= '{ print $1 }'`
    HEX=`echo $i | awk -F= '{ print $2 }'`
    SIG=`echo $LBL | cksum  | awk '{ printf "0x%08X\n", $1}'`
    if [[ $HEX == 0x* ]]
    then
        echo "#define $LBL $HEX"
    else
        echo "#define $LBL $SIG"
    fi
done  | sort -u > $tmpout

cat $tmpout | column -t >> $header

echo >> $header
echo "#endif" >> $header

