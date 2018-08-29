//
// Created by mrporky on 24/08/18.
//

#include "chip8.h"

// Clears the display
int opcode00E0(chip8 *emulator) {
    emulator->setDisplay(0x0000, 'a');
    emulator->emulate(0x00E0);

    bool correct = true;
    for (u_int16_t i = 0; i < 2048; i++) {
        if (emulator->getDisplay(i) != 0) {
            correct = false;
            std::cout << "Failed to clear display at byte: " << i << std::endl;
            break;
        }
    }
    return correct;
}

// Returns from subroutine
int opcode00EE(chip8 *emulator) {
    emulator->setStack(0, 0x0020);
    emulator->setSP(1);
    emulator->emulate(0x00EE);

    if (emulator->getPC() == 0x0020) {
        std::cout << "Successfully returned from subroutine" << std::endl;
        return true;
    } else {
        std::cout << "Failed to return to correct address!" << std::endl;
    }
    return false;
}

// Jumping to addresses
int opcode1000(chip8 *emulator) {
    emulator->emulate(0x1200);
    if (emulator->getPC() == 0x0200) {
        std::cout << "Successfully jumped to specified address" << std::endl;
    }
}

// Calling subroutine at NNN
int opcode2000(chip8 *emulator) {
    // Check that PC is saved and that the and that PC is changed to Opcode location
    u_int16_t pc_tmp = emulator->getPC();
    emulator->emulate(0x2200);
    if (pc_tmp == emulator->getStack(0) && emulator->getPC() == 0x0200) {
        std::cout << "Successfully called subroutine" << std::endl;
        return true;
    } else {
        std::cout << "Failed to call subroutine at location" << emulator->getPC() << " with pc tmp = "
                  << pc_tmp << std::endl;
    }
    return false;
}

// Skip next instruction if Vx == kk
int opcode3000(chip8 *emulator) {
    u_int16_t pc_tmp = emulator->getPC();
    emulator->setRegister(0, 0x0010);
    emulator->emulate(0x3010);

    if (emulator->getPC() == (pc_tmp + 4)) {
        pc_tmp = emulator->getPC();
        emulator->emulate(0x3020);
        if (pc_tmp + 2 == emulator->getPC()) {
            std::cout << "Successfully completed test of next instruction skipping!" << std::endl;
            return true;
        }
    } else {
        std::cout << "Failed to run skip instruction on equals to constant!" << std::endl;
    }
    return false;
}

// Skip next instruction if Vx = Vy
int opcode5000(chip8 *emulator) {
    emulator->setRegister(0, 0x0010);
    emulator->setRegister(1, 0x0010);
    u_int16_t pc_tmp = emulator->getPC();
    emulator->emulate(0x5010);
    if (emulator->getPC() == (pc_tmp + 4)) {
        std::cout << "Successful run of test of register comparisons" << std::endl;
        return true;
    } else {
        std::cout << "Failed to complete opcode 0x5000 test" << std::endl;
    }
    return false;
}

// Puts value of kk into register VX
int opcode6000(chip8 *emulator) {
    emulator->emulate(0x6010);
    if (emulator->getRegister(0) == 0x0010) {
        std::cout << "Successfully loaded the value into the register" << std::endl;
        return true;
    }
    return false;
}

// Adds constant to register
int opcode7000(chip8 *emulator) {
    emulator->setRegister(0, 0x0010);
    emulator->emulate(0x7410);
    if (emulator->getRegister(0) == 0x0020) {
        std::cout << "Successfully added value to register 0" << std::endl;
        return true;
    }
    return false;
}

// Stores value of register vy in register vx
int opcode8000(chip8 *emulator) {
    emulator->setRegister(0, 0x0110);
    emulator->setRegister(1, 0x0011);
    emulator->emulate(0x8104);
    if (emulator->getRegister(1) == (emulator->getRegister(0) + 0x0011)) {
        std::cout << "Successfully copied VY into VX" << std::endl;
    } else {
        std::cout << "Failed to copy over VY into VX" << std::endl;
    }
}

int opcodeD000(chip8 *emulator) {
    emulator->setRegister(0, 0x010);
    emulator->emulate(0xD008);

}

int main(int args, char **argv) {
    FILE *file = fopen(argv[1], "rb");
    chip8 emulator(file);
    emulator.init();




    //opcodeD000(&emulator);
    emulator.updateRender();


    while (true) {
        emulator.emulate(0);
    }

    return 0;
}

