#include "systemc.h"

SC_MODULE (contador) {
	sc_in_clk     clock ;
	sc_in<bool>   reset ;
	sc_in<bool>   enable;
	sc_out<sc_uint<4> > counter_out;

	sc_uint<4>	count;

	void incr_count () {
	    if (reset.read() == 1) {
	      	count =  0;
	      	counter_out.write(count);
	    } 
		else if (enable.read() == 1) {
	      		count++;
	      		counter_out.write(count);
		}
	}

	SC_CTOR(contador) {
		SC_METHOD(incr_count);
		sensitive << reset;
		sensitive << clock.pos();
	}
}; 

