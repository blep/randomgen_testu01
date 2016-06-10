#pragma once

#include "pcg_lite.h"

#if defined(_MSC_VER)

# define PCGLITE_FORCE_INLINE __forceinline

# include <stdlib.h>

# define PCGLITE_ROTL32(x, r) _rotl(x,y)
# define PCGLITE_ROTL64(x, r) _rotl64(x,y)
#else /* #if defined(_MSC_VER) */
# define PCGLITE_FORCE_INLINE inline __attribute__((always_inline))

PCGLITE_FORCE_INLINE uint32_t pcglite_rotl32( uint32_t x, int8_t r )
{
#if PCGLITE_WITH_ASM  &&  __clang__  &&  (__x86_64__  || __i386__)
    asm ("rorw   %%cl, %0" : "=r" (x) : "0" (x), "c" (r));
    return value;
#else
	return (x << r) | (x >> (32u - r));
#endif
}

PCGLITE_FORCE_INLINE uint64_t pcglite_rotl64( uint64_t x, int8_t r )
{
  return (x << r) | (x >> (64u - r));
}
# define PCGLITE_ROTL32(x, r) pcglite_rotl32(x,y)
# define PCGLITE_ROTL64(x, r) pcglite_rotl64(x,y)

#endif


#define PCGLITE_INLINE inline

#define PCGLITE_MULTIPLIER_64 6364136223846793005ULL


/** Move to the next state.
 */
PCGLITE_FORCE_INLINE void internal_pcglite_step32( PCGLite_SeededState32 *state )
{
	state->state_ = state->state_ * PCGLITE_MULTIPLIER_64 + state->seq_;
}


PCGLITE_INLINE void pcglite_seed32( PCGLite_SeededState32 *state, uint64_t initState, uint64_t initSeq )
{
	state->state_ = 0;
	state->seq_ = (initSeq << 1) | 1;
	internal_pcglite_step32( state );
	state->state_ += initState;
	internal_pcglite_step32( state );
}


/** Provides the next 32 bits random number in the sequence.
 */
PCGLITE_INLINE uint32_t pcglite_random32( PCGLite_SeededState32 *seededState )
{
/* Based on pcg_setseq_64_xsh_rr_32_random_r */
	uint64_t state = seededState->state_;
	internal_pcglite_step32( seededState );

    return PCGLITE_ROTL32(((state >> 18u) ^ state) >> 27u, state >> 59u);
}


/** Multi-step advance functions (jump-ahead, jump-back).
 *
 * The method used here is based on Brown, "Random Number Generation
 * with Arbitrary Stride,", Transactions of the American Nuclear
 * Society (Nov. 1994).  The algorithm is very similar to fast
 * exponentiation.
 *
 * Number of inner loop based on number of bits in signedDelta (up to 64 loopsà. If signedDelta is negative, then always 64 inner loops.
 */
PCGLITE_INLINE void pcglite_advance32( PCGLite_SeededState32 *seededState, int64_t signedDelta )
{
/* Based on pcg_setseq_64_advance_r */
	uint64_t state = seededState->state_;
	uint64_t delta = (uint64_t)signedDelta;
	uint64_t cur_mult = PCGLITE_MULTIPLIER_64;
	uint64_t cur_plus = seededState->seq_;

    uint64_t acc_mult = 1u;
    uint64_t acc_plus = 0u;
    while (delta > 0) {
        if (delta & 1) {
            acc_mult *= cur_mult;
            acc_plus = acc_plus * cur_mult + cur_plus;
        }
        cur_plus = (cur_mult + 1) * cur_plus;
        cur_mult *= cur_mult;
        delta /= 2;
    }
    seededState->state_ = acc_mult * state + acc_plus;
}
