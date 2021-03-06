#include <CHIP-8/cpu.h>
#include <bitset>
#include <cstdlib>
#include <fstream>

using u8 = uint8_t;
using u16 = uint16_t;
using namespace std;

CPU::CPU(): io(IO("CHIP-8 Emulator", width, height, 15))
{
    DT = 0;
    ST = 0;
    I  = 0x0;
    PC = 0x200;
    SP = 0xEA0;

    for (auto &data : V)
        data = 0x00;
    for (auto &data : RAM)
        data = 0x00;

    init_fonts();

    cpu_timer.start();
    delay_timer.start();
}

void CPU::init_fonts ()
{
    LD(0x100, vec<u8>{0xF0, 0x90, 0x90, 0x90, 0xF0}); // 0
    LD(0x110, vec<u8>{0x20, 0x60, 0x20, 0x20, 0x70}); // 1
    LD(0x120, vec<u8>{0xF0, 0x10, 0xF0, 0x80, 0xF0}); // 2
    LD(0x130, vec<u8>{0xF0, 0x10, 0xF0, 0x10, 0xF0}); // 3
    LD(0x140, vec<u8>{0x90, 0x90, 0xF0, 0x10, 0x10}); // 4
    LD(0x150, vec<u8>{0xF0, 0x80, 0xF0, 0x10, 0xF0}); // 5
    LD(0x160, vec<u8>{0xF0, 0x80, 0xF0, 0x90, 0xF0}); // 6
    LD(0x170, vec<u8>{0xF0, 0x10, 0x20, 0x40, 0x40}); // 7
    LD(0x180, vec<u8>{0xF0, 0x90, 0xF0, 0x90, 0xF0}); // 8
    LD(0x190, vec<u8>{0xF0, 0x90, 0xF0, 0x10, 0xF0}); // 9
    LD(0x1A0, vec<u8>{0xF0, 0x90, 0xF0, 0x90, 0x90}); // A
    LD(0x1B0, vec<u8>{0xE0, 0x90, 0xE0, 0x90, 0xE0}); // B
    LD(0x1C0, vec<u8>{0xF0, 0x80, 0x80, 0x80, 0xF0}); // C
    LD(0x1D0, vec<u8>{0xE0, 0x90, 0x90, 0x90, 0xE0}); // D
    LD(0x1E0, vec<u8>{0xF0, 0x80, 0xF0, 0x80, 0xF0}); // E
    LD(0x1F0, vec<u8>{0xF0, 0x80, 0xF0, 0x80, 0x80}); // F
}

void CPU::open_rom (string path)
{
    ifstream rom(path, ios::binary);
    if (!rom.is_open())
        return;

    // Carga el rom a la memoria
    char byte;
    for (size_t i=0x200; rom.get(byte); ++i)
        RAM[i] = u8(byte);
}

IO::Sprite to_sprite(const u8& byte)
{
    IO::Sprite sprite;

    for (u8 i=0; i<8; ++i) {
        bool bit = (byte >> (7-i)) & 0x01;
        if (bit)
            sprite[i] = 0xFFFFFFFF;
        else
            sprite[i] = 0x000000FF;
    }
    return sprite;
}

char to_char (const u8& hex)
{
    if (hex < 10)
        return char(hex) + '0';

    else if (hex >= 10 && hex < 16)
        return char(hex) - 10 + 'A';

    else
        return char(0);
}

bool CPU::matches(u16 opcode, const char* pattern)
{
    for (auto i=0; i<4; ++i)
    {
        u8 nibble = u8(opcode >> 4*(3-i)) & 0x0F;

        if (pattern[i] != 'n' &&
            pattern[i] != 'x' &&
            pattern[i] != 'y' &&
            pattern[i] != to_char(nibble))
        {
            return false;
        }
    }
    return true;
}

u8& CPU::screen_byte (u8 x, u8 y)
{
    u16 index = (8 * y) + x;
    return (RAM[0xF00 + index]);
}

u16 CPU::stack_top ()
{
    auto first  = u16(RAM[SP - 2]);
    auto second = u16(RAM[SP - 1]);
    auto top = u16((first << 8) | second);
    return top;
}

void CPU::stack_pop ()
{
    SP -= 2;
}

void CPU::stack_push (u16 address)
{
    RAM[SP + 0] = address >> 8;
    RAM[SP + 1] = address & 0x00FF;
    SP += 2;
}

void CPU::run ()
{
    while (true) {
        io.update();
        auto opcode = fetch();
        execute(opcode);
        update_timers();

        if (PC >= 0xEA0)
            exit(EXIT_FAILURE);
    }
}

u16 CPU::fetch ()
{
    u16 opcode = u16(RAM[PC] << 8) | RAM[PC+1];
    return opcode;
}

void CPU::execute (u16 opcode)
{
    u8  x    = (opcode >> 8) & 0x0F;
    u8  y    = (opcode >> 4) & 0x0F;
    u8  n    =  opcode & 0x0F;
    u8  byte =  opcode & 0xFF;
    u16 addr =  opcode & 0x0FFF;

    auto last_PC = PC;

    #define SWITCH(expr)
    #define CASE(expr) if (matches (opcode, expr))
    #define BREAK else

    SWITCH(opcode)
    {
        CASE("00E0") CLS  ();               BREAK
        CASE("00EE") RET  ();               BREAK
        CASE("0nnn") SYS  (addr);           BREAK
        CASE("1nnn") JP   (addr);           BREAK
        CASE("2nnn") CALL (addr);           BREAK
        CASE("3xnn") SE   (V[x], byte);     BREAK
        CASE("4xnn") SNE  (V[x], byte);     BREAK
        CASE("5xy0") SE   (V[x], V[y]);     BREAK
        CASE("6xnn") LD   (V[x], byte);     BREAK
        CASE("7xnn") ADD  (V[x], byte);     BREAK
        CASE("8xy0") LD   (V[x], V[y]);     BREAK
        CASE("8xy1") OR   (V[x], V[y]);     BREAK
        CASE("8xy2") AND  (V[x], V[y]);     BREAK
        CASE("8xy3") XOR  (V[x], V[y]);     BREAK
        CASE("8xy4") ADD  (V[x], V[y]);     BREAK
        CASE("8xy5") SUB  (V[x], V[y]);     BREAK
        CASE("8xy6") SHR  (V[x]);           BREAK
        CASE("8xy7") SUBN (V[x], V[y]);     BREAK
        CASE("8xyE") SHL  (V[x]);           BREAK
        CASE("9xy0") SNE  (V[x], V[y]);     BREAK
        CASE("Annn") LD   (I, addr);        BREAK
        CASE("Bnnn") JP   (V[0], addr);     BREAK
        CASE("Cxnn") RND  (V[x], byte);     BREAK
        CASE("Dxyn") DRW  (V[x], V[y], n);  BREAK
        CASE("Ex9E") SKP  (V[x]);           BREAK
        CASE("ExA1") SKNP (V[x]);           BREAK
        CASE("Fx07") LD   (V[x], DT);       BREAK
        CASE("Fx0A") LD   (V[x], KEY());    BREAK
        CASE("Fx15") LD   (DT, V[x]);       BREAK
        CASE("Fx18") LD   (ST, V[x]);       BREAK
        CASE("Fx1E") ADD  (I, V[x]);        BREAK
        CASE("Fx29") LD   (I, FONT(V[x]));  BREAK
        CASE("Fx33") LD   (I, BCD(V[x]));   BREAK
        CASE("Fx55") LD   (I, RNGV(0,x));   BREAK
        CASE("Fx65") LD   (RNGV(0,x), I);
    }
    #undef SWITCH
    #undef CASE
    #undef BREAK

    if (last_PC == PC)
        PC += 2;
}

void CPU::update_timers ()
{
    while (cpu_timer.getTime() < 1000000/500) {
        if (cpu_timer.getTime() < 1000)
            SDL_Delay(1);
    }
    cpu_timer.start();

    if (delay_timer.getTime() > 1000000/60) {
        if (DT > 0) --DT;
        if (ST > 0) --ST;
        delay_timer.start();
    }
}

void CPU::CLS ()
{
    // Clears screen

    io.clear();
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
    // Calls system subroutine (not implemented)

    auto stop_warning_from_compiler = addr;
    stop_warning_from_compiler++;
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

    V[0xF] = 0x00; // Flag

    for (u8 row=0; row<n; ++row) {
        u8 writer = RAM[I + row];
        if (io.draw(to_sprite(writer), x, y + row))
            V[0xF] = 0x01;
    }
    io.refresh_display();
}

void CPU::LD (u16 &a, u16 b)
{
    // Loads a value

    a = b;
}

void CPU::LD (u8 &a, u8 b)
{
    // Loads a value

    a = b;
}

void CPU::LD (u16 addr, vec<u8*> range)
{
    // Loads values to multiple addresses

    for (u16 i=0; i<range.size(); ++i)
        RAM[addr + i] = *range[i];
}

void CPU::LD (u16 addr, vec<u8> range)
{
    // Loads values to multiple addresses

    for (u16 i=0; i<range.size(); ++i)
        RAM[addr + i] = range[i];
}

void CPU::LD (vec<u8*> range, u16 addr)
{
    // Loads values to multiple addresses

    for (u16 i=0; i<range.size(); ++i)
        *range[i] = RAM[addr + i];
}

void CPU::LD (vec<u8> range, u16 addr)
{
    // Loads values to multiple addresses

    for (u16 i=0; i<range.size(); ++i)
        range[i] = RAM[addr + i];
}

void CPU::ADD (u16 &a, u16 b)
{
    // Adds two values

    a += b;
}

void CPU::ADD (u8 &a, u8 b)
{
    // Adds two values and stores carry in VF

    if (a + b > 0xFF)
        V[0xF] = 0x01;
    else
        V[0xF] = 0x00;

    a = (u16(a) + u16(b)) & 0x00FF;
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

    if (key == io.last_key())
        JP(PC + 4);
}

void CPU::SKNP (u8 key)
{
    // Skips instruction if key isn't pressed

    if (key != io.last_key())
        JP(PC + 4);
}

CPU::vec<u8> CPU::BCD (u8 bin)
{
    // Transforms binary to bcd and stores in vector

    vec<u8> bcd(3);

    bcd[0] = bin / 100;
    bin %= 100;
    bcd[1] = bin / 10;
    bin %= 10;
    bcd[2] = bin / 1;

    return bcd;
}

u16 CPU::FONT (u8 digit)
{
    // Returns the address of a given digit's font

    u16 address = 0x100;
    address &= (digit << 4);
    return address;
}

u8 CPU::KEY ()
{
    // Waits for SDL to get a key event and returns the key

    return io.wait_key();
}

CPU::vec<u8*> CPU::RNGV (u8 lower_bound, u8 upper_bound)
{
    // Returns the range between Va and Vb as a vector

    vec<u8*> range(upper_bound - lower_bound + 1);
    for (u8 i=lower_bound; i<=upper_bound; ++i)
        range[i] = &V[i];

    return range;
}
