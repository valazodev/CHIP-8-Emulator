#ifndef CPU_H
#define CPU_H

#include <array>
#include <ctime>
#include <string>
#include <vector>

#include <CHIP-8/io.h>

class CPU
{
public:
    CPU();
    void open_rom(std::string path);
    // [[noreturn]]
    void run();

private:
    /* Containers */
    template <typename T>
    using vec = std::vector <T>;
    template <typename T, size_t SIZE>
    using arr = std::array <T, SIZE>;

    /* Types */
    using u8 = uint8_t;
    using u16 = uint16_t;

    /* Constants */
    const u8 width = 64;
    const u8 height = 32;
    const unsigned freq = 500;
    const clock_t ms_per_update = 1000 / freq;
    const clock_t clocks_per_update = CLOCKS_PER_SEC / freq;
    const clock_t CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;

    /* Components */
    IO io;

    u8  DT;   // Delay timer
    u8  ST;   // Sound timer
    u16 SP;   // Stack pointer
    u16 I;    // Address register
    u16 PC;   // Program counter

    arr <u8,16>   V;     // Data register
    arr <u8,4096> RAM;   // Random-access memory

    /* Data */
    u16 opcode;

    /* Clocks */
    clock_t timers_clock;

    /* Helpers */
    void init_fonts  ();
    auto byte2sprite (const u8& byte);
    void uint2str    (u16 val, char* str);
    bool matches     (const char* opcode, const char* pattern);

    /* Control unit */
    void fetch   ();
    void execute ();

    /* Stack operations */
    u16  stack_top  ();
    void stack_pop  ();
    void stack_push (u16 address);

    /* Timer operations */
    void update_timers ();

    /* Assembler subroutines */
    void ADD  (u16 &a, u16 b);
    void ADD  (u8 &a, u8 b);
    void AND  (u8 &a, u8 b);
    void CALL (u16 addr);
    void CLS  ();
    void DRW  (u8 x, u8 y, u8 nibble);
    void RET  ();
    void SYS  (u16 addr);
    void JP   (u16 addr);
    void JP   (u8 offset, u16 addr);
    void LD   (vec<u8*> range, u16 addr);
    void LD   (vec<u8> range, u16 addr);
    void LD   (u16 addr, vec<u8*> range);
    void LD   (u16 addr, vec<u8> range);
    void LD   (u16 &a, u16 b);
    void LD   (u8 &a, u8 b);
    void SUB  (u8 &a, u8 b);
    void SUBN (u8 &a, u8 b);
    void OR   (u8 &a, u8 b);
    void XOR  (u8 &a, u8 b);
    void RND  (u8 &a, u8 b);
    void SE   (u8 a, u8 b);
    void SNE  (u8 a, u8 b);
    void SHL  (u8 &val);
    void SHR  (u8 &val);
    void SKP  (u8 key);
    void SKNP (u8 key);

    /* Helper pseudo-subroutines */
    vec<u8>  BCD  (u8 bin);
    u16      FONT (u8 digit);
    u8       KEY  ();
    vec<u8*> RNGV (u8 lower_bound, u8 upper_bound);

};

#endif // CPU_H
