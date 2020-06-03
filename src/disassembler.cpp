#include <CHIP-8/disassembler.h>

#include <iostream>
#include <memory>

using namespace std;

struct Disassembler::Impl {
    ifstream file;
    void execute(uint16_t opcode);
    string type(uint16_t opcode);
    bool comp(string a, string b);
};

Disassembler::Disassembler()
{
    pimpl = new Impl();
}

Disassembler::~Disassembler()
{
    delete pimpl;
}

void Disassembler::open_file(string path)
{
    pimpl->file.open(path, ios::binary);
}

void Disassembler::run()
{
    if (!pimpl->file.is_open()) {
        cout << "Archivo no encontrado.\n";
        return;
    }
    char bytes[2];
    while (pimpl->file.read(bytes,2)) {
        uint16_t opcode;
        opcode = uint16_t(reinterpret_cast<uint16_t&>(bytes[0]) << 8);
        opcode |= reinterpret_cast<uint16_t&>(bytes[1]);
        pimpl->execute(opcode);
    }
}

string Disassembler::Impl::type(uint16_t opcode)
{
    string ret = "";
    for (uint8_t i=0; i<4; ++i)
    {
        uint8_t shift = 4*(3-i);
        uint8_t nibble;
        nibble = uint8_t(opcode >> shift);
        nibble = uint8_t(nibble & 0x000F);
        switch (nibble)
        {
            case 0x00: ret += "0"; break;
            case 0x01: ret += "1"; break;
            case 0x02: ret += "2"; break;
            case 0x03: ret += "3"; break;
            case 0x04: ret += "4"; break;
            case 0x05: ret += "5"; break;
            case 0x06: ret += "6"; break;
            case 0x07: ret += "7"; break;
            case 0x08: ret += "8"; break;
            case 0x09: ret += "9"; break;
            case 0x0A: ret += "A"; break;
            case 0x0B: ret += "B"; break;
            case 0x0C: ret += "C"; break;
            case 0x0D: ret += "D"; break;
            case 0x0E: ret += "E"; break;
            case 0x0F: ret += "F"; break;
        }
    }
    return ret;
}

bool Disassembler::Impl::comp(string a, string b)
{
    for (uint8_t i=0; i<4; ++i) {
        switch (b[i]) {
            case 'n': break;
            case 'x': break;
            case 'y': break;
            default:
                if (a[i] != b[i])
                    return false;
        }
    }
    return true;
}

void Disassembler::Impl::execute(uint16_t opcode)
{
    const string op = type(opcode);

    char nibble = op[3];
    char addr[4] = {op[1], op[2], op[3], '\0'};
    char byte[3] = {op[2], op[3], '\0'};
    char Vx[3] = {'V', op[1], '\0'};
    char Vy[3] = {'V', op[2], '\0'};

         if (comp(op,"00E0")) printf( "CLS" );
    else if (comp(op,"00EE")) printf( "RET" );
    else if (comp(op,"0nnn")) printf( "SYS  %s" , addr);
    else if (comp(op,"1nnn")) printf( "JP   %s" , addr);
    else if (comp(op,"2nnn")) printf( "CALL %s" , addr);
    else if (comp(op,"3xnn")) printf( "SE   %s, %s" , Vx, byte);
    else if (comp(op,"4xnn")) printf( "SNE  %s, %s" , Vx, byte);
    else if (comp(op,"5xy0")) printf( "SE   %s, %s" , Vx, Vy);
    else if (comp(op,"6xnn")) printf( "LD   %s, %s" , Vx, byte);
    else if (comp(op,"7xnn")) printf( "ADD  %s, %s" , Vx, byte);
    else if (comp(op,"8xy0")) printf( "LD   %s, %s" , Vx, Vy);
    else if (comp(op,"8xy1")) printf( "OR   %s, %s" , Vx, Vy);
    else if (comp(op,"8xy2")) printf( "AND  %s, %s" , Vx, Vy);
    else if (comp(op,"8xy3")) printf( "XOR  %s, %s" , Vx, Vy);
    else if (comp(op,"8xy4")) printf( "ADD  %s, %s" , Vx, Vy);
    else if (comp(op,"8xy5")) printf( "SUB  %s, %s" , Vx, Vy);
    else if (comp(op,"8xy6")) printf( "SHR  %s, %s" , Vx, Vy);
    else if (comp(op,"8xy7")) printf( "SUBN %s, %s" , Vx, Vy);
    else if (comp(op,"8xyE")) printf( "SHL  %s, %s" , Vx, Vy);
    else if (comp(op,"9xy0")) printf( "SNE  %s, %s" , Vx, Vy);
    else if (comp(op,"Annn")) printf( "LD   I , %s" , addr);
    else if (comp(op,"Bnnn")) printf( "JP   V0, %s" , addr);
    else if (comp(op,"Cxnn")) printf( "RND  %s, %s" , Vx, byte);
    else if (comp(op,"Dxyn")) printf( "DRW  %s, %s, %c" , Vx, Vy, nibble);
    else if (comp(op,"Ex9E")) printf( "SKP  %s" , Vx);
    else if (comp(op,"ExA1")) printf( "SKNP %s" , Vx);
    else if (comp(op,"Fx07")) printf( "LD   %s, DT"  , Vx);
    else if (comp(op,"Fx0A")) printf( "LD   %s, K"   , Vx);
    else if (comp(op,"Fx15")) printf( "LD   DT, %s"  , Vx);
    else if (comp(op,"Fx18")) printf( "LD   ST, %s"  , Vx);
    else if (comp(op,"Fx1E")) printf( "ADD  I , %s"  , Vx);
    else if (comp(op,"Fx29")) printf( "LD   F , %s"  , Vx);
    else if (comp(op,"Fx33")) printf( "LD   B , %s"  , Vx);
    else if (comp(op,"Fx55")) printf( "LD  [I], %s" , Vx);
    else if (comp(op,"Fx65")) printf( "LD   %s, [I]" , Vx);
    else printf("NOP");

    cout << endl;
}
