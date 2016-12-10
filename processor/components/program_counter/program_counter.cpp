#include "systemc.h"

/* Program counter
 *
 * Keeps a counter, allowing to increase by 1 and load
 * a value. Address the instruction memory.
 * */
SC_MODULE(program_counter){
	//-- Signal declarations --//
	sc_in_clk  clock;
	sc_in<bool> reset;
	sc_in<bool> enable;
	sc_in<bool> load;
	sc_out<sc_uint<9>> counter_out;
	sc_in<sc_uint<9>> counter_in;

	//-- Local variables --//
	sc_uint<8> count = 0;

	//-- The logic --//
	void increment() {
		if(reset.read() == 1) {
			count = 0;
			counter_out.write(count);
		} else if (enable.read() == 1) {
			count = count + 1;
			counter_out.write(count);
		} else if (load.read() == 1) {
			count = counter_in.read();
			counter_out.write(count);
		}
	}

	//-- Constructor --//
	SC_CTOR(program_counter) {
		cout << "New component - Program Counter" << endl;
		SC_METHOD(increment);
		sensitive << reset << clock.pos();
	}
};
