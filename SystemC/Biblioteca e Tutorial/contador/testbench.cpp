#include "systemc.h"
#include "contador.cpp"

int sc_main (int argc, char* argv[]) {
	sc_signal<bool>   clock;
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

	sc_start(1, SC_NS);
	c = 0;
	enable = 1;
	reset=0;

	while(i){
		cout << "#=== contador: " << counter_out << " | clock: " << c << " | tempo: " << sc_time_stamp() << endl;
		c++;		
		
		if(c==2000){
			i=false;
		}

		clock = 0; 
    		sc_start(1, SC_NS);
    		clock = 1; 
    		sc_start(1, SC_NS);
	}

  	return 0;

}

