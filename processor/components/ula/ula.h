#ifndef _ULA_
#define _ULA_

#include "systemc.h"

SC_MODULE(ula) {

	//-- Sinais --//
	sc_out<sc_int<32>> saida;
	sc_out<bool> N;
	sc_out<bool> Z;
	sc_in<sc_int<32>> a;
	sc_in<sc_int<32>> b;
	sc_in<sc_uint<4>> operacao;
	sc_in<bool> reset; // reset Z e N

	//-- Methods --//
	void compute();

	SC_HAS_PROCESS(ula);
	
	//-- Constructor --//
	ula (sc_module_name _name) : sc_module {_name} {
		SC_METHOD(compute);	
		sensitive << a << b << operacao << reset;
	};

};


void ula::compute(){
	int ar = a.read();
	int br = b.read();
	int op = operacao.read();
	
	if (reset.read() == 1) {
		N.write(0);
		Z.write(0);
	}	

	switch(op){
		case 6:
			saida.write(ar + br);
			break;
		case 7:
			saida.write(ar - br);
			break;
		case 5:
			if(ar == br) {
				saida.write(1);
				Z.write(1);
				N.write(0);
			}
			else if(ar < br) { 
				saida.write(0);
				Z.write(0);
				N.write(1);
			} else { 
				saida.write(2);
				Z.write(0);
				N.write(0);
			}
			break;
		case 1:
			saida.write(ar & br);
			break;
		case 2:
			saida.write(ar | br);
			break;
		case 3:
			saida.write(ar ^ br);
			break;
		case 4:
			saida.write(~ar);
			break;
		default:
			saida.write(0);
			break;
	}
}

#endif
