#include "systemc.h"

/* Multiplexer 2x1.
 *
 * Used for selecting the value used to
 * address the DM when in LD/ST.
 **/
SC_MODULE(multiplex2){
	//-- Signal declarations --//
	sc_in<sc_uint<2>> seletor; 		// Select an input
	sc_in<sc_uint<9>> opd;			// ULA input
	sc_in<sc_uint<8>> of1;			// DM input
	sc_out<sc_uint<9>> saida;		// Out

	/* Method that selects the right input
	 * according to the selector.
	 * **/
	void select();

	/** 
	 * Constructor
	 */
	SC_CTOR(multiplex2) {
		cout << "New component - Multiplexer3" << endl;
		SC_METHOD(select);
		sensitive << seletor << opd << of1;
	}

};

void multiplex2::select() {
	if (seletor.read() == 0) {
		saida.write(opd.read());
	} else if (seletor.read() == 1) {
		int t = of1.read();
		sc_uint<9> a = t;
		saida.write(a);
	}
}

