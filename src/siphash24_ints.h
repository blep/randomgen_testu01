#pragma once

/* A siphash_2_4 implementation specialized to hash 32/64 bits integers.
 */

#define SIPHASH_XOR64_INT(v, x) ((v) ^= (x))
#define SIPHASH_XOR64_TO(v, s) ((v) ^= (s))
#define SIPHASH_ROTL64(v, s) ((v) << (s)) | ((v) >> (64 - (s)))
#define SIPHASH_ROTL64_TO(v, s) ((v) = SIPHASH_ROTL64((v), (s)))
#define SIPHASH_ADD64_TO(v, s) ((v) += (s))

#define SIPHASH_2_ROUND(m, v0, v1, v2, v3)	\
do {									\
    SIPHASH_XOR64_TO((v3), (m));				\
    SIPHASH_COMPRESS(v0, v1, v2, v3);		\
    SIPHASH_COMPRESS(v0, v1, v2, v3);		\
    SIPHASH_XOR64_TO((v0), (m));				\
} while (0)

#define SIPHASH_COMPRESS(v0, v1, v2, v3)	\
do {					\
    SIPHASH_ADD64_TO((v0), (v1));		\
    SIPHASH_ADD64_TO((v2), (v3));		\
    SIPHASH_ROTL64_TO((v1), 13);		\
    SIPHASH_ROTL64_TO((v3), 16);		\
    SIPHASH_XOR64_TO((v1), (v0));		\
    SIPHASH_XOR64_TO((v3), (v2));		\
    SIPHASH_ROTL64_TO((v0), 32);		\
    SIPHASH_ADD64_TO((v2), (v1));		\
    SIPHASH_ADD64_TO((v0), (v3));		\
    SIPHASH_ROTL64_TO((v1), 17);		\
    SIPHASH_ROTL64_TO((v3), 21);		\
    SIPHASH_XOR64_TO((v1), (v2));		\
    SIPHASH_XOR64_TO((v3), (v0));		\
    SIPHASH_ROTL64_TO((v2), 32);		\
} while(0)


#define SIP_INIT_STATE0 8317987319222330741ULL
#define SIP_INIT_STATE1 7237128888997146477ULL
#define SIP_INIT_STATE2 7816392313619706465ULL 
#define SIP_INIT_STATE3 8387220255154660723ULL

inline uint64_t
sip_hash24_key_only(uint64_t key)
{
    uint64_t k0, k1;
    uint64_t v0, v1, v2, v3;

    k0 = key;
    k1 = 0;

    v0 = k0; SIPHASH_XOR64_TO(v0, SIP_INIT_STATE0);
    v1 = k1; SIPHASH_XOR64_TO(v1, SIP_INIT_STATE1);
    v2 = k0; SIPHASH_XOR64_TO(v2, SIP_INIT_STATE2);
    v3 = k1; SIPHASH_XOR64_TO(v3, SIP_INIT_STATE3);

    SIPHASH_2_ROUND(0, v0, v1, v2, v3);

    SIPHASH_XOR64_INT(v2, 0xff);

    SIPHASH_COMPRESS(v0, v1, v2, v3);
    SIPHASH_COMPRESS(v0, v1, v2, v3);
    SIPHASH_COMPRESS(v0, v1, v2, v3);
    SIPHASH_COMPRESS(v0, v1, v2, v3);

    SIPHASH_XOR64_TO(v0, v1);
    SIPHASH_XOR64_TO(v0, v2);
    SIPHASH_XOR64_TO(v0, v3);
    return v0;
}
