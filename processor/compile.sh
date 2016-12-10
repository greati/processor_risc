g++ -I. -I$SYSTEMC_HOME/include -L. -L$SYSTEMC_HOME/lib-linux64 processor_run.cpp -o processor_run -lsystemc -lm -std=c++11
