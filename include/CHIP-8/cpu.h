#ifndef CPU_H
#define CPU_H

#include <array>
#include <ctime>
#include <string>
#include <vector>

#include <CHIP-8/display.h>

class CPU
{
public:
    CPU();
    void open_rom(std::string path);

private:

    using u8 = uint8_t;
    using u16 = uint16_t;
    using Font = std::array <u8,5>;
    using vec = std::vector <u8>;
    using Hz = unsigned;

    const u8 width = 64;
    const u8 height = 32;

    Display display;

    u8 DT;
    u8 ST;
    u16 I;
    u16 SP;
    u16 PC;

    const Hz freq = 500;
    const clock_t ms_per_update = 1000 / freq;
    const clock_t clocks_per_update = CLOCKS_PER_SEC / freq;

    std::array <u8,16>    V;
    std::array <u8,4096>  RAM;

    // Byte splitters
    u8    first_hex       (const u16& opcode);
    u8    second_hex      (const u16& opcode);
    u8    third_hex       (const u16& opcode);
    u8    fourth_hex      (const u16& opcode);
    auto  byte_to_sprite  (const u8& byte);

    /* Opcodes */
    void      execute_opcode  ();
    u16  fetch_opcode    ();

    /* Display */
    void draw_sprite (u8 x, u8 y, u8 height);

    /* BCD */
    void  set_BCD   (u8 binary);

    /* Fonts */
    u16  get_font_address  (u8 hex_value);
    void      load_symbol       (u16 address, Font values);
    void      load_fonts        ();

    /* Stack operations */
    u16  stack_top   ();
    void      stack_pop   ();
    void      stack_push  (u16 address);

    /* Register operations */
    void  reg_dump  (u8 top_index);
    void  reg_load  (u8 top_index);

    /* Flow instructions */
    void  call_subroutine   (u16 address);
    void  jump_to           (u16 address);
    void  return_from_call  ();
    void  skip_next_opcode  ();

    /* Operations involving carry/borrow */
    u8   add       (u8 a, u8 b);
    u8   subtract  (u8 a, u8 b);
    u8   lshift    (u8 num);
    u8   rshift    (u8 num);
    u16  add_16    (u16 a, u16 b);

    // Assembler subroutines
    void  CLS   ();
    void  RET   ();
    void  SYS   (u16 addr);
    void  CALL  (u16 addr);
    void  JP    (u16 addr);
    void  JP    (u8 offset, u16 addr);
    void  DRW   (u8 x, u8 y, u8 nibble);
    void  LD    (u16 addr, vec bcd);
    void  LD    (u16 &a, u16 b);
    void  ADD   (u16 &a, u16 b);
    void  SUB   (u8 &a, u8 b);
    void  SUBN  (u8 &a, u8 b);
    void  OR    (u8 &a, u8 b);
    void  AND   (u8 &a, u8 b);
    void  XOR   (u8 &a, u8 b);
    void  SE    (u8 a, u8 b);
    void  SNE   (u8 a, u8 b);
    void  SHL   (u8 &val);
    void  SHR   (u8 &val);
    void  RND   (u8 &a, u8 b);
    void  SKP   (u8 key);
    void  SKNP  (u8 key);

    // Helper pseudo-subroutines
    vec   BCD   (u8 bin);
    void  ADDC  (u8 &a, u8 b);

};

#endif // CPU_H
