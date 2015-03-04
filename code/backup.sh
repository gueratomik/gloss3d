#!/bin/sh

if [ -z $1 ]
then
    exit 1
fi

ftp -n ftpperso.free.fr <<EOF
user velotrip
bin
hash
cd MOTIF
put $1 `basename $1`
quit
EOF
