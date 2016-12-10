#include "systemc.h"

/* Component: Control module
 *
 * Control module of the processor. Responsible
 * for activating/deactivating functions of
 * the other components at each cycle.
 *
 * Implements internally a Mealy's finite state machine.
 * **/
SC_MODULE(control) {

	public:
		//-- Signals --//
		//-- In --//
		sc_in_clk clock;
		sc_in<sc_uint<4> > opcode;	// OPCODE
		sc_in<sc_uint<8> > of1;		// Operando fonte 1
		sc_in<sc_uint<8> > of2;		// Operando fonte 2
		sc_in<sc_uint<9> > opd;		// Operando destino
		sc_in<bool> N;
		sc_in<bool> Z;

		//-- Out --//
		// Program counter control
		sc_out<bool> enableCP;
		sc_out<bool> loadCP;
		sc_out<bool> resetCP;
		sc_out<sc_uint<9>> jumpValueCP;

		// Instruction memory control
		sc_out<bool> enableIM;
		sc_out<bool> writeIM;

		// Data memory control;
		sc_out<bool> enableDM;
		sc_out<bool> writeDM;

		// Instruction register control
		sc_out<bool> enableRI;
		sc_out<bool> writeRI;
		sc_out<sc_uint<8>> immediateValue;
		sc_out<sc_uint<9>> immediateRegister;

		// Register base control
		sc_out<bool> enableRB;
		sc_out<bool> writeRB;

		// Register pipeline
		sc_out<bool> enableRPL;
		sc_out<bool> writeRPL;
		sc_out<bool> resetRPL;

		// ULA control
		sc_out<bool> resetZN;

		// Multiplex for write data on register base
		sc_out<sc_uint<2>> seletorMultiRBW;

		// Multiplex for addressing DM
		sc_out<sc_uint<2>> seletorMultiDM;
	
		//-- Constructor --//
		SC_CTOR(control) {
			SC_METHOD(state_machine);
			sensitive << clock.pos();
		}	

	private:
		//-- Local variables --//
		int curState = 0;
		bool restartPipe = false;

		//-- Methods for making pipeline easy --//

		/*
		 * Prepare components so that, in the next cycle,
		 * the instruction is available on the bus.
		 * * */
		void prepareInstToBus();

		/* Prepare components so that the instruction
		 * is stored in the instruction register in
		 * the next cycle.
		 * * */
		void prepareInstBusToRI();

		/* Prepare components so that the decodified instruction
		 * is written in the pipeline register.
		 * **/
		void prepareRItoRPL();

		/* Make the instruction waits when reaches the cycle
		 * before the pipeline register (for pipeline purposes
		 * only).
		 * **/
		void afterRIWrite();

		/* The Mealy's state machine: uses the current
		 * state and informations from outside in order
		 * to keep the processor working properly in
		 * terms of microinstructions.
		 * **/
		void state_machine();


};

void control::prepareInstToBus() {
	enableIM.write(1);	// Enable IM
	writeIM.write(0);	// Read from IM
	enableCP.write(1);	// Increment counter
}

void control::prepareInstBusToRI() {
	enableIM.write(0);	// Disable IM
	enableRI.write(1);	// Enable RI	
	writeRI.write(1);	// Write IR
	enableCP.write(0);	// Stop incrementing PC
}

void control::prepareRItoRPL() {
	enableRI.write(0);	// Disable Ri
	enableRPL.write(1);	// Enable pipeline register 
	writeRPL.write(1);	// Write pipeline register
}

void control::afterRIWrite() {
	enableRI.write(0);	
}

void control::state_machine() {
	switch(curState) {
		// Write instruction in the bus
		case 0:
			prepareInstToBus();
			curState = 1;
		break;
		// Once the instruction is in the bus, we write it into the RI
		case 1:
			prepareInstBusToRI();
			curState = 2;
		break;
		// Once in the IR and in the bus for decoding, write at pipeline decoded
		case 2:
			if (!restartPipe) {
				prepareRItoRPL();
				curState = 3;
			} else { 
				curState = 0;
				restartPipe = false;
			}
		break;
		// Written in pipeline
		case 3:
			enableRPL.write(0);
			curState = 5;
			prepareInstToBus(); // Take new instruction (pipeline)
		break;
		// Prepare execution
		case 5:
			prepareInstBusToRI(); // Put new instruction on RI (pipeline)
			
			// Load operations: can be LRI (immediate) or LD
			if (opcode.read() == 8 || opcode.read() == 13) {
				enableRB.write(1);
				writeRB.write(1);
				// LRI operation
				if (opcode.read() == 13) {
					immediateRegister.write(opd.read());
					immediateValue.write(of1.read());
					seletorMultiRBW.write(2);
					curState = 9;
				// LD operation
				} else if (opcode.read() == 8) {
					enableDM.write(1);
					writeDM.write(0);
					seletorMultiRBW.write(1);
					seletorMultiDM.write(1);
					curState = 10;
				}
			// ST operation
			} else if (opcode.read() == 9) {
				enableRB.write(1);
				writeRB.write(0);
				curState = 7;
				seletorMultiDM.write(0);
			// J operation
			} else if (opcode.read() == 10) {
				enableCP.write(0);
				loadCP.write(1);
				jumpValueCP.write(opd);
				curState = 8; 
				restartPipe = true;
			// JN operation
			} else if (opcode.read() == 11) {
				if (N.read() == 1) {
					jumpValueCP.write(opd);
					enableCP.write(0);
					loadCP.write(1);
					resetZN.write(1);
					restartPipe = true;
				}
				curState = 8; 
			// JZ operation
			} else if (opcode.read() == 12) {
				if (Z.read() == 1) {
					jumpValueCP.write(opd);
					enableCP.write(0);
					loadCP.write(1);
					resetZN.write(1);
					restartPipe = true;
				}
				curState = 8;
			// ULA operations
			} else if (opcode.read() != 0) {
				seletorMultiRBW.write(0);
				enableRB.write(1);
				writeRB.write(0);
				curState = 6;
			} else if (opcode.read() == 0) {
				sc_stop();
			}
		break;
		// Execute ULA operations
		case 6:
			enableRB.write(1);
			writeRB.write(1);
			curState = 9;
			afterRIWrite(); // Stop pipe propagation
		break;
		// Execute ST operation
		case 7:
			enableDM.write(1);
			writeDM.write(1);
			curState = 9;
			afterRIWrite(); // Stop pipe propagation
		break;
		// Execute jumps
		case 8:
			loadCP.write(0);
			curState = 2;
		break;
		// Store results from ULA operations
		case 9:
			enableRB.write(0);
			enableDM.write(0);
			curState = 2;
			afterRIWrite(); // Stop pipe propagation
		break;
		// Execute LD operation
		case 10:
			enableRB.write(1);
			writeRB.write(1);
			curState = 9;
		break;
	}
}
