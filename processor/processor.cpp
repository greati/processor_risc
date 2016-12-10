#include "systemc.h"

#include <sstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <map>
#include <fstream>
#include <stdlib.h>

#include "components/multiplex/multiplex3.cpp"
#include "components/multiplex/multiplex2.cpp"
#include "components/ula/ula.h"
#include "components/program_counter/program_counter.cpp"
#include "components/ram/ram.h"
#include "components/register_base/register_base.h"
#include "components/decoder/decoder.cpp"
#include "components/control/control.cpp"
#include "components/pipeline_reg/pipeline_reg.h"
#include "components/instruction_register/instruction_register.cpp"

sc_uint<64> instStringToInt(const std::string & inst);

/* Transform an instruction string in the format
 * 
 *      [OPCODE] [OD] [OF1] [OF2]
 * 
 * into a unsigned long long in the format
 *
 *      map[OPCODE][OD][OF1][OF2] 
 *
 * Used for convert a instruction from a file
 * to a decodable format.
 * */
sc_uint<64> instStringToInt(const std::string & inst) {
	std::map<std::string, int> m = {
		{"AND",1},
		{"OR", 2},
		{"XOR",3},
		{"NOT",4},
		{"CMP",5},
		{"ADD",6},
		{"SUB",7},
		{"LD", 8},
		{"ST", 9},
		{"J",  10},
		{"JN", 11},
		{"JZ", 12},
		{"LRI",13}};
	sc_uint<64> instInt = 0;
	std::stringstream ss (inst);
	std::string opcode;
	int of1, of2, od;
	// Get OPCODE and OD
	ss >> opcode >> od;
	instInt += od * 1e6;
	instInt += m[opcode] * ((long long int) 1e9);
	if (opcode == "J" || opcode == "JN" || opcode == "JZ")
		return instInt;
	// Get OF1
	ss >> of1;
	instInt += of1 * 1e3;
	if (opcode == "ST" || opcode == "LD" || opcode == "LRI")
		return instInt;
	// Get OF2
	ss >> of2;
	instInt += of2;
	return instInt;
};

SC_MODULE(processor) {

	//-- Local --//
	std::vector<sc_uint<64>> instructions;

	//-- Components --//
	ram<sc_uint<64>,256> IM{"IM"};					// Instruction memory
	ram<sc_int<32>,512> DM{"DM"};					// Data memory
	decoder DEC{"DEC"};						// Decoder
	program_counter PC{"PC"};					// Program counter
	ula ULA{"ULA"};							// ULA
	control CONTROL{"CONTROL"};					// Control
	register_base<sc_int<32>,32> REGISTERS{"REGISTERS"};		// Registers
	multiplex3 MULTIRB{"MULTIRB"};
	multiplex2 MULTIDM{"MULTIDM"};
	pipeline_reg PIPELINESE{"PIPELINESE"}; 				// Pipeline register search-exec 
	instruction_register RI{"INSTREG"};

	//-- Signals --//
	sc_in<bool> clock;	

	//-> Control signals
	sc_signal<bool> sigEnableIM;
	sc_signal<bool> sigWriteIM;
	sc_signal<sc_uint<64>> sigInIM; // nothing!

	sc_signal<bool> sigEnableCP;
	sc_signal<bool> sigLoadCP;
	sc_signal<bool> sigResetCP;

	sc_signal<bool> sigEnableDM;
	sc_signal<bool> sigWriteDM;

	sc_signal<bool> sigEnableRI;
	sc_signal<bool> sigWriteRI;

	sc_signal<sc_uint<8>> sigImmediateValue;
	sc_signal<sc_uint<9>> sigImmediateRegister;

	sc_signal<bool> sigEnableRB;
	sc_signal<bool> sigWriteRB;

	sc_signal<bool> sigEnableRPL;
	sc_signal<bool> sigWriteRPL;
	sc_signal<bool> sigResetRPL;

	sc_signal<bool> sigResetZN;

	sc_signal<sc_uint<2>> sigSeletorMultiRBW;
	sc_signal<sc_uint<2>> sigSeletorMultiDM;

	//-> Other
	sc_signal<sc_uint<4>> sigOpcode;	// OPCODE
	sc_signal<sc_uint<8>> sigOf1;		// Operando fonte 1
	sc_signal<sc_uint<8>> sigOf2;		// Operando fonte 2
	sc_signal<sc_uint<9>> sigOpd;		// Operando destino
	sc_signal<sc_uint<4>> sigOpcodePipe;	// OPCODE
	sc_signal<sc_uint<8>> sigOf1Pipe;	// Operando fonte 1
	sc_signal<sc_uint<8>> sigOf2Pipe;	// Operando fonte 2
	sc_signal<sc_uint<9>> sigOpdPipe;	// Operando destino
	sc_signal<bool> sigZ;
	sc_signal<bool> sigN;

	sc_signal<sc_uint<64>> sigInstWordRI;

	sc_signal<sc_uint<9>> sigInstAddress;
	sc_signal<sc_uint<64>> sigInstWord;

	sc_signal<sc_uint<8>> sigDataAddressDM;
	sc_signal<sc_int<32>> sigDataDM;

	sc_signal<sc_uint<9>> sigCounterIn;

	sc_signal<sc_int<32>> sigSaidaULA;
	sc_signal<sc_int<32>> sigAULA;
	sc_signal<sc_int<32>> sigBULA;

	sc_signal<sc_int<32>> sigSaidaMultiRBW;

	sc_signal<sc_uint<9>> sigSaidaMultiDM;


	/* Constructor that gets the path for an instruction's
	 * file and converts them to integer instructions,
	 * ready for decodification.
	 *
	 * Always called when constructing the processor.
	 * */
	processor(sc_module_name _name, const char * instructionsPath) : sc_module{_name} {
		sleep(1);
		cout << "\nThe processor is ready to work." << endl;
		sleep(1);

		//-- Connections --//
		
		//-> Control
		// In
		CONTROL.clock(clock);
		CONTROL.opcode(sigOpcodePipe);
		CONTROL.of1(sigOf1Pipe);	
		CONTROL.of2(sigOf2Pipe);	
		CONTROL.opd(sigOpdPipe);
		CONTROL.Z(sigZ);
		CONTROL.N(sigN);
		// Out 
		CONTROL.enableIM(sigEnableIM);		
		CONTROL.writeIM(sigWriteIM);		
		CONTROL.enableCP(sigEnableCP);
		CONTROL.loadCP(sigLoadCP);
		CONTROL.resetCP(sigResetCP);
		CONTROL.enableDM(sigEnableDM);
		CONTROL.writeDM(sigWriteDM);
		CONTROL.enableRI(sigEnableRI);
		CONTROL.writeRI(sigWriteRI);
		CONTROL.immediateValue(sigImmediateValue);
		CONTROL.immediateRegister(sigImmediateRegister);
		CONTROL.enableRB(sigEnableRB);
		CONTROL.writeRB(sigWriteRB);
		CONTROL.enableRPL(sigEnableRPL);
		CONTROL.writeRPL(sigWriteRPL);
		CONTROL.resetRPL(sigResetRPL);
		CONTROL.resetZN(sigResetZN);
		CONTROL.seletorMultiRBW(sigSeletorMultiRBW);
		CONTROL.seletorMultiDM(sigSeletorMultiDM);
		CONTROL.jumpValueCP(sigCounterIn);

		//-> Instruction Memory
		// In
		IM.clock(clock);
		IM.enable(sigEnableIM);
		IM.write(sigWriteIM);
		IM.address(sigInstAddress);
		IM.dataIn(sigInIM);
		// Out
		IM.dataOut(sigInstWordRI);

		//-> Instruction register
		// In
		RI.clock(clock);
		RI.enable(sigEnableRI);
		RI.write(sigWriteRI);
		RI.instructionIn(sigInstWordRI);
		// Out
		RI.instructionOut(sigInstWord);

		//-> Data memory
		// In
		DM.clock(clock);
		DM.enable(sigEnableDM);
		DM.write(sigWriteDM);
		DM.address(sigSaidaMultiDM);
		DM.dataIn(sigAULA);
		// Out
		DM.dataOut(sigDataDM);

		//-> Decoder
		// In
		DEC.inst(sigInstWord);
		// Out
		DEC.opcode(sigOpcode);
		DEC.of1(sigOf1);	
		DEC.of2(sigOf2);	
		DEC.od(sigOpd);

		//-> Program counter
		// In
		PC.clock(clock);
		PC.reset(sigResetCP);
		PC.enable(sigEnableCP);
		PC.load(sigLoadCP);
		PC.counter_in(sigCounterIn);
		// Out
		PC.counter_out(sigInstAddress);

		//-> Pipeline register
		// In
		PIPELINESE.clock(clock);
		PIPELINESE.enable(sigEnableRPL);
		PIPELINESE.write(sigWriteRPL);
		PIPELINESE.opcodeIn(sigOpcode);
		PIPELINESE.of1In(sigOf1);	
		PIPELINESE.of2In(sigOf2);	
		PIPELINESE.odIn(sigOpd);
		// Out
		PIPELINESE.opcodeOut(sigOpcodePipe);
		PIPELINESE.of1Out(sigOf1Pipe);	
		PIPELINESE.of2Out(sigOf2Pipe);	
		PIPELINESE.odOut(sigOpdPipe);

		//-> ULA
		// In
		ULA.reset(sigResetZN);
		ULA.a(sigAULA);
		ULA.b(sigBULA);
		ULA.operacao(sigOpcodePipe);
		// Out
		ULA.saida(sigSaidaULA);
		ULA.N(sigN);
		ULA.Z(sigZ);

		//-> Register base
		// In
		REGISTERS.clock(clock);
		REGISTERS.enable(sigEnableRB);
		REGISTERS.write(sigWriteRB);
		REGISTERS.addressSource1(sigOf1Pipe);
		REGISTERS.addressSource2(sigOf2Pipe);
		REGISTERS.addressDest(sigOpdPipe);
		REGISTERS.dataToWrite(sigSaidaMultiRBW); 
		// Out	
		REGISTERS.dataSource1(sigAULA);
		REGISTERS.dataSource2(sigBULA);

		//-> Multiplex for write data on register base
		// In
		MULTIRB.seletor(sigSeletorMultiRBW);
		MULTIRB.saidaULA(sigSaidaULA);
		MULTIRB.dataDM(sigDataDM);
		MULTIRB.immediateData(sigImmediateValue);
		// Out
		MULTIRB.saida(sigSaidaMultiRBW);
	
		//-> Multiplex for addressing DM
		// In
		MULTIDM.seletor(sigSeletorMultiDM);
		MULTIDM.opd(sigOpdPipe);
		MULTIDM.of1(sigOf1Pipe);
		// Out
		MULTIDM.saida(sigSaidaMultiDM);


		//-- Sentivity list --//
		sensitive << clock.pos();
		
		//-- Instruction's file read system --//
		std::ifstream ifs;
		std::string inst;
		int size = 0;
		ifs.open(instructionsPath,std::ifstream::in);

		cout << " ~~~~---------------------------~~~~" << endl;
		cout << "Attempting to find instructions..." << endl;
		sleep(1);
		if (ifs.is_open()) {
			cout << "Found! Reading instructions..." << endl;
			sleep(1);
			while (getline(ifs,inst)) {
				std::size_t found = inst.find("#");
				if(found != std::string::npos && found != 0){
					instructions.push_back(instStringToInt(inst.substr(0, found)));
					size++;
				}else if(found == std::string::npos){
					instructions.push_back(instStringToInt(inst));
					size++;
				}
			}
			cout << "Loading into instruction memory..." << endl;
			sleep(1);
			IM.updateMemory(instructions); // Update memory
			cout << "Done! Instructions to be processed:" << endl;
			for (int i = 0; i < instructions.size(); ++i) {
				cout << instructions[i] << endl;
			}
		} else {
			cout << "Error! Cannot find file with instructions." << endl;
		}
		cout << " ~~~~---------------------------~~~~" << endl << endl;
		sleep(1);
	}
};

