extern "C" {
#include "ulcg.h"
#include "unif01.h"
#include "bbattery.h"
}

#include "pcg_variants.h"

#include <iomanip>
#include <iostream>
#include <ios>
#include <sstream>
#include <string>
#include <stdint.h>
#include <stdio.h>


std::ostream & operator <<( std::ostream &os, pcg32_random_t state )
{
	os << std::hex;
	os << "state=0x" << std::setw(16) << std::setfill('0') << state.state 
	   << ", inc=0x" << std::setw(16) << std::setfill('0') << state.inc;
	return os;
}


template<typename FnGetBits, typename State>
struct Unif01Gen32 : unif01_Gen
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
Unif01Gen32<FnGetBits,State> 
makeUnif01Gen32( std::string name, FnGetBits getBitsFn, State initialState )
{
	Unif01Gen32<FnGetBits,State> gen( name, getBitsFn, initialState );
	return gen;
}

int main (void)
{
	pcg32_random_t initState;
	pcg32_srandom_r(&initState, 42u, 54u);
	auto rng = makeUnif01Gen32( "pcg32_random_r", &pcg32_random_r, initState );

	bbattery_SmallCrush (&rng);

	return 0;
}
