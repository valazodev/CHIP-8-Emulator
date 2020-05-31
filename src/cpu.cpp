#include <CHIP-8/cpu.h>

// Headers C
#include <cstdlib>
#include <ctime>

// Headers STL
#include <fstream>

CPU::CPU() : display(Display("CHIP-8 Emulator", width, height, 20))
{
    load_fonts();
}

void CPU::load_symbol(uint16_t address, Font values)
{
    for (unsigned i=0; i<5; ++i)
        memory[address + i] = values[i];
}

uint16_t CPU::get_font_address(uint8_t hex_value)
{
    uint16_t address = 0x100;
    address &= (hex_value << 4);
    return address;
}

void CPU::load_fonts()
{
    load_symbol(0x100, Font{0xF0, 0x90, 0x90, 0x90, 0xF0}); /* 0 */
    load_symbol(0x110, Font{0x20, 0x60, 0x20, 0x20, 0x70}); /* 1 */
    load_symbol(0x120, Font{0xF0, 0x10, 0xF0, 0x80, 0xF0}); /* 2 */
    load_symbol(0x130, Font{0xF0, 0x10, 0xF0, 0x10, 0xF0}); /* 3 */
    load_symbol(0x140, Font{0x90, 0x90, 0xF0, 0x10, 0x10}); /* 4 */
    load_symbol(0x150, Font{0xF0, 0x80, 0xF0, 0x10, 0xF0}); /* 5 */
    load_symbol(0x160, Font{0xF0, 0x80, 0xF0, 0x90, 0xF0}); /* 6 */
    load_symbol(0x170, Font{0xF0, 0x10, 0x20, 0x40, 0x40}); /* 7 */
    load_symbol(0x180, Font{0xF0, 0x90, 0xF0, 0x90, 0xF0}); /* 8 */
    load_symbol(0x190, Font{0xF0, 0x90, 0xF0, 0x10, 0xF0}); /* 9 */
    load_symbol(0x1A0, Font{0xF0, 0x90, 0xF0, 0x90, 0x90}); /* A */
    load_symbol(0x1B0, Font{0xE0, 0x90, 0xE0, 0x90, 0xE0}); /* B */
    load_symbol(0x1C0, Font{0xF0, 0x80, 0x80, 0x80, 0xF0}); /* C */
    load_symbol(0x1D0, Font{0xE0, 0x90, 0x90, 0x90, 0xE0}); /* D */
    load_symbol(0x1E0, Font{0xF0, 0x80, 0xF0, 0x80, 0xF0}); /* E */
    load_symbol(0x1F0, Font{0xF0, 0x80, 0xF0, 0x80, 0x80}); /* F */
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

auto CPU::byte_to_sprite(const uint8_t& byte)
{
    Display::Sprite sprite;
    for (uint8_t i=0; i<8; ++i)
        sprite[i] = bool( (byte >> (7-i)) & 0x01 );

    return sprite;
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

uint16_t CPU::add_16 (uint16_t a, uint16_t b)
{
    uint8_t& overflow = data_reg[0x0F];
    uint32_t ret = uint32_t(a) + uint32_t(b);
    overflow = uint8_t(ret >> 16);
    return uint16_t(ret & 0x0000FFFF);
}

void CPU::draw_sprite (uint8_t x, uint8_t y, uint8_t height)
{
    uint16_t screen = 0x0F00 + (8 * y) + x;

    data_reg[0xF] = 0;
    for (unsigned i=0; i<height; ++i)
    {
        uint8_t writer = memory[address_reg + i];
        uint8_t written = memory[screen + (8 * i)];

        // Bit flipped from set to unset
        if ((writer & written) > 0x00)
            data_reg[0xF] = 1;

        written ^= writer;
        auto sprite = byte_to_sprite(written);
        display.draw(sprite, x, y+i);
    }
}

void CPU::set_BCD (uint8_t binary)
{
    memory[address_reg + 0] = binary / 100;
    binary %= 100;

    memory[address_reg + 1] = binary / 10;
    binary %= 10;

    memory[address_reg + 2] = binary / 1;
}

void CPU::reg_dump (uint8_t top_index)
{
    for (uint8_t i=0; i<=top_index; ++i)
        memory[address_reg + i] = data_reg[i];
}

void CPU::reg_load (uint8_t top_index)
{
    for (uint8_t i=0; i<=top_index; ++i)
        data_reg[i] = memory[address_reg + i];
}

void CPU::execute_opcode ()
{
    auto t0 = clock();

    auto old_pc = program_counter;
    uint16_t opcode = fetch_opcode();

    // Tabla de opcodes
    switch( first_hex(opcode) )
    {
    case 0x0:
        if (opcode == 0x00E0) display.clear(); else
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
        draw_sprite(X,Y,N);
        break;
    }
    case 0xE: {
        auto X = second_hex(opcode);
        if ((opcode & 0x00FF) == 0x009E)
            if(key() == data_reg[X])
                skip_next_opcode();
        if ((opcode & 0x00FF) == 0x00A1)
            if(key() != data_reg[X])
                skip_next_opcode();
        break;
    }
    case 0xF: {
        auto& I = address_reg;
        auto  X = second_hex(opcode);
        auto  second_byte = opcode & 0x00FF;
        if (second_byte == 0x07) data_reg[X] = delay_timer;  else
        if (second_byte == 0x0A) data_reg[X] = get_key();    else
        if (second_byte == 0x15) delay_timer = data_reg[X];  else
        if (second_byte == 0x18) sound_timer = data_reg[X];  else
        if (second_byte == 0x1E) I = add_16(I, data_reg[X]); else
        if (second_byte == 0x29) I = get_font_address (data_reg[X]); else
        if (second_byte == 0x33) set_BCD (data_reg[X]); else
        if (second_byte == 0x55) reg_dump (X); else
        if (second_byte == 0x65) reg_load (X);
        break;
    }

    }
    --sound_timer;
    --delay_timer;

    if (old_pc == program_counter)
        program_counter += 2;

    auto t1 = clock();
    auto dif = t1-t0;

    if (dif < clocks_per_update) {
        SDL_Delay(1);
    }
}
