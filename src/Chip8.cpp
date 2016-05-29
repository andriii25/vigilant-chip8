include "chip8.hpp"
#include <stdlib.h>
#include <time.h>

chip8::chip8(char * memory) {
  mem = memory;
  srand(time(NULL));
}

chip8::~chip8() {

}

void chip8::run_instruction() {
  uint16_t opcode = mem[PC_reg];
  opcode = opcode<<8 | mem[PC_reg+1];
  if(opcode == 0x00E0) { // CLS -  clear the display
    //TODO: implement
  }
  else if(opcode == 0x00EE) { // RET - Return from subroutine
    PC_reg = mem[SP_reg];
    SP_reg--;

  }
  else if((opcode & 0xF000) == 0x1000) { // JP addr - Jump to lower 14-bit
    uint16_t addr = opcode&0x0FFF;
    PC_reg = addr;
  }
  else if((opcode & 0xF000) == 0x2000) { // CALL addr - Call subroutine
    SP_reg++;
    mem[SP_reg] = PC_reg;
    uint16_t addr = opcode&0x0FFF;
    PC_reg = addr;
  }
  else if((opcode & 0xF000) == 0x3000) { // SE Vx, byte - Skip net if Vx=b
    uint16_t reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content = DATA_reg[reg_index];
    uint8_t op_byte = (opcode&0x00FF);
    if(reg_content == op_byte) {
      PC_reg += 2;
    }
  }
  else if((opcode & 0xF000) == 0x4000) { // SNE Vx, byte - Skip next if Vx!=b
    uint16_t reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content = DATA_reg[reg_index];
    uint8_t op_byte = (opcode&0x00FF);
    if(reg_content != op_byte) {
      PC_reg += 2;
    }
  }
  else if((opcode & 0xF000) == 0x5000) { // SE Vx, Vy - Skip next if Vx=Vy
    uint16_t reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content_x = DATA_reg[reg_index];
    reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];

    if(reg_content_x == reg_content_y) {
      PC_reg += 2;
    }
  }
  else if((opcode & 0xF000) == 0x6000) { // LD Vx, byte - Set Vx = byte
    uint16_t reg_index = (opcode&0x0F00)>>8;
    uint8_t byte = opcode&0x00FF;
    DATA_reg[reg_index] = byte;
  }
  else if((opcode & 0xF000) == 0x7000) { // ADD Vx, byte - Vx = Vx+byte
    uint16_t reg_index = (opcode&0x0F00)>>8;
    uint8_t byte = opcode&0x00FF;
    DATA_reg[reg_index] = DATA_reg[reg_index] + byte;
  }
  else if((opcode & 0xF00F) == 0x8000) { // LD Vx, Vy - Set Vx=Vy
    uint16_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    DATA_reg[reg_index] = reg_content_y;
  }
  else if((opcode & 0xF00F) == 0x8001) { // OR Vx, Vy - Set Vx= Vx OR Vy
    uint16_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    DATA_reg[reg_index] = DATA_reg[reg_index] | reg_content_y;
  }
  else if((opcode & 0xF00F) == 0x8002) { // AND Vx, Vy - Set Vx= Vx AND Vy
    uint16_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    DATA_reg[reg_index] = DATA_reg[reg_index] & reg_content_y;
  }
  else if((opcode & 0xF00F) == 0x8003) { // XOR Vx, Vy - Set Vx= Vx XOR Vy
    uint16_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    DATA_reg[reg_index] = DATA_reg[reg_index] ^ reg_content_y;
  }
  else if((opcode & 0xF00F) == 0x8004) { // ADD Vx, Vy - Set Vx= Vx + Vy
    uint16_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    uint16_t sum = DATA_reg[reg_index] + reg_content_y;
    if(sum > 255) {
      DATA_reg[15] = 1;
    }
    DATA_reg[reg_index] = sum;
  }
  else if((opcode & 0xF00F) == 0x8005) { // SUB Vx, Vy - Set Vx= Vx - Vy
    uint16_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content_x = DATA_reg[reg_index];
    if (reg_content_x > reg_content_y) {
      DATA_reg[15] = 1;
    } else {
      DATA_reg[15] = 0;
    }

    DATA_reg[reg_index] = reg_content_x - reg_content_y;
  }
  else if((opcode & 0xF00F) == 0x8006) { // SHR Vx, Vy - Vx>>1
    uint8_t reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content_x = DATA_reg[reg_index];
    if (reg_content_x & 0x01) {
      DATA_reg[15] = 1;
    } else {
      DATA_reg[15] = 0;
    }

    DATA_reg[reg_index] = reg_content_x>>1;
  }
  else if((opcode & 0xF00F) == 0x8007) { // SUBN Vx, Vy - Vx = Vx - Vy
    uint8_t reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];
    reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content_x = DATA_reg[reg_index];
    if (reg_content_y > reg_content_x) {
      DATA_reg[15] = 1;
    } else {
      DATA_reg[15] = 0;
    }

    DATA_reg[reg_index] = reg_content_y - reg_content_x;
  }
  else if((opcode & 0xF00F) == 0x800E) { // SHR Vx, Vy - Vx = Vx<<1
    uint8_t reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content_x = DATA_reg[reg_index];
    if (reg_content_x & 0x01) {
      DATA_reg[15] = 1;
    } else {
      DATA_reg[15] = 0;
    }

    DATA_reg[reg_index] = reg_content_x<<1;
  }
  else if((opcode & 0xF000) == 0x9000) { // SNE Vx, Vy - Skip next if Vx!=Vy
    uint16_t reg_index = (opcode&0x0F00)>>8;
    uint8_t reg_content_x = DATA_reg[reg_index];
    reg_index = (opcode&0x00F0)>>4;
    uint8_t reg_content_y = DATA_reg[reg_index];

    if(reg_content_x != reg_content_y) {
      PC_reg += 2;
    }
  }
  else if((opcode & 0xF000) == 0xA000) { // LD I, addr - I = addr
    I_reg = opcode & 0x0FFF;
  }
  else if((opcode & 0xF000) == 0xB000) { // JP V0, addr
    uint16_t addr = opcode & 0x0FFF;
    PC_reg = addr + DATA_reg[0];
  }
  else if((opcode & 0xF000) == 0xC000) { //RND Vx, byte - set Vx=random AND b
    uint16_t reg_index = (opcode&0x0F00)>>8;
    DATA_reg[reg_index] = (rand() % 256) & (opcode&0x00FF);
  }
  else if((opcode & 0xF000) == 0xD000) { //DRW Vx, Vy, nibble
    //TODO: implement Graphics
  }
  else if((opcode & 0xF0FF) == 0xE09E) { //SKP Vx - skip if key-input = vX
    //TODO: implement Input
  }
  else if((opcode & 0xF0FF) == 0xE0A1) { //SKNP Vx - skip if key-input != vX
    //TODO: implement Input
  }
  else if((opcode & 0xF0FF) == 0xF007) { // LD Vx, DT - Set vx = delay timer
    //TODO: implement timer register
  }
  else if((opcode & 0xF0FF) == 0xF00A) { // LD Vx, K - wait for key, set in Vx
    //TODO: implement Input
  }
  else if((opcode & 0xF0FF) == 0xF015) { // LD DT, Vx - set delay timer = Vx
    //TODO: implement timer register
  }

}


void chip8::update() {

}
