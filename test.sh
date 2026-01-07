#!/bin/bash

# Build first if needed
if [ ! -f "build/test_soundex" ]; then
    echo "Building project..."
    ./build.sh
fi

# Run unit tests
echo ""
./build/test_soundex
exit $?
