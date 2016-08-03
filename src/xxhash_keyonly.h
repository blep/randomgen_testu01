/*
A dumbed down version of XXH64 (less portable) hashing only a 64 bits key
to see what it reduce to)
   
* Based on:
*  xxHash - Fast Hash algorithm
*  Copyright (C) 2012-2016, Yann Collet
*
*  BSD 2-Clause License (http://www.opensource.org/licenses/bsd-license.php)
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions are
*  met:
*
*  * Redistributions of source code must retain the above copyright
*  notice, this list of conditions and the following disclaimer.
*  * Redistributions in binary form must reproduce the above
*  copyright notice, this list of conditions and the following disclaimer
*  in the documentation and/or other materials provided with the
*  distribution.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*  You can contact the author at :
*  - xxHash homepage: http://www.xxhash.com
*  - xxHash source repository : https://github.com/Cyan4973/xxHash
*/


#pragma once

#include <stdint.h>

/* Note : although _rotl exists for minGW (GCC under windows), performance seems poor */
#if defined(_MSC_VER)
#  define XXHKO_rotl32(x,r) _rotl(x,r)
#  define XXHKO_rotl64(x,r) _rotl64(x,r)
#else
#  define XXHKO_rotl32(x,r) ((x << r) | (x >> (32 - r)))
#  define XXHKO_rotl64(x,r) ((x << r) | (x >> (64 - r)))
#endif

/* 
*/

static const uint64_t XXHKO_PRIME64_1 = 11400714785074694791ULL;
static const uint64_t XXHKO_PRIME64_2 = 14029467366897019727ULL;
static const uint64_t XXHKO_PRIME64_3 =  1609587929392839161ULL;
static const uint64_t XXHKO_PRIME64_4 =  9650029242287828579ULL;
static const uint64_t XXHKO_PRIME64_5 =  2870177450012600261ULL;


static uint64_t XXH64_little_endian_keyonly(uint64_t key)
{
    uint64_t k1 = key * XXHKO_PRIME64_2;
    k1 = XXHKO_rotl64(k1, 31);
    k1 *= XXHKO_PRIME64_1;
    uint64_t h64 = k1 ^ (XXHKO_PRIME64_5 + 8);
    h64  = XXHKO_rotl64(h64,27) * XXHKO_PRIME64_1 + XXHKO_PRIME64_4;
    
    h64 ^= h64 >> 33;
    h64 *= XXHKO_PRIME64_2;
    h64 ^= h64 >> 29;
    h64 *= XXHKO_PRIME64_3;
    h64 ^= h64 >> 32;

    return h64;
}



static uint64_t XXH64_little_endian_keyonly_simplified(uint64_t key)
{
    uint64_t k1 = key * XXHKO_PRIME64_2;
    k1 = XXHKO_rotl64(k1, 31);
    k1 *= XXHKO_PRIME64_1;
    uint64_t h64 = k1 ^ (XXHKO_PRIME64_5 + 8);
    h64  = XXHKO_rotl64(h64,27) * XXHKO_PRIME64_1 + XXHKO_PRIME64_4;
    h64 ^= h64 >> 33;
    h64 *= XXHKO_PRIME64_2;
    h64 ^= h64 >> 29;
/*    
    h64 *= XXHKO_PRIME64_3;
    h64 ^= h64 >> 32;
*/
    return h64;
}

