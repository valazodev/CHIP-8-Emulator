#include <CHIP-8/cpu.h>

int main(int argc, char *argv[])
{
    CPU cpu;
    cpu.open_rom("../Pong.ch8");
    cpu.run();
}
