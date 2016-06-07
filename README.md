# randomgen_testu01
Tests some random generator against TestU01 random generator quality test suite.

# Build

git submodule update --init --recursive

mkdir build
cd build
cmake -G "Unix Makefiles" .. && make
