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

# TestU01 results

|           RNG            |    SmallCrush     |        Crush         |      BigCrush       |
| ------------------------ | ----------------- | -------------------- | ------------------- |
| murmur1_counter          |                OK |            4 suspect |   1 suspect, 8 fail |
| murmur2_counter          |                OK |    4 suspect, 2 fail |  12 suspect, 8 fail |
| murmur3_counter          |                OK |    3 suspect, 1 fail |   3 suspect, 7 fail |
| pcg32_random_r           |                OK |                   OK |                  OK |
| pcglite32_random         |                OK |                   OK |                  OK |
| siphash11_key_counter    |                   | 13 suspect, 118 fail |                     |
| siphash12_key_counter    |            1 fail |  15 suspect, 25 fail | 23 suspect, 59 fail |
| siphash14_key_counter    |                OK |                   OK |   2 suspect, 3 fail |
| siphash24_counter        |                OK |                   OK |   4 suspect, 3 fail |
| siphash24_key_counter    |                OK |                   OK |   4 suspect, 3 fail |
| siphash24_key_counter_64 |                OK |            2 suspect |                  OK |
| stdcpp_knuth_b           | 1 suspect, 8 fail |  11 suspect, 65 fail |                     |
| stdcpp_minstd_rand       |            9 fail |  12 suspect, 78 fail |                     |
| stdcpp_minstd_rand0      |            9 fail |  14 suspect, 82 fail |                     |
| stdcpp_mt19937           |                OK |               2 fail |                     |
| stdcpp_mt19937_64        |         1 suspect |               2 fail |                     |
| stdcpp_random_device     |                OK |                   OK |                     |
| stdcpp_ranlux24          |            9 fail |  11 suspect, 78 fail |                     |
| stdcpp_ranlux48          |                OK |                   OK |                  OK |
| xxh32_key_counter        |                OK |    3 suspect, 1 fail |                     |
| xxh32_key_counter_64     |                OK |    3 suspect, 1 fail |                     |
| xxh64_key_counter        |                OK |                   OK |                     |
| xxh64_key_counter_64     |                OK |                   OK |                     |

## Test Status description

- OK: success, no statistical issue detected in RNG
- N fail: N statistical tests failed.
- N suspect: N statistical tests came close to failing. Longer testing required to clarify.
- blank: test was not run. There is usually no point in running a long BigCrush test when Crush failed...
