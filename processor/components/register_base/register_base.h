#ifndef _REGS_
#define _REGS_

#include "systemc.h"

template<typename TData, int TSize = 32> 
SC_MODULE(register_base) {

	//-- Signals --//
	sc_in<bool> clock;			// Clock
	sc_in<bool> enable;			// Available to read/write
	sc_in<bool> write;			// write if true; read if false
	sc_in<sc_uint<8>> addressSource1;	// Addressing first operand
	sc_in<sc_uint<8>> addressSource2;	// Addressing second operand 
	sc_in<sc_uint<9>> addressDest;		// Addressing destination operand 
	sc_in<TData> dataToWrite; 		// Data to be written
	sc_out<TData> dataSource1; 		// Data for the first operand
	sc_out<TData> dataSource2; 		// Data for the second operand

	//-- Method --//
	void register_base_routine();	// Performs RAM's behaviour
	void print(int);

	SC_HAS_PROCESS(register_base);
	
	//-- Constructor --//
	register_base (sc_module_name _name) : sc_module {_name} {
		cout << "New component - Register base" << endl;
		SC_METHOD(register_base_routine);	
		sensitive << clock.pos();
		// Create memory in memory
		regdata = new sc_signal<TData>[TSize];
	};

	private:
		sc_signal<TData> * regdata;	// Keeps the data written

};

template<typename TData, int TSize> 
void register_base<TData,TSize>::print(int n) {
	for (int i = 0; i < n; ++i) {
		cout << "[" << i << "]" << "-" << regdata[i] << endl;
	}
	cout << endl;
}

template<typename TData, int TSize> 
void register_base<TData,TSize>::register_base_routine() {
	if (enable.read() == 1) {
		if (write.read() == 1) {
			regdata[addressDest.read()] = dataToWrite.read();
		} else {
			dataSource1.write(regdata[addressSource1.read()]);
			dataSource2.write(regdata[addressSource2.read()]);
		}
	}
}

#endif
