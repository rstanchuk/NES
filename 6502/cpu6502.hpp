#include <stdio.h>

class Bus;

class cpu6502 {
    public:
        cpu6502();
        ~cpu6502();

    public:
        enum FLAGS6502 {
            C = (1 << 0),   //Carry
            Z = (1 << 1),   //Zero
            I = (1 << 2),   //Disable Interrupts
            D = (1 << 3),   //Decimal Mode
            B = (1 << 4),   //Break
            U = (1 << 5),   //Unused
            V = (1 << 6),   //Overflow
            N = (1 << 7)    //Negative
        };

        uint8_t  a      = 0x00;		// Accumulator Register
        uint8_t  x      = 0x00;		// X Register
        uint8_t  y      = 0x00;		// Y Register
        uint8_t  stkp   = 0x00;		// Stack Pointer (points to location on bus)
        uint16_t pc     = 0x0000;	// Program Counter
        uint8_t  status = 0x00;		// Status Register

        void ConnectBus(Bus *n) {
            bus = n;
        };

    private:
        Bus *bus = nullptr;
        uint8_t read(uint16_t a);
        void write(uint16_t a, uint8_t d);

        uint8_t GetFlag(FLAGS6502 f);
        void SetFlag(FLAGS6502 f, bool v);
};