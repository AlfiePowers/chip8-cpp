#include "chip8.h"

void chip8::setDisplay(u_int16_t i, char value) {
    display[i] = static_cast<u_int8_t>(value);
}

// Functions for unit tests (direct memory setting)
void chip8::setMemory(u_int16_t location, u_int8_t value) { this->memory[location] = value; }

void chip8::setPC(u_int16_t pc) { this->pc = pc; }

u_int8_t chip8::getDisplay(u_int16_t i) { return display[i]; }

u_int8_t chip8::getMemory(u_int16_t i) { return memory[i]; }

void chip8::setStack(u_int16_t location, u_int8_t value) { this->stack[location] = value; }

u_int8_t chip8::getStack(u_int16_t location) { return stack[location]; }

u_int16_t chip8::getPC() { return pc; }

void chip8::setSP(u_int8_t sp) { this->sp = sp; }

void chip8::setRegister(u_int8_t registerindex, u_int8_t value) { register_store[registerindex] = value; }

u_int8_t chip8::getRegister(u_int8_t registerIndex) { return register_store[registerIndex]; }

void chip8::init() {
    // Set the initial values
    this->pc = 0x200;
    this->opcode = 0;
    this->ip = 0;
    this->sp = 0;

    char *buffer = (char *) malloc(sizeof(char) * 4096);

    size_t sizeOfROM = fread(buffer, sizeof(char), 4096, file);
    std::cout << "ROM Size: " << sizeOfROM << " - " <<
              (sizeof(u_int8_t) * 4096) << std::endl;

    // Loading in the font-set
    for (u_int8_t i = 0; i < 80; i++) {
        memory[i] = fontSet[i];
    }

    // Clearing the screen
    memset(display, 0, 2048);

    // Create the SDL Window:
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SDL2 Pixel Drawing",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, WIDTH, HEIGHT);
    pixels = new Uint32[WIDTH * HEIGHT];
    memset(pixels, 0, WIDTH * HEIGHT * sizeof(Uint32));

    std::cout << "Created window" << std::endl;

    // Load memory into right place
    for (u_int16_t i = 0; i < 4096; i++) {
        memory[i + 512] = (uint8_t) buffer[i]; // Load into memory starting
    }

    // Close the file and free up the buffer
    fclose(file);
    free(buffer);
}

void chip8::emulate(int i) {
    this->opcode = (memory[pc] << 8 | memory[pc + 1]);
    if (i != 0) {
        this->opcode = static_cast<u_int16_t>(i);
    }

    // TODO: Finish this to implement user input
    getKeyboardEvent();

    bool failedpc = false;  // TODO Remove testing var
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000:        // Clear the screen
                    memset(display, 0, 2048);
                    pc += 2;
                    break;
                case 0x000E:        // Return from stack call
                    sp--;
                    this->pc = this->stack[sp];
                    pc += 2;
                    break;
                default:
                    std::cout << "No such Opcode: " << opcode << std::endl;
                    failedpc = true;
            }
            break;
        case 0x1000:        // Jump to Address 1NNN
            //std::cout << "Jumping to address " << (opcode & 0x0FFF) << std::endl;
            this->pc = static_cast<u_int16_t>(opcode & 0x0FFF);
            break;
        case 0x2000:        // Jump to subroutine at address NNN (max 16 levels)
            //std::cout << "Jumping to the subroutine at " << (opcode & 0x0FFF) << std::endl;
            this->stack[sp] = static_cast<u_int8_t>(pc);
            sp++;
            pc = static_cast<u_int16_t>(opcode & 0x0FFF);
            break;
        case 0x3000:        // 3XRR, Skip next instruction if register VX == Constant RR
            //std::cout << "Skipping next instruction!" << std::endl;
            register_store[(opcode & 0x0F00) / 256] == (opcode & 0x00FF) ? pc += 4 : pc += 2;
            break;
        case 0x4000:        // 4XRR, Skip next instruction if register VX != Constant RR
            register_store[(opcode & 0x0F00) / 256] != (opcode & 0x00FF) ? pc += 4 : pc += 2;
            break;
        case 0x5000:        // 5XY0, Skip next instruction if register VX == Register VY
            register_store[(opcode & 0x0F00) / 256] == register_store[(opcode & 0x00F0) / 16] ? pc += 4 : pc += 2;
            break;
        case 0x6000:        // 6XRR, Move constant RR to register VX
            register_store[(opcode & 0x0F00) / 256] = static_cast<u_int8_t>(opcode & 0x00FF);
            pc += 2;
            break;
        case 0x7000:        // 7XRR, Add constant RR to register VX
            register_store[(opcode & 0x0F00) / 256] += static_cast<u_int8_t>(opcode & 0x00FF);
            pc += 2;
            break;
        case 0x8000:        // Register specific instructions
            switch (opcode & 0x000F) {
                case 0x0000:        // 8XY0, Move Register VY into VX
                    register_store[(opcode & 0x0F00) / 256] = register_store[(opcode & 0x00F0) / 16];
                    pc += 2;
                    break;
                case 0x0001:        // 8XY1, OR Register VY with register VX -> Store in VX
                    register_store[(opcode & 0x0F00) / 256] |= register_store[(opcode & 0x00F0) / 16];
                    pc += 2;
                    break;
                case 0x0002:        // 8XY2, AND Register VY with register VX -> Store in VX
                    register_store[(opcode & 0x0F00) / 256] &= register_store[(opcode & 0x00F0) / 16];
                    pc += 2;
                    break;
                case 0x0003:        // 8XY3, X-OR Register VY with register VX -> Store in VX
                    register_store[(opcode & 0x0F00) / 256] ^= register_store[(opcode & 0x00F0) / 16];
                    pc += 2;
                    break;
                case 0x0004:        // 8XY4, Add Register VY to VX, Store in VX, Carry in VF
                    if (register_store[(opcode & 0x0F00) / 256] + register_store[(opcode & 0x00F0) / 16] > 255) {
                        register_store[15] = 1;
                    }
                    register_store[(opcode & 0x0F00) / 256] += register_store[(opcode & 0x00F0) / 16];
                    pc += 2;
                    break;
                case 0x0005:        // 8XY5, Sub Register VY from VX, Store in VX Borrow in VF
                    if (register_store[(opcode & 0x0F00) / 256] > register_store[(opcode & 0x00F0) / 16]) {
                        register_store[15] = 1;
                    }
                    register_store[(opcode & 0x0F00) / 256] -= register_store[(opcode & 0x00F0) / 16];
                    pc += 2;
                    break;
                case 0x0006:        // 8X06, Shift register VX to the right, LSB goes to VF
                    register_store[15] = static_cast<u_int8_t>(register_store[(opcode & 0x0F00) / 256] & 1);
                    register_store[(opcode & 0x0F00) / 256] >>= 1;
                    pc += 2;
                    break;
                case 0x0007:        // 8XY7, subtract register VX from register VY, store in VX
                    if (register_store[(opcode & 0x0F00) / 256] < register_store[(opcode & 0x00F0) / 16]) {
                        register_store[15] = 1;
                    }
                    register_store[(opcode & 0x0F00) / 256] =
                            register_store[(opcode & 0x00F0) / 16] - register_store[(opcode & 0x0F00) / 256];
                    pc += 2;
                    break;
                case 0x000E:        // 8X0E, shift register VX left, bit 7 stored into register VF
                    register_store[15] = static_cast<u_int8_t>(
                            register_store[(opcode & 0x0F00) / 256] >> (sizeof(u_int8_t) * 8 - 1) & 1);
                    register_store[(opcode & 0x0F00) / 256] <<= 1;
                    pc += 2;
                    break;
            }
            break;
        case 0x9000:        // 9XY0, skip next instruction if register VX != register VY
            register_store[(opcode & 0x0F00) / 256] != register_store[(opcode & 0x00F0) / 16] ? pc += 4 : pc += 2;
            break;
        case 0xA000:        // ANNN, Set IR with constant NNN
            ip = static_cast<u_int16_t>(opcode & 0x0FFF);
            pc += 2;
            break;
        case 0xB000:        // BNNN, Jump to address NNN + register V0
            pc = static_cast<u_int16_t>((opcode & 0x0FFF) + register_store[0]);
            break;
        case 0xC000:        // CXNN, Register VX set to bitwise and between random number AND NN
            srand(static_cast<unsigned int>(time(NULL)));
            register_store[(opcode & 0x0F00) / 256] = static_cast<u_int8_t>((rand() % 255) & (opcode & 0x00FF));
            pc += 2;
            break;
        case 0xD000:        // DXYN, Draws sprite at screen location (register VX, VY) height n
        {
            // Store the x and y in separate vars so we don't have to keep referencing the registers
            u_int16_t x = register_store[(opcode & 0x0F00)];
            u_int16_t y = register_store[(opcode & 0x00F0)];
            auto height = static_cast<u_int16_t>(opcode & 0x000F);
            u_int16_t pixel;
            register_store[0xF] = 0;        // Set the flag register to initially be 0
            for (int yline = 0; yline < height; yline++) {
                pixel = memory[ip + yline];     // Get pixel data out of mem
                for (int xline = 0; xline < 8; xline++) {       // Spec says should be 8 bits in length
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (display[(x + xline + ((y + yline) * 64))] == 1) {
                            register_store[0xF] = 1;
                        }
                        if (display[x + xline + ((y + yline) * 64)] == 0) {     // X-OR with check rather than operation
                            draw(static_cast<u_int16_t>(x + xline), static_cast<u_int16_t>(y + yline));
                            display[x + xline + ((y + yline) * 64)] ^= 1;
                        }
                    }
                }
            }
            updateRender();
            pc += 2;
            break;
        }
        case 0xE000:
            switch (opcode & 0xFFF0) {
                case 0x000E:        // EX9E, Skip if key (register RX) pressed
                    // TODO
                    pc += 2;
                    break;
                case 0x001:         // EXA1, Skip if key (register RX) not pressed
                    // TODO
                    pc += 2;
                    break;
            }
            break;
        case 0xF000:
            std::cout << "Current PC: " << pc << " Opcode: " << (opcode) << std::endl;
            switch (opcode & 0x00FF) {
                case 0x0007:        // FR07, Get delay timer into VR
                    // TODO
                    pc += 2;
                    break;
                case 0x000A:        // FR0A, Wait for keypress, put key in register VR
                    // TODO
                    pc += 2;
                    break;
                case 0x0015:        // FR15, Set delay timer to VR
                    // TODO
                    pc += 2;
                    break;
                case 0x0018:        // FR18, Set sound timer to VR
                    // TODO
                    pc += 2;
                    break;
                case 0x001E:        // FR1E, Add register VR to the index register
                    ip += register_store[(opcode & 0x0F00) / 256];
                    pc += 2;
                    break;
                case 0x0029:        // FR29, Point IR to the sprite for hex char in vr
                    ip = static_cast<u_int16_t>(register_store[(opcode & 0x0F00) >> 8] * 0x5);
                    pc += 2;
                    break;
                case 0x0033:        // FR33, Store the bcd representation of register VR at location IR, IR + 1, IR + 2
                    memory[ip] = static_cast<u_int8_t>(register_store[(opcode & 0x0F00) >> 8] / 100);
                    memory[ip + 1] = static_cast<u_int8_t>((register_store[(opcode & 0x0F00) >> 8] / 10) % 10);
                    memory[ip + 2] = static_cast<u_int8_t>((register_store[(opcode & 0x0F00) >> 8] % 100) % 10);
                    pc += 2;
                    break;
                case 0x0055:        // FR55, Store registers V0-VR at location IR onwards
                    for(u_int8_t j = 0; j < (opcode & 0x0F00); j++){
                        memory[ip+j] = register_store[j];
                    }
                    pc += 2;
                    break;
                case 0x0065:        // FR65, Load registers V0, VR from locations IR onwards
                    for(u_int8_t k = 0; k < (opcode & 0x0F00); k++){
                        register_store[k] =  memory[ip+k];
                    }
                    pc += 2;
                    break;
                default:
                    std::cout << "No such opcode" << std::endl;
                    failedpc = true;
                    pc += 2;
            }
            break;

        default:
            std::cout << "No such instruction at " << pc << " OPCODE: " << opcode << std::endl;
            pc += 2;
            break;
    }
    if(failedpc){
        //std::cout << "Current PC: " << pc << " Opcode: " << (opcode) << std::endl;
        failedpc = false;
    }
}

void chip8::draw(u_int16_t x, u_int16_t y) {
    for (u_int8_t i = 0; i < 16; i++) {
        for (u_int8_t j = 0; j < 16; j++) {
            pixels[(j + (y * 16)) * WIDTH + (i + (x * 16))] = 0xFFFFFF;
        }
    }
}

void chip8::updateRender() {
    SDL_UpdateTexture(texture, NULL, pixels, WIDTH * sizeof(Uint32));
    SDL_WaitEvent(&event);


    switch (event.type) {
        case SDL_QUIT:
            break;
        case SDL_SCANCODE_ESCAPE:
            break;
    }
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

SDL_Event chip8::getKeyboardEvent() {

}