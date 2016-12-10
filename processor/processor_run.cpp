#include "systemc.h"

#include "processor.cpp"

#include <sstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>

int sc_main(int argn, char * argc[]){

	cout << "Loading processor..." << endl;
	sleep(1);

	processor p("PROC", argc[1]);
	
	sc_signal<bool> clock;

	p.clock(clock);

	int numberCycles = 0;

	while (not sc_end_of_simulation_invoked()) {
		clock = 0;
		sc_start(1, SC_NS);
		clock = 1;
		sc_start(1, SC_NS);
		numberCycles++;
	}
	
	cout << "\nFinished after " << numberCycles - 4 << " cicles. Final state:\n" << endl;	

	cout << "/******* REGISTERS ********/" << endl;
	p.REGISTERS.print(10);
	cout << "/******* MEMORY ********/" << endl;
	p.DM.print(10);
	return 0;
}
