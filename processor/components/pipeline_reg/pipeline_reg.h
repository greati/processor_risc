#ifndef _PIPELINE_
#define _PIPELINE_

#include "systemc.h"

SC_MODULE(pipeline_reg) {

	//-- Signals --//
	sc_in<bool> clock;		// Clock
	sc_in<bool> enable;		// Available to read/write
	sc_in<bool> write;		// write if 1; read if 0
	sc_in<sc_uint<4>> opcodeIn;	// OPCODE
	sc_in<sc_uint<8>> of1In;	// Operando fonte 1
	sc_in<sc_uint<8>> of2In;	// Operando fonte 2
	sc_in<sc_uint<9>> odIn;		// Operando destino
	sc_out<sc_uint<4>> opcodeOut;	// OPCODE
	sc_out<sc_uint<8>> of1Out;	// Operando fonte 1
	sc_out<sc_uint<8>> of2Out;	// Operando fonte 2
	sc_out<sc_uint<9>> odOut;	// Operando destino

	//-- Method --//
	void pipeline_reg_routine();	// Performs RAM's behaviour

	SC_HAS_PROCESS(pipeline_reg);
	
	//-- Constructor --//
	pipeline_reg (sc_module_name _name) : sc_module {_name} {
		cout << "New component - Pipeline register" << endl;
		SC_THREAD(pipeline_reg_routine);	
		sensitive << clock.pos();
	};

	private:
	//-- Local --//
	int _opcode, _of1, _of2, _od;

};

void pipeline_reg::pipeline_reg_routine() {
	while (true) {
		wait();
		if (enable.read() == 1) {
			if (write.read() == 0) {
				opcodeOut.write(_opcode);
				of1Out.write(_of1);
				of2Out.write(_of2);
				odOut.write(_od);
			} else {
				_opcode = opcodeIn.read();
				_od = odIn.read();
				_of1 = of1In.read();
				_of2 = of2In.read();
				// Write immediately
				opcodeOut.write(_opcode);
				of1Out.write(_of1);
				of2Out.write(_of2);
				odOut.write(_od);
			}
		}
	}
}

#endif
