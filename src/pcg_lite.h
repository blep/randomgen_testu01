#pragma once

// Portable implementation of main PCG Random Number Generator
#if __cplusplus
extern "C" {
#endif

#define PCG_STATE_SETSEQ_64_INITIALIZER                                        \
    { 0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL }


typedef struct PCGLite_SeededState32
{
	uint64_t state_;
	uint64_t seq_;
} PCGLite_SeededState32;

extern void pcglite_seed32( PCGLite_SeededState32 *state, uint64_t initState, uint64_t initSeq );

extern uint32_t pcglite_random32( PCGLite_SeededState32 *state );

#if __cplusplus
}
#endif