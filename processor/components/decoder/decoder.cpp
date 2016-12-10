#include "systemc.h"

/* Component: Instruction decoder
 *
 * Takes an integer instruction in the format
 * 
 * [ooodddfffsss]
 *
 * And simply returns it's parts:
 *
 * [ooo] [ddd] [fff] [sss]
 *
 * where
 *
 * ooo: opcode
 * ddd: destination
 * fff: first operand
 * sss: second operand
 *
 * **/
SC_MODULE(decoder) {

	//-- Signals --//
	sc_in<sc_uint<64>> inst;	// Instruction

	sc_out<sc_uint<4>> opcode;	// OPCODE
	sc_out<sc_uint<8>> of1;		// Operando fonte 1
	sc_out<sc_uint<8>> of2;		// Operando fonte 2
	sc_out<sc_uint<9>> od;		// Operando destino

	/* Method that decodifies the instruction.
	 * It performs simple arithmetic operations.
	 * **/
	void decode();

	/* Constructor.
	 * */
	SC_CTOR(decoder) {
		cout << "New component - Decoder" << endl;
		SC_METHOD(decode);
		sensitive << inst;
	}
};

void decoder::decode() {
	unsigned long long instruction = inst.read();

	cout << "Processing instruction " << instruction << endl;

	of2.write(instruction % ((unsigned long long) 1e3));
	instruction /= 1000;
	of1.write(instruction % 1000);	
	instruction /= 1000;
	od.write(instruction % 1000);	
	instruction /= 1000;
	opcode.write(instruction);	
}

