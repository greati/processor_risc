#include "systemc.h"
#include "ram.h"

int sc_main(int argn, char * argv[]) {

	//const char * inputFile = "in.text";

	std::vector<sc_int<32>> initRam;

	ram<sc_int<32>,256> memRam("MEMRAM", initRam);

	sc_signal<bool> clock, enable, write;
	sc_signal<int> address;
	sc_signal<sc_int<32>> data;	

	memRam.clock(clock);
	memRam.enable(enable);
	memRam.write(write);
	memRam.address(address);
	memRam.data(data);

	enable = 0;
	write = 0;
	address = 9;
	data = 0;

	cout << data.read() << endl;
	sc_start(0, SC_PS);
	clock = 1;
	enable = 1;
	write = 1;
	data.write(0xF);
	address.write(0x10);	
	sc_start(1, SC_PS);
	clock = 0;
	enable = 0;
	sc_start(1, SC_PS);
	enable = 1;
	write = 0;
	address.write(0x10);
	sc_start(1, SC_PS);
	cout << data.read() << endl;



	return 0;
}
