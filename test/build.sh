#!/usr/bin/env bash

ROOT="`pwd`/.."
DATA="$ROOT/test/china-latest.shp"
LIB="$ROOT/build/lib"
INC="$ROOT/build/include"
GDAL="$ROOT/gdal_vt"

echo "$LIB"

# must be absolute path
echo "start compiling"
g++ -o exe vt_test.cc -L/${LIB} -lgdal -I/${INC}
