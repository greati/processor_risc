#ifndef _RAM_
#define _RAM_

#include "systemc.h"
#include <vector>
#include <string.h>

template<typename TData, int TSize = 256> 
SC_MODULE(ram) {

	//-- Signals --//
	sc_in<bool> clock;		// Clock
	sc_in<bool> enable;		// Available to read/write
	sc_in<bool> write;		// write if true; read if false
	sc_in<sc_uint<9>> address;	// Addressing the memory for write/read
	sc_in<TData> dataIn; 		// Data to be written/read
	sc_out<TData> dataOut;

	//-- Method --//
	void ram_routine();		// Performs RAM's behaviour
	void print(int);

	//-- Local --//
	std::string moduleName; 	// Only for debugging

	SC_HAS_PROCESS(ram);
	
	//-- Constructor --//
	ram (sc_module_name _name) : sc_module {_name} {
		cout << "New component - RAM - " << _name << endl;
		moduleName = _name;
		SC_THREAD(ram_routine);	
		sensitive << clock.pos();
		// Create memory in memory
		ramdata = new sc_signal<TData>[TSize];
	}

	//-- Method that loads data into the memory --//
	void updateMemory(const std::vector<TData> & _data) {
		for (int i = 0; i < _data.size(); ++i) {
			ramdata[i] = _data[i];
		}
	}

	private:
		sc_signal<TData> * ramdata;	// Keeps the data written

};

template<typename TData, int TSize> 
void ram<TData,TSize>::print(int n) {
	for (int i = 0; i < n; ++i) {
		cout << "[" << i << "] - " << ramdata[i] << endl;
	}	
	cout << endl;
}

template<typename TData, int TSize> 
void ram<TData,TSize>::ram_routine() {
	while (true) {
		wait();
		if (enable.read() == 1) {
			if (write.read() == 1) {
				ramdata[address.read()] = dataIn.read();
			} else {
				dataOut.write(ramdata[address.read()]);
			}
		}
	}
}

#endif
