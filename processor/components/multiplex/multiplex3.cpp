#include "systemc.h"

/* Multiplexer 3x1.
 *
 * Used for selecting the value
 * to write in the register base.
 **/
SC_MODULE(multiplex3){
	//-- Signal declarations --//
	sc_in<sc_uint<2>> seletor; 		// Select an input
	sc_in<sc_int<32>> saidaULA;		// ULA input
	sc_in<sc_int<32>> dataDM;		// DM input
	sc_in<sc_uint<8>> immediateData; 	// Immediate input
	sc_out<sc_int<32>> saida;		// Out

	/* Method that selects the right input
	 * according to the selector.
	 * **/
	void select();

	/** 
	 * Constructor
	 */
	SC_CTOR(multiplex3) {
		cout << "New component - Multiplexer3" << endl;
		SC_METHOD(select);
		sensitive << seletor << immediateData << saidaULA << dataDM;
	}

};

void multiplex3::select() {
	if (seletor.read() == 0) {
		saida.write(saidaULA.read());
	} else if (seletor.read() == 1) {
		saida.write(dataDM.read());
	} else if (seletor.read() == 2) {
		int t = immediateData.read();
		saida.write(t);
	}
}
