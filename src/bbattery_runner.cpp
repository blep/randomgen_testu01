extern "C" {
#include "ulcg.h"
#include "unif01.h"
#include "bbattery.h"
}

#include "pcg_variants.h"
#include "MurmurHash1.h"
#include "MurmurHash2.h"
#include "MurmurHash3.h"

#include <iomanip>
#include <iostream>
#include <ios>
#include <functional>
#include <memory>
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

struct CounterState
{
    CounterState()
        : count_( 0 )
    {
    }

    uint32_t next()
    {
        return count_++;
    }

    uint32_t count_;
};


std::ostream &operator <<( std::ostream &os, const CounterState &state )
{
    os << "count=" << std::setw(8) << std::setfill('0') << state.count_;
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



std::unique_ptr<Unif01GenBase> 
createRng( const std::string &name )
{
    std::unique_ptr<Unif01GenBase> rng;

    pcg32_random_t initState;
    pcg32_srandom_r(&initState, 42u, 54u);

    if ( name == "pcg32_random_r" )
        return makeUnif01Gen32( name, &pcg32_random_r, initState );
    else if ( name == "raw_counter" )
        return makeUnif01Gen32( name, &raw_counter, CounterState() );
    else if ( name == "murmur1_counter" )
        return makeUnif01Gen32( name, &murmur1_counter, CounterState() );
    else if ( name == "murmur2_counter" )
        return makeUnif01Gen32( name, &murmur2_counter, CounterState() );
    else if ( name == "murmur3_counter" )
        return makeUnif01Gen32( name, &murmur3_counter, CounterState() );
    else
    {
        printf( "Unknown rng: '%s'. Aborting...", name.c_str() );
        abort();
    }
}


int main (int argc, const char *argv[] )
{
    std::string name = (argc > 1 ) ? argv[1] : "pcg32_random_r";

    std::unique_ptr<Unif01GenBase> rng = createRng( name );
    bbattery_SmallCrush (rng.get());

    return 0;
}