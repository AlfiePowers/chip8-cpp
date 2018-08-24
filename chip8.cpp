#include "chip8.h"

void chip8::setDisplay(u_int16_t i, char value){
    display[i] = static_cast<u_int8_t>(value);
}

// Functions for unit tests (direct memory setting)
void chip8::setMemory(u_int16_t location, u_int8_t value) {this->memory[location] = value;}
void chip8::setPC(u_int16_t pc) {this->pc = pc;}
u_int8_t chip8::getDisplay(u_int16_t i) {return display[i];}
u_int8_t chip8::getMemory(u_int16_t i) {return memory[i];}
void chip8::setStack(u_int16_t location, u_int8_t value){this->stack[location] = value;}
u_int8_t chip8::getStack(u_int16_t location) {return stack[location];}
u_int16_t chip8::getPC() {return pc;}
void chip8::setSP(u_int8_t sp) {this->sp = sp;}
void chip8::setRegister(u_int8_t registerindex, u_int8_t value) {register_store[registerindex] = value;}
u_int8_t chip8::getRegister(u_int8_t registerIndex) {return register_store[registerIndex];}

void chip8::init(){
    this->pc = 0x200;
    this->opcode = 0;
    this->ip = 0;
    this->sp = 0;

    char* buffer = (char*) malloc(sizeof(char) * 4096);

    size_t sizeOfROM = fread(buffer, sizeof(char), 4096, file);
    std::cout << "ROM Size: " << sizeOfROM << " - " <<
              (sizeof(char) * 4096) << std::endl;

    for(u_int16_t i = 0; i < 4096; i++){
        memory[i + 512] = (uint8_t)buffer[i]; // Load into memory starting
    }

    fclose(file);
    free(buffer);

    /*
    for(u_int16_t i = 0; i < 2048; i++){
        if(i % 64 == 0)
            std::cout << "" << std::endl << i << ": ";
        std::cout << memory[i] << "";
    }
    std::cout << "" << std::endl;
     */
    std::cout << (0x0F10 & 0x0F00) / 256 << std::endl;
}

void chip8::emulate(int i){
    this->opcode = (memory[pc] << 8 | memory[pc+1]);
    if(i != 0){
        this->opcode = static_cast<u_int16_t>(i);
    }
    std::cout << "Current PC: " << pc << " Opcode: " << (opcode) << std::endl;
    switch(opcode & 0xF000){
        case 0x0000:
            switch(opcode & 0x000F){
                case 0x0000:        // Clear the screen
                    for(u_int16_t di = 0; di < 2048; di++){this->setDisplay(di, 0);}
                    pc+=2;
                    break;
                case 0x000E:        // Return from stack call
                    sp--;
                    this->pc = this->stack[sp];
                    pc+=2;
                    break;
                default:
                    std::cout << "No such Opcode: " << opcode << std::endl;
            }
            break;
        case 0x1000:        // Jump to Address 1NNN
            std::cout << "Jumping to address " << (opcode & 0x0FFF) << std::endl;
            this->pc = static_cast<u_int16_t>(opcode & 0x0FFF);
            break;
        case 0x2000:        // Jump to subroutine at address NNN (max 16 levels)
            std::cout << "Jumping to the subroutine at " << (opcode & 0x0FFF) << std::endl;
            this->stack[sp] = static_cast<u_int8_t>(pc);
            sp++;
            pc = static_cast<u_int16_t>(opcode & 0x0FFF);
            break;
        case 0x3000:        // 3XRR, Skip next instruction if register VX == Constant RR
            std::cout << "Skipping next instruction!" << std::endl;
            register_store[(opcode & 0x0F00) / 256] == (opcode & 0x00FF) ? pc+=4 : pc+=2;
            break;
        case 0x4000:        // 4XRR, Skip next instruction if register VX != Constant RR
            register_store[(opcode & 0x0F00) / 256] != (opcode & 0x00FF) ? pc+=4 : pc +=2;
            break;
        case 0x5000:        // 5XY0, Skip next instruction if register VX == Register VY
            register_store[(opcode & 0x0F00) / 256] == register_store[(opcode & 0x00F0) / 16] ? pc+=4 : pc +=2;
            break;
        case 0x6000:        // 6XRR, Move constant RR to register VX
            register_store[(opcode & 0x0F00) / 256] = static_cast<u_int8_t>(opcode & 0x00FF);
            pc+=2;
            break;
        case 0x7000:        // 7XRR, Add constant RR to register VX
            register_store[(opcode & 0x0F00) / 256] += static_cast<u_int8_t>(opcode & 0x00FF);
            pc+=2;
            break;
        case 0x8000:        // Register specific instructions
            switch(opcode & 0x000F){
                case 0x0000:        // 8XY0, Move Register VY into VX
                    register_store[(opcode & 0x0F00) - 256] = register_store[(opcode & 0x00F0) - 16];
                    pc+=2;
                    break;
                case 0x0001:        // 8XY1, OR Register VY with register VX -> Store in VX
                    register_store[(opcode & 0x0F00) - 256] |= register_store[(opcode & 0x00F0) - 16];
                    pc+=2;
                    break;
                case 0x0002:        // 8XY2, AND Register VY with register VX -> Store in VX
                    register_store[(opcode & 0x0F00) - 256] &= register_store[(opcode & 0x00F0) - 16];
                    pc+=2;
                    break;
                case 0x0003:        // 8XY3, X-OR Register VY with register VX -> Store in VX
                    register_store[(opcode & 0x0F00) - 256] ^= register_store[(opcode & 0x00F0) - 16];
                    pc+=2;
                    break;
                case 0x0004:        // 8XY4, Add Register VY to VX, Store in VX, Carry in VF
                    register_store[(opcode & 0x0F00) - 256] += register_store[(opcode & 0x00F0) - 16];
                    pc+=2;
                    break;
                case 0x0005:        // 8XY5, Sub Register VY from VX, Store in VX Borrow in VF
                    // TODO
                    pc+=2;
                    break;
                case 0x0006:        // 8X06, Shift register VX to the right, LSB goes to VF
                    // TODO
                    pc+=2;
                    break;
                case 0x0007:        // 8XY7, subtract register VX from register VY, store in VX
                    // TODO
                    pc+=2;
                    break;
                case 0x000E:        // 8X0E, shift register VX left, bit 7 stored into register VF
                    // TODO
                    pc+=2;
                    break;
            }
            break;
        case 0x9000:        // 9XY0, skip next instruction if register VX != register VY
            // TODO
            pc+=2;
            break;
        case 0xA000:        // ANNN, Set IR with constant NNN
            // TODO
            pc+=2;
            break;
        case 0xB000:        // BNNN, Jump to address NNN + register V0
            // TODO
            break;
        case 0xC000:        // CXNN, Register VX set to bitwise and between random number AND NN
            // TODO
            pc+=2;
            break;
        case 0xD000:        // DXYN, Draws sprite at screen location (register VX, VY) height n
            // TODO
            pc+=2;
            break;
        case 0xE000:
            switch (opcode & 0xFFF0){
                case 0x000E:        // EX9E, Skip if key (register RX) pressed
                    // TODO
                    pc+=2;
                    break;
                case 0x001:         // EXA1, Skip if key (register RX) not pressed
                    // TODO
                    pc+=2;
                    break;
            }
            break;
        case 0xF000:
            switch (opcode & 0xFF00){
                case 0x0007:        // FR07, Get delay timer into VR
                    // TODO
                    pc+=2;
                    break;
                case 0x000A:        // FR0A, Wait for keypress, put key in register VR
                    // TODO
                    pc+=2;
                    break;
                case 0x0015:        // FR15, Set delay timer to VR
                    // TODO
                    pc+=2;
                    break;
                case 0x0018:        // FR18, Set sound timer to VR
                    // TODO
                    pc+=2;
                    break;
                case 0x001E:        // FR1E, Add register VR to the index register
                    // TODO
                    pc+=2;
                    break;
                case 0x0029:        // FR29, Point IR to the sprite for hex char in vr
                    // TODO
                    pc+=2;
                    break;
                case 0x0033:        // FR33, Store the bcd representation of register VR at location IR, IR + 1, IR + 2
                    // TODO
                    pc+=2;
                    break;
                case 0x0055:        // FR55, Store registers V0-VR at location IR onwards
                    // TODO
                    pc+=2;
                    break;
                case 0x0065:        // FR65, Load registers V0, VR from locations IR onwards
                    // TODO
                    pc+=2;
                    break;
            }
            break;

        default:
            std::cout << "No such instruction at " << pc << " OPCODE: " << opcode << std::endl;
            pc+=2;
            break;
    }
}