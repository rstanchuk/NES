class Bus;

class cpu6502 {
    public:
        cpu6502();
        ~cpu6502();

        void ConnectBus(Bus *n) {
            bus = n;
        };

    private:
        Bus *bus = nullptr;
        uint8_t read(uint16_t a);
        void write(uint16_t a, uint8_t d);
};