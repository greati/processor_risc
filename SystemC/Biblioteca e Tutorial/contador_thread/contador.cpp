#include "systemc.h"

SC_MODULE (contador) {
	sc_in<bool>     clock ;
	sc_in<bool>   reset ;
	sc_in<bool>   enable;
	sc_out<sc_uint<4> > counter_out;
	int i = 0;

	sc_uint<4>	count;

	void incr_count () {
		while(true){
		    	if (reset.read() == 1) {
		      		count =  0;
		      		counter_out.write(count);

		    	} 
			else if (enable.read() == 1) {
		      		count++;
		      		counter_out.write(count);

			}
			wait();
			cout << "#=== contador: " << counter_out << " | clock: " << i << " | tempo: " << sc_time_stamp() << endl;
			i++;
		}
	}

	SC_CTOR(contador) {
		SC_THREAD(incr_count);
		sensitive << reset;
		sensitive << clock.pos();
	}

}; 

