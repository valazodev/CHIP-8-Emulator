#include <CHIP-8/cpu.h>
#include <CHIP-8/disassembler.h>


int main(int argc, char *argv[])
{
    CPU cpu;
    cpu.open_rom("../Pong.ch8");
    cpu.run();
    /*
    Disassembler d;
    d.open_file("../Pong.ch8");
    d.run();
    */
    return 0;
}
