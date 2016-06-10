# randomgen_testu01
Tests some uniform random number generator (RNG) using [TestU01](https://en.wikipedia.org/wiki/TestU01) library that perform empirical statistical testing on uniform RNG.

# Build

git submodule update --init --recursive

mkdir build
cd build
cmake -G "Unix Makefiles" .. && make

# Run tests

sudo pip install invoke

doit -n2 small_crush
-n2 indicate the number of parallel process
