//
// Created by mrporky on 24/08/18.
//

#include "test_class.h"
#include "chip8.h"

// Clears the display
int opcode00E0(chip8* emulator){
    emulator->setDisplay(0x0000, 'a');
    emulator->emulate(0x00E0);

    bool correct = true;
    for(u_int16_t i = 0; i < 2048; i++){
        if(emulator->getDisplay(i) != 0){
            correct = false;
            std::cout << "Failed to clear display at byte: " << i << std::endl;
            break;
        }
    }
    return correct;
}

// Returns from subroutine
int opcode00EE(chip8* emulator){
    emulator->setStack(0, 0x0020);
    emulator->setSP(1);
    emulator->emulate(0x00EE);

    if(emulator->getPC() == 0x0020){
        std::cout << "Successfully returned from subroutine" << std::endl;
        return true;
    }else{
        std::cout << "Failed to return to correct address!" << std::endl;
    }
    return false;
}

// Jumping to addresses
int opcode1000(chip8* emulator){
    emulator->emulate(0x1200);
    if(emulator->getPC() == 0x0200){
        std::cout << "Successfully jumped to specified address" << std::endl;
    }
}

// Calling subroutine at NNN
int opcode2000(chip8* emulator){
    // Check that PC is saved and that the and that PC is changed to Opcode location
    u_int16_t pc_tmp = emulator->getPC();
    emulator->emulate(0x2200);
    if(pc_tmp == emulator->getStack(0) && emulator->getPC() == 0x0200){
        std::cout << "Successfully called subroutine" << std::endl;
        return true;
    }else{
        std::cout << "Failed to call subroutine at location" << emulator->getPC() << " with pc tmp = "
        << pc_tmp << std::endl;
    }
    return false;
}

// Skip next instruction if Vx == kk
int opcode3000(chip8* emulator){
    u_int16_t pc_tmp = emulator->getPC();
    emulator->setRegister(0, 0x0010);
    emulator->emulate(0x3010);

    if(emulator->getPC() == (pc_tmp + 4)){
        pc_tmp = emulator->getPC();
        emulator->emulate(0x3020);
        if(pc_tmp + 2 == emulator->getPC()){
            std::cout << "Successfully completed test of next instruction skipping!" << std::endl;
            return true;
        }
    }else{
        std::cout << "Failed to run skip instruction on equals to constant!" << std::endl;
    }
    return false;
}

// Skip next instruction if Vx = Vy
int opcode5000(chip8* emulator){
    emulator->setRegister(0, 0x0010);
    emulator->setRegister(1, 0x0010);
    u_int16_t pc_tmp = emulator->getPC();
    emulator->emulate(0x5010);
    if(emulator->getPC() == (pc_tmp + 4)){
        std::cout << "Successful run of test of register comparisons" << std::endl;
        return true;
    }else{
        std::cout << "Failed to complete opcode 0x5000 test" << std::endl;
    }
    return false;
}



int main(int args, char** argv){
    FILE *file = fopen(argv[1], "rb");
    chip8 emulator(file);
    emulator.init();

    opcode5000(&emulator);

    /*
    for(int i= 0;true;i++){
        emulator.emulate(0);
    }*/
    return 0;
}
