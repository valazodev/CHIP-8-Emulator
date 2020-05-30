#ifndef CPU_H
#define CPU_H

#include <array>
#include <memory>
#include <string>

#include <CHIP-8/display.h>

class CPU
{
public:
    CPU();
    void open_rom(std::string path);

private:
    using Font = std::array <uint8_t,5>;

    const uint8_t width = 64;
    const uint8_t height = 32;

    Display display;

    uint8_t delay_timer;
    uint8_t sound_timer;
    uint16_t address_reg;
    uint16_t stack_pointer;
    uint16_t program_counter;

    std::array <uint8_t,16>    data_reg;
    std::array <uint8_t,4096>  memory;

    // Byte splitters
    /* DONE */ uint8_t  first_hex       (const uint16_t& opcode);
    /* DONE */ uint8_t  second_hex      (const uint16_t& opcode);
    /* DONE */ uint8_t  third_hex       (const uint16_t& opcode);
    /* DONE */ uint8_t  fourth_hex      (const uint16_t& opcode);
    /* DONE */ auto     byte_to_sprite  (const uint8_t& byte);

    // Opcodes
    void      execute_opcode  ();
    /* DONE */ uint16_t  fetch_opcode    ();

    // Display
    /* DONE */ void draw_sprite (uint8_t x, uint8_t y, uint8_t height);

    // Fonts
    /* DONE */ uint16_t  get_font_address  (uint8_t hex_value);
    /* DONE */ void      load_symbol       (uint16_t address, Font values);
    /* DONE */ void      load_fonts        ();

    // Stack operations
    /* DONE */ uint16_t  stack_top   ();
    /* DONE */ void      stack_pop   ();
    /* DONE */ void      stack_push  (uint16_t address);

    // Flow instructions
    /* DONE */ void  call_subroutine   (uint16_t address);
    /* DONE */ void  jump_to           (uint16_t address);
    /* DONE */ void  return_from_call  ();
    /* DONE */ void  skip_next_opcode  ();

    // Operations involving carry/borrow
    /* DONE */ uint8_t   add       (uint8_t a, uint8_t b);
    /* DONE */ uint8_t   subtract  (uint8_t a, uint8_t b);
    /* DONE */ uint8_t   lshift    (uint8_t num);
    /* DONE */ uint8_t   rshift    (uint8_t num);
    /* DONE */ uint16_t  add_16    (uint16_t a, uint16_t b);
};

#endif // CPU_H
