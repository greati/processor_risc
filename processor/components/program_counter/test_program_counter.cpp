#include "systemc.h"
#include "program_counter.cpp"

int sc_main(int argc, char * argv[]) {

	sc_signal<bool> clock;
	sc_signal<bool> enable;
	sc_signal<bool> reset;
	sc_signal<sc_uint<8> > counter_out;

	// Instance
	program_counter pc("PC");
	pc.clock(clock);
	pc.enable(enable);
	pc.reset(reset);
	pc.counter_out(counter_out);

	// Open VCD file
	sc_trace_file *wf = sc_create_vcd_trace_file("pc");
	sc_trace(wf, clock, "clock");
	sc_trace(wf, reset, "reset");
	sc_trace(wf, enable, "enable");
	sc_trace(wf, counter_out, "counter_out");

	reset = 0; 	// Not reset now
	enable = 0; 	// Not counting now
		
	// Mostrar que mantém 0
	for (int i = 0; i < 5; ++i) {
		clock = 0;
		sc_start();
		clock = 1;
		sc_start();
	}

	// Reset!
	reset = 1;
	cout << sc_time_stamp() << "Asserting reset\n" << endl;
	for (int i = 0; i < 10; ++i) {
		clock = 0;
		sc_start();
		clock = 1;
		sc_start();
	}

	// Undoing Reset!
	reset = 0;
	cout << sc_time_stamp() << "De-asserting reset\n" << endl;
	for (int i = 0; i < 5; ++i) {
		clock = 0;
		sc_start();
		clock = 1;
		sc_start();
	}

	// Enable!
	enable = 1;	
	cout << sc_time_stamp() << "Assert enable!\n" << endl;
	for (int i = 0; i < 256; ++i) {
		clock = 0;
		sc_start();
		clock = 1;
		sc_start();	
	}

	cout << sc_time_stamp() << "De-assert enable!\n" << endl;
	enable = 0;
		
	cout << sc_time_stamp() << "End simulation!\n" << endl;
	sc_close_vcd_trace_file(wf);
	return 0;
}
