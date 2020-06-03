#include <CHIP-8/cpu.h>
#include <CHIP-8/disassembler.h>


int main(int argc, char *argv[])
{
    CPU cpu;
    cpu.open_rom("../Pong.ch8");
    return 0;
}
