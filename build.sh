#! /bin/bash

CPP_FLAGS="-Wall -Wextra -fno-exceptions -fno-rtti"
CPP_FLAGS_WARN="-Wno-write-strings -Wno-unused-parameter"
CPP_FLAGS_DEBUG="-O0 -g -DDEBUG"
CPP_FLAGS_RELEASE="-O3"

LD_FLAGS="-flto"

CXX=clang++

if [ "$1" == "release" ]
then
	CPP_FLAGS+=" $CPP_FLAGS_WARN $CPP_FLAGS_RELEASE"
else
	CPP_FLAGS+=" $CPP_FLAGS_WARN $CPP_FLAGS_DEBUG"
fi

mkdir -p build/

echo "$(which $CXX) $CPP_FLAGS $LD_FLAGS src/test.cpp -o build/soundex_test"
$CXX $CPP_FLAGS $LD_FLAGS src/test.cpp -o build/soundex_test

cp *.txt build/
