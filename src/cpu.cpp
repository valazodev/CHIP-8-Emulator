#include <CHIP-8/cpu.h>

// Headers C
#include <cstdlib>
#include <ctime>

// Headers STL
#include <fstream>

CPU::CPU()
{
    display = std::make_unique <Display> ("CHIP-8 Emulator", width, height);
}

void CPU::open_rom (std::string path)
{
    std::ifstream rom(path);
    if (!rom.is_open())
        return;

    // Limpia la memoria
    for (auto &data : memory)
        data = 0x0000;

    // Carga el rom a la memoria
    for (size_t i=512; rom>>memory[i]; ++i)

    program_counter = 512;
    stack_pointer = 0x0EA0;
    for (;;) execute_opcode();
}

uint16_t CPU::fetch_opcode ()
{
    auto& pc = program_counter;
    auto first  = uint16_t(memory[pc]);
    auto second = uint16_t(memory[pc+1]);
    auto opcode = uint16_t((first << 8) | second);
    return opcode;
}

uint8_t CPU::first_hex (const uint16_t& opcode)
{
    return uint8_t(opcode >> 12);
}

uint8_t CPU::second_hex (const uint16_t& opcode)
{
    return uint8_t((opcode & 0x0FFF) >> 8);
}

uint8_t CPU::third_hex (const uint16_t& opcode)
{
    return uint8_t((opcode & 0x00FF) >> 4);
}

uint8_t CPU::fourth_hex (const uint16_t& opcode)
{
    return uint8_t(opcode & 0x000F);
}

uint16_t CPU::stack_top ()
{
    auto& sp = stack_pointer;
    auto first  = uint16_t(memory[sp]);
    auto second = uint16_t(memory[sp+1]);
    auto top = uint16_t((first << 8) | second);
    return top;
}

void CPU::stack_pop ()
{
    stack_pointer -= 2;
}

void CPU::stack_push (uint16_t address)
{
    auto& sp = stack_pointer;
    sp += 2;
    memory[sp] = address >> 8;
    memory[sp+1] = address & 0x00FF;
}

void CPU::jump_to (uint16_t address)
{
    program_counter = address;
}

void CPU::call_subroutine (uint16_t address)
{
    stack_push(program_counter + 2);
    jump_to(address);
}

void CPU::return_from_call ()
{
    jump_to(stack_top());
    stack_pop();
}

void CPU::skip_next_opcode ()
{
    jump_to(program_counter + 4);
}

uint8_t CPU::add (uint8_t a, uint8_t b)
{
    uint8_t& carry = data_reg[0x0F];
    uint16_t ret = uint16_t(a) + uint16_t(b);
    carry = uint8_t(ret >> 8);
    return uint8_t(ret & 0x00FF);
}

uint8_t CPU::subtract (uint8_t a, uint8_t b)
{
    uint8_t& no_borrow = data_reg[0x0F];

    if (a >= b) {
        no_borrow = 0x01;
        return a - b;
    } else {
        no_borrow = 0x00;
        return b - a;
    }
}

uint8_t CPU::lshift (uint8_t num)
{
    auto& msb = data_reg[0x0F];
    msb = num >> 7;
    return uint8_t((num & 0x7F) << 1);
}

uint8_t CPU::rshift (uint8_t num)
{
    auto& lsb = data_reg[0x0F];
    lsb = num & 0x01;
    return num >> 1;
}

void CPU::draw_sprite (uint8_t x, uint8_t y, uint8_t height)
{
    auto& par = pixel_aspect_ratio;
    uint16_t address = 0x0F00 + (par * y) + x;
    for (auto i=0; i<height; ++i) {
        memory[address] = 1*
    }
    display->draw(x, y, height);
}

void CPU::execute_opcode ()
{
    auto old_pc = program_counter;
    uint16_t opcode = fetch_opcode();

    // Tabla de opcodes
    switch( first_hex(opcode) )
    {
    case 0x0:
        if (opcode == 0x00E0) display->clear(); else
        if (opcode == 0x00EE) return_from_call();
        break;

    case 0x1:
        jump_to(opcode & 0x0FFF);
        break;

    case 0x2:
        call_subroutine(opcode & 0x0FFF);
        break;

    case 0x3: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        if (data_reg[X] == NN) skip_next_opcode();
        break;
    }
    case 0x4: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        if (data_reg[X] != NN) skip_next_opcode();
        break;
    }
    case 0x5: {
        if (fourth_hex(opcode) != 0) break;
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        if (data_reg[X] == data_reg[Y]) skip_next_opcode();
        break;
    }
    case 0x6: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        data_reg[X] = uint8_t(NN);
        break;
    }
    case 0x7: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        data_reg[X] += uint8_t(NN);
        break;
    }
    case 0x8: {
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        auto N = fourth_hex(opcode);
        if (N == 0x00) data_reg[X] = data_reg[Y];  else
        if (N == 0x01) data_reg[X] |= data_reg[Y]; else
        if (N == 0x02) data_reg[X] &= data_reg[Y]; else
        if (N == 0x03) data_reg[X] ^= data_reg[Y]; else
        if (N == 0x04) data_reg[X] = add      (data_reg[X], data_reg[Y]); else
        if (N == 0x05) data_reg[X] = subtract (data_reg[X], data_reg[Y]); else
        if (N == 0x06) data_reg[X] = rshift   (data_reg[X]);              else
        if (N == 0x07) data_reg[X] = subtract (data_reg[Y], data_reg[X]); else
        if (N == 0x0E) data_reg[X] = lshift   (data_reg[X]);
        break;
    }
    case 0x9: {
        if (fourth_hex(opcode) != 0) break;
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        if (data_reg[X] != data_reg[Y]) skip_next_opcode();
        break;
    }
    case 0xA: {
        address_reg = opcode & 0x0FFF;
        break;
    }
    case 0xB: {
        jump_to(data_reg[0] + (opcode & 0x0FFF));
        break;
    }
    case 0xC: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        srand(unsigned(time(nullptr)));
        data_reg[X] = uint8_t(rand() % 256) & NN;
        break;
    }
    case 0xD: {
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        auto N = fourth_hex(opcode);
        draw(X,Y,N);
        break;
    }
    case 0xE: {
        break;
    }
    case 0xF: {
        break;
    }

    }
    if (old_pc == program_counter)
        program_counter += 2;
}
