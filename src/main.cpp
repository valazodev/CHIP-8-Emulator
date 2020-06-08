#include <CHIP-8/cpu.h>
#include <CHIP-8/disassembler.h>


int main(int argc, char *argv[])
{
    /*Disassembler d;
    d.open_file("../Test.ch8");
    d.run();*/

    CPU cpu;
    cpu.open_rom("../Pong.ch8");
    cpu.run();
}
