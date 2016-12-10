#include "systemc.h"
#include "contador.cpp"

int sc_main (int argc, char* argv[]) {
	//sc_signal<bool>   clock;
	sc_clock clock("clk", 10, SC_NS, 0.5, 0.0, SC_NS, true);
	sc_signal<bool>   reset;
	sc_signal<bool>   enable;
	sc_signal<sc_uint<4> > counter_out;
	bool i = true;
	int c;

	contador counter("COUNTER");
	counter.clock(clock);
	counter.reset(reset);
	counter.enable(enable);
	counter.counter_out(counter_out);
	c = 0;
	enable = 1;
	reset=0;
	sc_start();
	

  	return 0;

}

