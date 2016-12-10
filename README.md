# RISC Processor - SystemC
This is a simple processor implemented in SystemC. It implements a small set of instructions and performs
a trivial pipeline, with only two stages and no true predictions.
 
The instruction word has the form:

| OPCODE | D | F1 | F2 |
|--------|---|----|----|
| 4 bits | 9 bits | 9 bits | 9 bits|
 
And the instruction set is listed below:

| Instruction | Action       | Example                                |
| ----------- |:------------:|:--------------------------------------:|
| AND         | D<-F1 & F2                              | AND 1 2 3   |
| OR          | D<-F1 | F2                              | OR 1 2 3    |
| XOR         | D<-F1 ^ F2                              | XOR 1 2 3   |
| NOT         | D<- ~F1                                 | NOT 1 2 3   |
| CMP         | Z<-1 if F1 == F2, N <-1 if F1 < F2      | CMP 1 2 3   |
| ADD         | D <- F1 + F2                            | ADD 1 2 3   |
| SUB         | D <- F1 - F2                            | SUB 1 2 3   |
| LD          | R[D] <- MEM[F1]                         | LD 1 2      |
| ST          | MEM[F1] <- R[D]                         | ST 1 2      |
| J           | CP <- D                                 | J 1         |
| JN          | CP <- D, if N==1                        | JN 1        |
| JZ          | CP <- D, if Z==1                        | JZ 1        |
| LRI         | R[D] <- F1                              | LRI 1 10    |

Internally, the processor has the following structure (please notice that control signals were omitted for
organization purposes):

![Block diagram](https://github.com/greati/processor_risc/blob/master/report/img/procdiag.png "Processor's diagram")

## Running

First of all, I strongly recommend installing SystemC 2.3.1 following this tutorial: http://chaitulabs.blogspot.com.br/2014/05/systemc-231-installation-in-ubuntu.html.

After that, enter the folder "processor" and execute the file ./compile.h. If it doesn't work, you can try to compile the file processor_run.cpp by yourself, indicating properly where the SystemC's libraries are. 

Finally, execute the ./processor_run program, passing a file with a list of instructions as argument. If you need examples
of algorithms for this processor, take a look at the folder "processor/tests".

Any suggestions or doubts, please refer to my e-mail, greati@ufrn.edu.br.
