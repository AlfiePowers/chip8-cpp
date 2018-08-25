#ifndef CHIP8_CHIP8_H
#define CHIP8_CHIP8_H


#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <bitset>
#include <random>

struct chip8 {
private:
    FILE *file;

    // Delcare the set of physical memory stores of the machine
    u_int8_t memory[4096];
    u_int8_t register_store[16];
    u_int8_t stack[48];

    u_int16_t opcode;
    u_int16_t pc;
    u_int16_t ip;
    u_int8_t sp;

    u_int8_t display[2048];

public:
    chip8(FILE *file){
        assert (file != nullptr);      // Make sure that the user has specified a file to be ran
        this->file = file;
    }

    u_int8_t fontSet[80] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,   // 3
        0x90, 0x90, 0xF0, 0x10, 0x10,   // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,   // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,   // 6
        0xF0, 0x10, 0x20, 0x40, 0x40,   // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,   // 8
        0xF0, 0x90, 0xF0, 0x10, 0x90,   // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,   // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,   // B
        0xF0, 0x80, 0x80, 0x80, 0xF0,   // C
        0xE0, 0x90, 0x90, 0x90, 0xE0,   // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,   // E
        0xF0, 0x80, 0xF0, 0x80, 0x80    // F
    };

    FILE getFile(){return *file;}
    void init();
    void emulate(int i);
    void setDisplay(u_int16_t i, char value);
    u_int8_t getDisplay(u_int16_t i);
    u_int8_t getMemory(u_int16_t i);

    // Functions for unit tests
    void setMemory(u_int16_t location, u_int8_t value);
    void setPC(u_int16_t pc);
    void setStack(u_int16_t location, u_int8_t value);
    u_int8_t getStack(u_int16_t location);
    u_int16_t getPC();
    void setSP(u_int8_t sp);
    void setRegister(u_int8_t registerindex, u_int8_t value);
    u_int8_t getRegister(u_int8_t registerIndex);
};


#endif //CHIP8_CHIP8_H
