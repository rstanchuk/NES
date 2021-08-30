#ifndef CPU6502_H
#define CPU6502_H

#include <vector>

// These are required for disassembler. If you dont require disassembly
// then just remove the function.
#include <string>
#include <map>

// Emulation Behaviour Logging ======================================
// Uncomment this to create a logfile entry for each clock tick of 
// the CPU. Beware: this slows down emulation considerably and
// generates extremely large files. I recommend "glogg" to view the
// data as it is designed to handle enormous files.
//
//#define LOGMODE // <- Uncomment me to enable logging!

#include <stdio.h>

// Forward declaration of generic communications bus class to
// prevent circular inclusions
class Bus;


// The 6502 Emulation Class. This is it!
class cpu6502 {
	public:
		cpu6502();
		~cpu6502();

	public:
		// CPU Core registers, exposed as public here for ease of access from external
		// examinors. This is all the 6502 has.
		uint8_t  a      = 0x00;		// Accumulator Register
		uint8_t  x      = 0x00;		// X Register
		uint8_t  y      = 0x00;		// Y Register
		uint8_t  stkp   = 0x00;		// Stack Pointer (points to location on bus)
		uint16_t pc     = 0x0000;	// Program Counter
		uint8_t  status = 0x00;		// Status Register
		
		// External event functions. In hardware these represent pins that are asserted
		// to produce a change in state.
		void reset();	// Reset Interrupt - Forces CPU into known state
		void irq();		// Interrupt Request - Executes an instruction at a specific location
		void nmi();		// Non-Maskable Interrupt Request - As above, but cannot be disabled
		void clock();	// Perform one clock cycle's worth of update

		// Indicates the current instruction has completed by returning true. This is
		// a utility function to enable "step-by-step" execution, without manually 
		// clocking every cycle
		bool complete();

		// Link this CPU to a communications bus
		void ConnectBus(Bus *n) { bus = n; }

		// Produces a map of strings, with keys equivalent to instruction start locations
		// in memory, for the specified address range
		std::map<uint16_t, std::string> disassemble(uint16_t nStart, uint16_t nStop);

		// The status register stores 8 flags. Ive enumerated these here for ease
		// of access. You can access the status register directly since its public.
		// The bits have different interpretations depending upon the context and 
		// instruction being executed.
		enum FLAGS6502 {
			C = (1 << 0),	// Carry Bit
			Z = (1 << 1),	// Zero
			I = (1 << 2),	// Disable Interrupts
			D = (1 << 3),	// Decimal Mode (unused in this implementation)
			B = (1 << 4),	// Break
			U = (1 << 5),	// Unused
			V = (1 << 6),	// Overflow
			N = (1 << 7),	// Negative
		};

	private:
		// Convenience functions to access status register
		uint8_t GetFlag(FLAGS6502 f);
		void    SetFlag(FLAGS6502 f, bool v);
		
		// Assisstive variables to facilitate emulation
		uint8_t  fetched     = 0x00;   // Represents the working input value to the ALU
		uint16_t temp        = 0x0000; // A convenience variable used everywhere
		uint16_t addr_abs    = 0x0000; // All used memory addresses end up in here
		uint16_t addr_rel    = 0x00;   // Represents absolute address following a branch
		uint8_t  opcode      = 0x00;   // Is the instruction byte
		uint8_t  cycles      = 0;	   // Counts how many cycles the instruction has remaining
		uint32_t clock_count = 0;	   // A global accumulation of the number of clocks

		// Linkage to the communications bus
		Bus     *bus = nullptr;
		uint8_t read(uint16_t a);
		void    write(uint16_t a, uint8_t d);

		// The read location of data can come from two sources, a memory address, or
		// its immediately available as part of the instruction. This function decides
		// depending on address mode of instruction byte
		uint8_t fetch();

		// This structure and the following vector are used to compile and store
		// the opcode translation table. The 6502 can effectively have 256
		// different instructions. Each of these are stored in a table in numerical
		// order so they can be looked up easily, with no decoding required.
		// Each table entry holds:
		//	Pneumonic : A textual representation of the instruction (used for disassembly)
		//	Opcode Function: A function pointer to the implementation of the opcode
		//	Opcode Address Mode : A function pointer to the implementation of the 
		//						  addressing mechanism used by the instruction
		//	Cycle Count : An integer that represents the base number of clock cycles the
		//				  CPU requires to perform the instruction

		struct INSTRUCTION {
			std::string name;		
			uint8_t     (cpu6502::*operate )(void) = nullptr;
			uint8_t     (cpu6502::*addrmode)(void) = nullptr;
			uint8_t     cycles = 0;
		};

		std::vector<INSTRUCTION> lookup;
		
	private: 
		// Addressing Modes =============================================
		// The 6502 has a variety of addressing modes to access data in 
		// memory, some of which are direct and some are indirect (like
		// pointers in C++). Each opcode contains information about which
		// addressing mode should be employed to facilitate the 
		// instruction, in regards to where it reads/writes the data it
		// uses. The address mode changes the number of bytes that
		// makes up the full instruction, so we implement addressing
		// before executing the instruction, to make sure the program
		// counter is at the correct location, the instruction is
		// primed with the addresses it needs, and the number of clock
		// cycles the instruction requires is calculated. These functions
		// may adjust the number of cycles required depending upon where
		// and how the memory is accessed, so they return the required
		// adjustment.

		uint8_t IMP();	uint8_t IMM();	
		uint8_t ZP0();	uint8_t ZPX();	
		uint8_t ZPY();	uint8_t REL();
		uint8_t ABS();	uint8_t ABX();	
		uint8_t ABY();	uint8_t IND();	
		uint8_t IZX();	uint8_t IZY();

	private: 
		// Opcodes ======================================================
		// There are 56 "legitimate" opcodes provided by the 6502 CPU. I
		// have not modelled "unofficial" opcodes. As each opcode is 
		// defined by 1 byte, there are potentially 256 possible codes.
		// Codes are not used in a "switch case" style on a processor,
		// instead they are repsonisble for switching individual parts of
		// CPU circuits on and off. The opcodes listed here are official, 
		// meaning that the functionality of the chip when provided with
		// these codes is as the developers intended it to be. Unofficial
		// codes will of course also influence the CPU circuitry in 
		// interesting ways, and can be exploited to gain additional
		// functionality!
		//
		// These functions return 0 normally, but some are capable of
		// requiring more clock cycles when executed under certain
		// conditions combined with certain addressing modes. If that is 
		// the case, they return 1.
		//
		// I have included detailed explanations of each function in 
		// the class implementation file. Note they are listed in
		// alphabetical order here for ease of finding.

		uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
		uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
		uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
		uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
		uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
		uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
		uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
		uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
		uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
		uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
		uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
		uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
		uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
		uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

		// I capture all "unofficial" opcodes with this function. It is
		// functionally identical to a NOP
		uint8_t XXX();
};

#endif