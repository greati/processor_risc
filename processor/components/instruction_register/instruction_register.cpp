#include "systemc.h"

/* Component: Instruction register
 *
 * Stores the next instruction to be
 * executed (or not, considering
 * a jump in the pipeline).
 * **/
SC_MODULE(instruction_register){

	//-- Signals --//
	sc_in<sc_uint<64>> instructionIn;
	sc_in<bool> enable;
	sc_in<bool> write;
	sc_in<bool> clock;
	sc_out<sc_uint<64>> instructionOut;

	/* Write/read in/from the register.
	 * * */
	void mem();

	sc_uint<64> instruction;

	SC_HAS_PROCESS(instruction_register);

	/* Constructor.
	 * **/
	instruction_register(sc_module_name _name) : sc_module{_name} {
		cout << "New component - Instruction Register" << endl;
		SC_METHOD(mem);
		sensitive << clock.pos();
	}

};

void instruction_register::mem() {
	if (enable.read() == 1) {
		if (write.read() == 1) {
			instruction = instructionIn.read();		
			instructionOut.write(instruction);
		} else if (write.read() == 0) {
			instructionOut.write(instruction);
		}
	}
}
