#include <CHIP-8/cpu.h>

int main(int argc, char *argv[])
{
    CPU cpu;
    std::string path;

    // Escribir la ruta del rom entre las comillas
    path = "";

    cpu.open_rom(path);
    cpu.run();
}
