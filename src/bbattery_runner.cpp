extern "C" {
    #include <ulcg.h>
    #include <unif01.h>
    #include <bbattery.h>
    #include <siphash.h>
    #include "siphash24_ints.h"
}

#include <pcg_variants.h>
#include "pcg_lite.h"
#include "pcg_lite_inline.h"
#include <MurmurHash1.h>
#include <MurmurHash2.h>
#include <MurmurHash3.h>

#include <iomanip>
#include <iostream>
#include <ios>
#include <functional>
#include <memory>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <utility>


std::ostream & operator <<( std::ostream &os, pcg32_random_t state )
{
    os << std::hex;
    os << "state=0x" << std::setw(16) << std::setfill('0') << state.state 
       << ", inc=0x" << std::setw(16) << std::setfill('0') << state.inc;
    return os;
}


std::ostream & operator <<( std::ostream &os, PCGLite_SeededState32 state )
{
    os << std::hex;
    os << "state=0x" << std::setw(16) << std::setfill('0') << state.state_ 
       << ", seq=0x" << std::setw(16) << std::setfill('0') << state.seq_;
    return os;
}

struct Unif01GenBase : unif01_Gen
{
    virtual ~Unif01GenBase()
    {
    }
};


template<typename FnGetBits, typename State>
struct Unif01Gen32 : Unif01GenBase
{
    Unif01Gen32( std::string nameStr, FnGetBits getBitsFn, State initialState )
        : name_( nameStr )
        , getBitsFn_( getBitsFn )
        , state_( initialState )
    {
        name = const_cast<char *>( name_.c_str() ); // C API...
        param = this;
        state = &state_;
        GetBits = &Unif01Gen32::getBits;
        GetU01 = &Unif01Gen32::getU01;
        Write = &Unif01Gen32::writeState;
    }

    Unif01Gen32( const Unif01Gen32 &other ) = delete;
    Unif01Gen32 operator =( const Unif01Gen32 &other ) = delete;

    static unsigned long getBits(void *vpar, void *vsta) {
        Unif01Gen32 *self = (Unif01Gen32 *)vpar;
        State *state = (State *)vsta;
        return self->getBitsFn_( state );
    }

    static double getU01(void *par, void *sta) {
        return getBits(par, sta) * unif01_INV32;
    }

    static void writeState (void *sta)
    {
        State *state = (State *)sta;
        std::cout << "state: " << *state << std::endl;
    }

    std::string name_;
    FnGetBits getBitsFn_;
    State state_;
};


template<typename FnGetBits, typename State>
std::unique_ptr<Unif01GenBase>
makeUnif01Gen32( std::string name, FnGetBits getBitsFn, State initialState )
{
    return std::unique_ptr<Unif01GenBase>( new Unif01Gen32<FnGetBits,State>( name, getBitsFn, initialState ) );
}

// Dumb increasing counter for "failing" statistical RNG tests

template<typename T>
struct GenericCounterState
{
    GenericCounterState()
        : count_( 0 )
    {
    }

    T next()
    {
        return count_++;
    }

    T count_;
};

using CounterState = GenericCounterState<uint32_t>;
using CounterState64 = GenericCounterState<uint64_t>;


std::ostream &operator <<( std::ostream &os, const CounterState &state )
{
    os << "count=" << std::setw(8) << std::setfill('0') << state.count_;
    return os;
}

std::ostream &operator <<( std::ostream &os, const CounterState64 &state )
{
    os << "count=" << std::setw(16) << std::setfill('0') << state.count_;
    return os;
}

uint32_t raw_counter( CounterState *state )
{
    return state->next();
}

uint32_t murmur1_counter( CounterState *state )
{
    uint32_t v = state->next();
    return MurmurHash1Aligned( &v, sizeof(v), 0);
}

uint32_t murmur2_counter( CounterState *state )
{
    uint32_t v = state->next();
    return MurmurHashAligned2( &v, sizeof(v), 0);
}

uint32_t murmur3_counter( CounterState *state )
{
    uint32_t v = state->next();
    uint32_t out;
    MurmurHash3_x86_32( &v, sizeof(v), 0, &out);
    return out;
}

uint32_t siphash24_counter( CounterState *state )
{
    union Key {
        uint32_t vs[4];
        uint8_t keys[16];
    } key;
    key.vs[0] = state->next();
    key.vs[1] = 0;
    key.vs[2] = 0;
    key.vs[3] = 0;
    return (uint32_t)sip_hash24( key.keys, NULL, 0);
}

uint32_t siphash24_key_counter( CounterState *state )
{
    return (uint32_t)sip_hash24_key_only( state->next());
}

uint32_t siphash14_key_counter( CounterState *state )
{
    return (uint32_t)sip_hash14_key_only( state->next());
}

uint32_t siphash12_key_counter( CounterState *state )
{
    return (uint32_t)sip_hash12_key_only( state->next());
}

uint32_t siphash11_key_counter( CounterState *state )
{
    return (uint32_t)sip_hash11_key_only( state->next());
}

uint32_t siphash24_key_counter_64( CounterState64 *state )
{
    return (uint32_t)sip_hash24_key_only( state->next());
}

template<typename T>
uint32_t cpp_random_engine( T *state )
{
    return (uint32_t)(*state)();
}


std::unique_ptr<Unif01GenBase> 
createRng( const std::string &name )
{
    std::unique_ptr<Unif01GenBase> rng;

    if ( name == "pcg32_random_r" )
    {
        pcg32_random_t initState;
        pcg32_srandom_r(&initState, 42u, 54u);
        return makeUnif01Gen32( name, &pcg32_random_r, initState );
    }
    if ( name == "pcglite32_random" )
    {
        PCGLite_SeededState32 initState;
        pcglite_seed32(&initState, 42u, 54u);
        return makeUnif01Gen32( name, &pcglite_random32, initState );
    }
    else if ( name == "raw_counter" )
        return makeUnif01Gen32( name, &raw_counter, CounterState() );
    else if ( name == "murmur1_counter" )
        return makeUnif01Gen32( name, &murmur1_counter, CounterState() );
    else if ( name == "murmur2_counter" )
        return makeUnif01Gen32( name, &murmur2_counter, CounterState() );
    else if ( name == "murmur3_counter" )
        return makeUnif01Gen32( name, &murmur3_counter, CounterState() );
    else if ( name == "siphash24_counter" )
        return makeUnif01Gen32( name, &siphash24_counter, CounterState() );
    else if ( name == "siphash24_key_counter" )
        return makeUnif01Gen32( name, &siphash24_key_counter, CounterState() );
    else if ( name == "siphash14_key_counter" )
        return makeUnif01Gen32( name, &siphash14_key_counter, CounterState() );
    else if ( name == "siphash12_key_counter" )
        return makeUnif01Gen32( name, &siphash12_key_counter, CounterState() );
    else if ( name == "siphash11_key_counter" )
        return makeUnif01Gen32( name, &siphash11_key_counter, CounterState() );
    else if ( name == "siphash24_key_counter_64" )
        return makeUnif01Gen32( name, &siphash24_key_counter_64, CounterState64() );
    else if ( name == "mt19937")
    {
        std::mt19937 rng( 56 );
        return makeUnif01Gen32( name, &cpp_random_engine<decltype(rng)>, rng );
    }
    else if ( name == "mt19937_64")
    {
        std::mt19937_64 rng( 56 );
        return makeUnif01Gen32( name, &cpp_random_engine<decltype(rng)>, rng );
    }
    else
    {
        printf( "Unknown rng: '%s'. Aborting...", name.c_str() );
        abort();
    }
}


static void 
dump_number( Unif01GenBase *rng, int count )
{
    printf("Numbers: \n");
    while ( count-- > 0 )
    {
        unsigned long value = rng->GetBits( rng->param, rng->state );
        printf( "%lx\n", value);
    }
}


int main (int argc, const char **argv )
{
    std::string name = "pcg32_random_r";
    std::string runner = "small_crush";
    ++argv; --argc;
    if ( argc > 0 )
    {
        name = *argv;
        ++argv; --argc;
    }
    if ( argc > 0 )
    {
        runner = *argv;
        ++argv; --argc;
    }

    std::unique_ptr<Unif01GenBase> rng = createRng( name );
    if ( runner == "small_crush" )
        bbattery_SmallCrush(rng.get());
    else if ( runner == "crush" )
        bbattery_Crush(rng.get());
    else if ( runner == "big_crush" )
        bbattery_BigCrush(rng.get());
    else if ( runner == "dump_int" )
    {
        int count = 10*1000;
        if ( argc > 0 )
        {
            count = atoi(*argv);
            if ( count <= 0 )
            {
                printf( "Invalid number of random number to dump: %s\n", *argv);
                return 2;
            }
            ++argv; --argc;
        }
        dump_number( rng.get(), count );
    }
    else
    {
        printf( "Unsupported test runner: %s\n", runner.c_str() );
        return 2;
    }

    return 0;
}
