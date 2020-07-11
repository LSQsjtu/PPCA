#include <bits/stdc++.h>
using namespace std;

int main()
{
    return 0;
}

void ADD(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] + x[rs2];
}

void ADDI(int bin[])
{
    int rd, rs1, immediate = 0;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];

    if (bin[31] == 1)
    {
        for (int i = 0; i < 11; i++)
        {
            if (bin[i] == 0)
            {
                immediate += 1 << i;
            }
        }
        immediate = -1 - immediate;
    }
    else
    {
        for (int i = 0; i < 11; i++)
        {
            immediate += bin[i + 20] << i;
        }
    }

    x[rd] = x[rs1] + immediate;
}

void SUB(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] - x[rs2];
}

void LUI(int bin[])
{
    int rd, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    immediate = sum / 4096 * 4096;
    x[rd] = immediate;
}

void AUIPC(int bin[])
{
    int rd, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    immediate = sum % 4096 * 4096;
    pc += immediate;
}

void XOR(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] ^ x[rs2];
}

void XORI(int bin[])
{
    int rd, rs1, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    if (bin[31] == 1)
    {
        for (int i = 0; i < 11; i++)
        {
            if (bin[i] == 0)
            {
                immediate += 1 << i;
            }
        }
        immediate = -1 - immediate;
    }
    else
    {
        for (int i = 0; i < 11; i++)
        {
            immediate += bin[i + 20] << i;
        }
    }
    x[rd] = x[rs1] ^ immediate;
}

void OR(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] | x[rs2];
}

void ORI(int bin[])
{
    int rd, rs1, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    immediate = I_offset(bin);
    x[rd] = x[rs1] | immediate;
}

void AND(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] & x[rs2];
}

void ANDI(int bin[])
{
    int rd, rs1, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    if (bin[31] == 1)
    {
        for (int i = 0; i < 11; i++)
        {
            if (bin[i] == 0)
            {
                immediate += 1 << i;
            }
        }
        immediate = -1 - immediate;
    }
    else
    {
        for (int i = 0; i < 11; i++)
        {
            immediate += bin[i + 20] << i;
        }
    }
    x[rd] = x[rs1] & immediate;
}

void SLL(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] << x[rs2];
}

void SLLI(int bin[])
{
    int rd, rs1, shamt;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    for (int i = 0; i < 6; i++)
    {
        shamt += bin[i + 20] << i;
    }
    x[rd] = x[rs1] << shamt;
}

void SRL(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] =unsigned (x[rs1]) >> x[rs2];
}

void SRLI(int bin[])
{
    int rd, rs1, shamt;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    for (int i = 0; i < 6; i++)
    {
        shamt += bin[i + 20] << i;
    }
    x[rd] = unsigned(x[rs1]) >> shamt;
}

void SRA(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] >> x[rs2];
    //最高位填充
}

void SRAI(int bin[])
{
    int rd, rs1, shamt;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    for (int i = 0; i < 6; i++)
    {
        shamt += bin[i + 20] << i;
    }
    x[rd] = x[rs1] >> shamt;
    //最高位填充
}

void SLT(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = x[rs1] < x[rs2];
}

void SLTI(int bin[])
{
    int rd, rs1, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    for (int i = 0; i < 11; i++) //待
    {
        immediate += bin[i + 20] << i;
    }
    x[rd] = x[rs1] < immediate;
}

void SLTU(int bin[])
{
    int rd, rs1, rs2;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    x[rd] = unsigned(x[rs1]) < unsigned(x[rs2]);
}

void SLTIU(int bin[])
{
    int rd, rs1, immediate;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    for (int i = 0; i < 11; i++)
    {
        immediate += bin[i + 20] << i;
    }
    if (bin[31])
    {
        for (int i = 11; i < 31; i++)
        {
            immediate += 1 << i;
        }
    }
    x[rd] = unsigned(x[rs1]) < immediate;
}

void BEQ(int bin[])
{
    int rs1, rs2;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];

    if (x[rs1] == x[rs2])
    {
        pc += SB_offset(bin);
    }
}

void BNE(int bin[])
{
    int rs1, rs2;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    if (x[rs1] != x[rs2])
    {
        pc += SB_offset(bin);
    }
}

void BLT(int bin[])
{
    int rs1, rs2;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    if (x[rs1] < x[rs2])
    {
        pc += SB_offset(bin);
    }
}

void BGE(int bin[])
{
    int rs1, rs2;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    if (x[rs1] >= x[rs2])
    {
        pc += SB_offset(bin);
    }
}

void BLTU(int bin[])
{
    int rs1, rs2;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    if (unsigned(x[rs1]) < unsigned(x[rs2]))
    {
        pc += SB_offset(bin);
    }
}

void BGEU(int bin[])
{
    int rs1, rs2;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    if (unsigned(x[rs1]) >= unsigned(x[rs2]))
    {
        pc += SB_offset(bin);
    }
}

void JAL(int bin[])
{
    int rd, offset = 0;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    x[rd] = pc + 4;

    pc += offset;
}

void JALR(int bin[])
{
    int rd, rs1;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    x[rd] = pc + 4;

    pc = x[rs1] + I_offset(bin);
    pc = pc & (~1);
}

void LB(int bin[])
{
    int rd, rs1, offset;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    offset = x[rs1] + I_offset(bin);
    char opt = memory[offset];
    x[rd] = int(opt);
}

void LH(int bin[])
{
    int rd, rs1, offset;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    offset = x[rs1] + I_offset(bin);
    char opt = memory[offset + 1];
    x[rd] = int(opt) << 8 + memory[offset];
}

void LW(int bin[])
{
    int rd, rs1, offset;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    offset = x[rs1] + I_offset(bin);
    char opt = memory[offset + 3];
    x[rd] = int(opt) << 24 + int(memory[offset + 2]) << 16 + int(memory[offset + 1]) << 8 + memory[offset];
}

void LBU(int bin[])
{
    int rd, rs1, offset;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    offset = x[rs1] + I_offset(bin);

    x[rd] = memory[offset];
}

void LHU(int bin[])
{
    int rd, rs1, offset;
    rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    offset = x[rs1] + I_offset(bin);

    x[rd] = int(memory[offset + 1]) << 8 + memory[offset];
}

void SB(int bin[])
{
    int rs1, rs2, offset;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    offset = x[rs1] + S_offset(bin);

    int tmp = unsigned(x[rs2]) % (1 << 9);
    memory[offset] = tmp;
    cout << tmp;
}

void SH(int bin[])
{
    int rs1, rs2, offset;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    offset = x[rs1] + S_offset(bin);

    int tmp = unsigned(x[rs2]) & 255;
    memory[offset] = tmp;
    tmp = unsigned(x[rs2]) >> 8 & 255;
    memory[offset + 1] = tmp;
}

void SW(int bin[])
{
    int rs1, rs2, offset;
    rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
    rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
    offset = x[rs1] + S_offset(bin);

    for (int i = 0; i < 4; i++)
    {
        unsigned int tmp = unsigned(x[rs2]) >> (8 * i);
        tmp = tmp & 255;
        memory[offset + i] = tmp;
    }
}
