#include <CHIP-8/cpu.h>

// Headers C
#include <cstdlib>
#include <ctime>

// Headers STL
#include <fstream>

using u8 = uint8_t;
using u16 = uint16_t;

CPU::CPU() : display(Display("CHIP-8 Emulator", width, height, 20))
{
    load_fonts();
}

void CPU::load_symbol(u16 address, Font values)
{
    for (unsigned i=0; i<5; ++i)
        RAM[address + i] = values[i];
}

u16 CPU::get_font_address(uint8_t hex_value)
{
    u16 address = 0x100;
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
    std::ifstream rom(path, std::ios::binary);
    if (!rom.is_open())
        return;

    // Limpia la memoria
    for (auto &data : RAM)
        data = 0x0000;

    // Carga el rom a la memoria
    char byte;
    for (size_t i=512; rom.get(byte); ++i) {
        RAM[i] = uint8_t(byte);
    }

    PC = 512;
    SP = 0x0EA0;
    for (;;) execute_opcode();
}

u16 CPU::fetch_opcode ()
{
    auto& pc = PC;
    auto first  = u16(RAM[pc]);
    auto second = u16(RAM[pc+1]);
    auto opcode = u16((first << 8) | second);
    return opcode;
}

uint8_t CPU::first_hex (const u16& opcode)
{
    return uint8_t(opcode >> 12);
}

uint8_t CPU::second_hex (const u16& opcode)
{
    return uint8_t((opcode & 0x0FFF) >> 8);
}

uint8_t CPU::third_hex (const u16& opcode)
{
    return uint8_t((opcode & 0x00FF) >> 4);
}

uint8_t CPU::fourth_hex (const u16& opcode)
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

u16 CPU::stack_top ()
{
    auto& sp = SP;
    auto first  = u16(RAM[sp]);
    auto second = u16(RAM[sp+1]);
    auto top = u16((first << 8) | second);
    return top;
}

void CPU::stack_pop ()
{
    SP -= 2;
}

void CPU::stack_push (u16 address)
{
    auto& sp = SP;
    sp += 2;
    RAM[sp] = address >> 8;
    RAM[sp+1] = address & 0x00FF;
}
/*
void CPU::jump_to (u16 address)
{
    PC = address;
}

void CPU::call_subroutine (u16 address)
{
    stack_push(PC + 2);
    jump_to(address);
}

void CPU::return_from_call ()
{
    jump_to(stack_top());
    stack_pop();
}

void CPU::skip_next_opcode ()
{
    jump_to(PC + 4);
}

uint8_t CPU::add (uint8_t a, uint8_t b)
{
    uint8_t& carry = V[0x0F];
    u16 ret = u16(a) + u16(b);
    carry = uint8_t(ret >> 8);
    return uint8_t(ret & 0x00FF);
}

uint8_t CPU::subtract (uint8_t a, uint8_t b)
{
    uint8_t& no_borrow = V[0x0F];

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
    auto& msb = V[0x0F];
    msb = num >> 7;
    return uint8_t((num & 0x7F) << 1);
}

uint8_t CPU::rshift (uint8_t num)
{
    auto& lsb = V[0x0F];
    lsb = num & 0x01;
    return num >> 1;
}

u16 CPU::add_16 (u16 a, u16 b)
{
    uint8_t& overflow = V[0x0F];
    uint32_t ret = uint32_t(a) + uint32_t(b);
    overflow = uint8_t(ret >> 16);
    return u16(ret & 0x0000FFFF);
}

void CPU::draw_sprite (uint8_t x, uint8_t y, uint8_t h)
{
    u16 screen = 0x0F00 + (8 * y) + x;

    V[0xF] = 0;
    for (unsigned i=0; i<h; ++i)
    {
        uint8_t writer = RAM[I + i];
        uint8_t written = RAM[screen + (8 * i)];

        // Bit flipped from set to unset
        if ((writer & written) > 0x00)
            V[0xF] = 1;

        written ^= writer;
        auto sprite = byte_to_sprite(written);
        display.draw(sprite, x, y+i);
    }
}

void CPU::set_BCD (uint8_t binary)
{
    RAM[I + 0] = binary / 100;
    binary %= 100;

    RAM[I + 1] = binary / 10;
    binary %= 10;

    RAM[I + 2] = binary / 1;
}

void CPU::reg_dump (uint8_t top_index)
{
    for (uint8_t i=0; i<=top_index; ++i)
        RAM[I + i] = V[i];
}

void CPU::reg_load (uint8_t top_index)
{
    for (uint8_t i=0; i<=top_index; ++i)
        V[i] = RAM[I + i];
}
*/

void CPU::execute_opcode ()
{
    auto t0 = clock();

    auto old_pc = PC;
    u16 opcode = fetch_opcode();

    SDL_Event e;
    SDL_PollEvent( &e );

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
        if (V[X] == NN) skip_next_opcode();
        break;
    }
    case 0x4: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        if (V[X] != NN) skip_next_opcode();
        break;
    }
    case 0x5: {
        if (fourth_hex(opcode) != 0) break;
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        if (V[X] == V[Y]) skip_next_opcode();
        break;
    }
    case 0x6: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        V[X] = uint8_t(NN);
        break;
    }
    case 0x7: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        V[X] += uint8_t(NN);
        break;
    }
    case 0x8: {
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        auto N = fourth_hex(opcode);

             if (N == 0x00) V[X] = V[Y];
        else if (N == 0x01) V[X] |= V[Y];
        else if (N == 0x02) V[X] &= V[Y];
        else if (N == 0x03) V[X] ^= V[Y];
        else if (N == 0x04) V[X] = add      (V[X], V[Y]);
        else if (N == 0x05) V[X] = subtract (V[X], V[Y]);
        else if (N == 0x06) V[X] = rshift   (V[X]);
        else if (N == 0x07) V[X] = subtract (V[Y], V[X]);
        else if (N == 0x0E) V[X] = lshift   (V[X]);
        break;
    }
    case 0x9: {
        if (fourth_hex(opcode) != 0) break;
        auto X = second_hex(opcode);
        auto Y = third_hex(opcode);
        if (V[X] != V[Y]) skip_next_opcode();
        break;
    }
    case 0xA: {
        I = opcode & 0x0FFF;
        break;
    }
    case 0xB: {
        jump_to(V[0] + (opcode & 0x0FFF));
        break;
    }
    case 0xC: {
        auto X  = second_hex(opcode);
        auto NN = opcode & 0x00FF;
        srand(unsigned(time(nullptr)));
        V[X] = uint8_t(rand() % 256) & NN;
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
            if(display.last_key() == V[X])
                skip_next_opcode();
        if ((opcode & 0x00FF) == 0x00A1)
            if(display.last_key() != V[X])
                skip_next_opcode();
        break;
    }
    case 0xF:
        auto  X = second_hex(opcode);
        auto  second_byte = opcode & 0x00FF;

             if (second_byte == 0x07) V[X] = DT;
        else if (second_byte == 0x0A) V[X] = display.get_key();
        else if (second_byte == 0x15) DT = V[X];
        else if (second_byte == 0x18) ST = V[X];
        else if (second_byte == 0x1E) I = add_16(I, V[X]);
        else if (second_byte == 0x29) I = get_font_address (V[X]);
        else if (second_byte == 0x33) set_BCD (V[X]);
        else if (second_byte == 0x55) reg_dump (X);
        else if (second_byte == 0x65) reg_load (X);
    }
    --ST;
    --DT;

    if (old_pc == PC)
        PC += 2;

    auto t1 = clock();
    auto dif = t1-t0;

    if (dif < clocks_per_update) {
        SDL_Delay(1);
    }
}





















void CPU::CLS ()
{
    // Clears screen

    display.clear();
    for (u16 addr = 0x0F00; addr < 0x0FFF; ++addr)
        RAM[addr] = 0x00;
}

void CPU::RET ()
{
    // Returns

    JP(stack_top());
    stack_pop();
}

void CPU::SYS (u16 addr)
{
    // Calls system subroutine
}

void CPU::CALL (u16 addr)
{
    // Calls subroutine

    stack_push(PC + 2);
    JP(addr);
}

void CPU::JP (u16 addr)
{
    // Jumps to address

    PC = addr;
}

void CPU::JP (u8 offset, u16 addr)
{
    // Jumps to address + offset

    PC = addr + offset;
}

void CPU::DRW (u8 x, u8 y, u8 n)
{
    // Draws sprite to screen

    u16 screen = 0x0F00 + (8 * y) + x;

    V[0xF] = 0x00;
    for (unsigned offset=0; offset<n; ++offset)
    {
        uint8_t written = RAM[screen + (8 * offset)];
        uint8_t writer = RAM[I + offset];

        // Bit flipped from set to unset
        if ((written & writer) > 0x00)
            V[0xF] = 0x01;

        written ^= writer;
        auto sprite = byte_to_sprite(written);
        display.draw(sprite, x, y + offset);
    }
}

void CPU::LD (u16 &a, u16 b)
{
    // Loads a value

    a = b;
}

void CPU::LD (u16 addr, vec bcd)
{
    // Loads values to multiple addresses

    for (u16 i=0; i<bcd.size(); ++i)
        RAM[addr + i] = bcd[i];
}

void CPU::ADD (u16 &a, u16 b)
{
    // Adds two values

    a += b;
}

void CPU::SUB (u8 &a, u8 b)
{
    // Subtracts two values and sets not-borrow flag (VF)

    if (a >= b)
        V[0xF] = 0x01;
    else
        V[0xF] = 0x00;

    a -= b;
}

void CPU::SUBN (u8 &a, u8 b)
{
    // Subtracts two values and sets not-borrow flag (VF)

    if (b >= a)
        V[0xF] = 0x01;
    else
        V[0xF] = 0x00;

    a = b - a;
}

void CPU::OR (u8 &a, u8 b)
{
    // Boolean operator OR

    a |= b;
}

void CPU::AND (u8 &a, u8 b)
{
    // Boolean operator AND

    a &= b;
}

void CPU::XOR (u8 &a, u8 b)
{
    // Boolean operator XOR

    a ^= b;
}

void CPU::SE (u8 a, u8 b)
{
    // Skips instruction if A equals B

    if (a == b)
        JP(PC + 4);
}

void CPU::SNE (u8 a, u8 b)
{
    // Skips instruction if A doesn't equal B

    if (a != b)
        JP(PC + 4);
}

void CPU::SHL (u8 &val)
{
    // Shifts left by 1 bit and saves the lost bit in VF

    V[0xF] = val >> 7;
    val &= 0x7F;
    val <<= 1;
}

void CPU::SHR (u8 &val)
{
    // Shifts right by 1 bit and saves the lost bit in VF

    V[0xF] = val & 0x01;
    val >>= 1;
}

void CPU::RND (u8 &a, u8 b)
{
    // Generates a random number (0-255) and applies AND operator

    srand(unsigned(time(nullptr)));
    a = u8(rand() % 256) & b;
}

void CPU::SKP (u8 key)
{
    // Skips instruction if key is pressed

    if (key == display.last_key())
        JP(PC + 4);
}

void CPU::SKNP (u8 key)
{
    // Skips instruction if key isn't pressed

    if (key != display.last_key())
        JP(PC + 4);
}

CPU::vec CPU::BCD (u8 bin)
{
    // Transforms binary to bcd and stores in vector

    vec bcd(3);

    bcd[0] = bin / 100;
    bin %= 100;
    bcd[1] = bin / 10;
    bin %= 10;
    bcd[2] = bin / 1;

    return bcd;
}

void CPU::ADDC (u8 &a, u8 b)
{
    // Adds two values and stores carry in VF

    if (a + b > 0xFF)
        V[0xF] = 0x01;
    else
        V[0xF] = 0x00;

    a = (u16(a) + u16(b)) & 0x00FF;
}



