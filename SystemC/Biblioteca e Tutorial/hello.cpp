#include <systemc.h>
SC_MODULE (hello_world) {
	SC_CTOR (hello_world) {
	
	}
	void say_hello() {
		cout << "Hello World.\n";
	}
};

int sc_main(int argc, char* argv[]) {
	hello_world hello("HELLO");
	hello.say_hello();
	return(0);
}
