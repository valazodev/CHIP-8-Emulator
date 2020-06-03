#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

#include <fstream>
#include <string>

class Disassembler
{

public:
     Disassembler();
    ~Disassembler();
    void open_file(std::string path);
    void run();

private:
    struct Impl;
    Impl* pimpl;

};

#endif // DISASSEMBLER_H
