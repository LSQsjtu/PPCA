#include <bits/stdc++.h>
using namespace std;

ofstream out;
int x[32] = {0}, reg_lock[32] = {0}, pc = 0, memory_lock = 0;
unsigned char memory[0x20000];

enum inst
{
    LUI,
    AUIPC,
    JAL,
    JALR,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    SB,
    SH,
    SW
};

char inst_name[37][8] = {
    "LUI",
    "AUIPC",
    "JAL",
    "JALR",
    "LB",
    "LH",
    "LW",
    "LBU",
    "LHU",
    "ADDI",
    "SLTI",
    "SLTIU",
    "XORI",
    "ORI",
    "ANDI",
    "SLLI",
    "SRLI",
    "SRAI",
    "ADD",
    "SUB",
    "SLL",
    "SLT",
    "SLTU",
    "XOR",
    "SRL",
    "SRA",
    "OR",
    "AND",
    "BEQ",
    "BNE",
    "BLT",
    "BGE",
    "BLTU",
    "BGEU",
    "SB",
    "SH",
    "SW"};

char reg_name[32][5] = {"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void read_in_memory() //读入内存
{
    char c[10] = {'\0'};
    while (cin >> c)
    {
        if (c[0] == '@')
        {
            pc = 0;
            //int len=strlen(c);
            for (int i = 1; i < 9; i++)
            {
                int tmp;
                if (c[i] >= '0' && c[i] <= '9')
                {
                    tmp = c[i] - '0';
                }
                else
                {
                    tmp = c[i] - 'A' + 10;
                }
                pc = (pc << 4) + tmp;
            }
        }
        else
        {
            int x = 0, y = 0;
            if (c[1] >= '0' && c[1] <= '9')
            {
                x = c[1] - '0';
            }
            else
            {
                x = c[1] - 'A' + 10;
            }
            if (c[0] >= '0' && c[0] <= '9')
            {
                y = c[0] - '0';
            }
            else
            {
                y = c[0] - 'A' + 10;
            }
            memory[pc] = 16 * y + x;
            ++pc;
        }
    }
}

int signed_extend(int data, int bits)
{
    if (data & (1 << bits)) //取符号位
        data |= 0xffffffff >> bits << bits;
    return data;
}

int B_offset(int in)
{
    int immediate = 0;
    immediate = (in >> 7) & 30;
    immediate += ((in >> 7) & 1) << 11;
    immediate += ((in >> 25) & 63) << 5;
    immediate += ((in >> 31) & 1) << 12;
    return signed_extend(immediate, 11);
}

int I_offset(int in)
{
    return (in >> 20);
}

int S_offset(int bin[])
{
    int immediate = 0;
    if (bin[31])
    {
        for (int i = 0; i < 5; i++)
        {
            if (bin[i + 7] == 0)
            {
                immediate += 1 << i;
            }
        }
        for (int i = 5; i < 11; i++)
        {
            if (bin[i + 20] == 0)
            {
                immediate += 1 << i;
            }
        }
        immediate = -immediate - 1;
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            if (bin[i + 7])
            {
                immediate += 1 << i;
            }
        }
        for (int i = 5; i < 11; i++)
        {
            if (bin[i + 20])
            {
                immediate += 1 << i;
            }
        }
    }
    return immediate;
}

int J_offset(int bin[])
{
    int immediate = 0;
    if (bin[31])
    {
        immediate = 1;
        for (int i = 1; i < 11; i++)
        {
            if (bin[i + 20] == 0)
            {
                immediate += (1 << i);
            }
        }
        if (bin[20] == 0)
        {
            immediate += 1 << 11;
        }
        for (int i = 12; i < 20; i++)
        {
            if (bin[i] == 0)
            {
                immediate += 1 << i;
            }
        }
        immediate = -immediate - 1;
    }
    else
    {
        for (int i = 1; i < 11; i++)
        {
            if (bin[i + 20])
            {
                immediate += 1 << i;
            }
        }
        if (bin[20])
        {
            immediate += 1 << 11;
        }
        for (int i = 12; i < 20; i++)
        {
            if (bin[i])
            {
                immediate += 1 << i;
            }
        }
    }
    return immediate;
}

struct instruction
{
    int ret, rd, rs1, rs2, immediate, inst, reg_pc, shamt;
    unsigned int sum;
    int *bin;
    bool Three_cycle, jump_flag;

    instruction()
    {
        ret = 0, rd = 0, rs1 = 0, rs2 = 0, immediate = 0, inst = 0, reg_pc = 0, shamt = 0;
        sum = 0;
        bin = new int[32];
        Three_cycle = true;
        jump_flag = 0;
    }

    ~instruction()
    {
        ret = 0, rd = 0, rs1 = 0, rs2 = 0, immediate = 0, inst = 0, reg_pc = 0, shamt = 0;
        sum = 0;
        delete bin;
        Three_cycle = true;
        jump_flag = 0;
    }

    void show_information(){
        //out << "after ID: pc:" << hex << reg_pc << dec << "  inst:" << inst_name[inst] << "\nrd:" << rd << "  rs1:" << rs1 << "  rs2:" << rs2 << "  imm:" << hex << immediate << "  inst:" << sum << endl;
    };

    void end()
    {
        if (sum == unsigned(0x0ff00513))
        {
            cout << (((unsigned int)x[10]) & 255);
            exit(0);
        }
    }

    bool ID()
    {
        int opt = memory[reg_pc] & 127;
        switch (opt)
        {
        case 55: //LUI
            inst = LUI;
            rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            immediate = sum >> 12 << 12;
            break;

        case 23: //AUIPC
            inst = AUIPC;
            rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            immediate = sum >> 12 << 12;
            break;

        case 111: //JAL
            inst = JAL;
            rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            immediate = J_offset(bin);
            break;

        case 103: //JALR
            inst = JALR;
            rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
            immediate = I_offset(sum);
            break;

        case 99: //BEQ……
            rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
            rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
            immediate = B_offset(sum);
            {
                int function = bin[12] + 2 * bin[13] + 4 * bin[14];
                switch (function)
                {
                case 0:
                    inst = BEQ;
                    break;

                case 1:
                    inst = BNE;
                    break;

                case 4:
                    inst = BLT;
                    break;

                case 5:
                    inst = BGE;
                    break;

                case 6:
                    inst = BLTU;
                    break;

                case 7:
                    inst = BGEU;
                    break;

                default:
                    break;
                }
            }
            break;

        case 3: //LB……
            this->rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            this->rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
            immediate = I_offset(sum);
            {
                int function = bin[12] + 2 * bin[13] + 4 * bin[14];
                switch (function)
                {
                case 0:
                    inst = LB;
                    break;

                case 1:
                    inst = LH;
                    break;

                case 2:
                    inst = LW;
                    break;

                case 4:
                    inst = LBU;
                    break;

                case 5:
                    inst = LHU;
                    break;
                }
            }
            break;

        case 35: //SB……
            rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
            rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
            immediate = S_offset(bin);
            {
                int function = bin[12] + 2 * bin[13] + 4 * bin[14];
                switch (function)
                {
                case 0:
                    inst = SB;
                    break;

                case 1:
                    inst = SH;
                    break;

                case 2:
                    inst = SW;
                    break;

                default:
                    break;
                }
            }
            break;

        case 19: //ADDI……
            this->rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            this->rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
            immediate = I_offset(sum);
            {
                int function = bin[12] + 2 * bin[13] + 4 * bin[14];
                switch (function)
                {
                case 0:
                    inst = ADDI;
                    break;

                case 2:
                    inst = SLTI;
                    break;

                case 3:
                    inst = SLTIU;
                    break;

                case 4:
                    inst = XORI;
                    break;

                case 6:
                    inst = ORI;
                    break;

                case 7:
                    inst = ANDI;
                    break;

                case 1:
                    inst = SLLI;
                    for (int i = 0; i < 6; i++)
                    {
                        shamt += bin[i + 20] << i;
                    }
                    break;

                case 5:
                    for (int i = 0; i < 6; i++)
                    {
                        shamt += bin[i + 20] << i;
                    }
                    if (bin[30])
                    {
                        inst = SRAI;
                    }
                    else
                    {
                        inst = SRLI;
                    }
                    break;
                }
            }
            break;

        case 51: //ADD……
            rd = bin[7] + 2 * bin[8] + 4 * bin[9] + 8 * bin[10] + 16 * bin[11];
            rs1 = bin[15] + 2 * bin[16] + 4 * bin[17] + 8 * bin[18] + 16 * bin[19];
            rs2 = bin[20] + 2 * bin[21] + 4 * bin[22] + 8 * bin[23] + 16 * bin[24];
            {
                int function = bin[12] + 2 * bin[13] + 4 * bin[14];
                switch (function)
                {
                case 0:
                    if (bin[30])
                    {
                        inst = SUB;
                    }
                    else
                    {
                        inst = ADD;
                    }
                    break;

                case 1:
                    inst = SLL;
                    break;

                case 2:
                    inst = SLT;
                    break;

                case 3:
                    inst = SLTU;
                    break;

                case 4:
                    inst = XOR;
                    break;

                case 5:
                    if (bin[30])
                    {
                        inst = SRA;
                    }
                    else
                    {
                        inst = SRL;
                    }
                    break;

                case 6:
                    inst = OR;
                    break;
                }
            }
            break;
        }
        if (reg_lock[rs1] || reg_lock[rs2])
        {
            return false;
        }

        if (opt == 99 || opt == 103 || opt == 111)
        {
            memory_lock = 1;
        }
        return true;
    }

    void EXE()
    {
        ret = x[rd];
        switch (inst)
        {
        case LUI:
            ret = immediate;
            break;

        case AUIPC:
            pc = reg_pc + immediate;
            ret = pc;
            jump_flag = true;
            break;

        case JAL:
            ret = reg_pc + 4;
            pc = reg_pc + immediate;
            jump_flag = true;
            break;

        case JALR:
            ret = reg_pc + 4;
            pc = x[rs1] + immediate;
            jump_flag = true;
            pc = pc & (~1);
            break;

        case BEQ:
            if (x[rs1] == x[rs2])
            {
                jump_flag = true;
                pc = reg_pc + immediate;
            }
            break;

        case BNE:
            if (x[rs1] != x[rs2])
            {
                jump_flag = true;
                pc = reg_pc + immediate;
            }
            break;

        case BGE:
            if (x[rs1] >= x[rs2])
            {
                jump_flag = true;
                pc = reg_pc + immediate;
            }
            break;

        case BLT:
            if (x[rs1] < x[rs2])
            {
                jump_flag = true;
                pc = reg_pc + immediate;
            }
            break;

        case BLTU:
            if (unsigned(x[rs1]) < unsigned(x[rs2]))
            {
                jump_flag = true;
                pc = reg_pc + immediate;
            }
            break;

        case BGEU:
            if (unsigned(x[rs1]) >= unsigned(x[rs2]))
            {
                jump_flag = true;
                pc = reg_pc + immediate;
            }
            break;

        case LB:
            immediate += x[rs1];
            break;

        case LH:
            immediate += x[rs1];
            break;

        case LW:
            immediate += x[rs1];
            break;

        case LBU:
            immediate += x[rs1];
            break;

        case LHU:
            immediate += x[rs1];
            break;

        case SB:
            immediate += x[rs1];
            break;

        case SH:
            immediate += x[rs1];
            break;

        case SW:
            immediate += x[rs1];
            break;

        case ADDI:
            ret = x[rs1] + immediate;
            break;

        case SLTI:
            ret = x[rs1] < immediate;
            break;

        case SLTIU:
            ret = unsigned(x[rs1]) < immediate;
            break;

        case XORI:
            ret = x[rs1] ^ immediate;
            break;

        case ORI:
            ret = x[rs1] | immediate;
            break;

        case ANDI:
            ret = x[rs1] & immediate;
            break;

        case SLLI:
            ret = x[rs1] << shamt;
            break;

        case SRAI:
            ret = x[rs1] >> shamt;
            break;

        case SRLI:
            ret = unsigned(x[rs1]) >> shamt;
            break;

        case SUB:
            ret = x[rs1] - x[rs2];
            break;

        case ADD:
            ret = x[rs1] + x[rs2];
            break;

        case SLL:
            ret = x[rs1] << x[rs2];
            break;

        case SLT:
            ret = x[rs1] < x[rs2];
            break;

        case SLTU:
            ret = unsigned(x[rs1]) < unsigned(x[rs2]);
            break;

        case XOR:
            ret = x[rs1] ^ x[rs2];
            break;

        case SRA:
            ret = x[rs1] >> x[rs2];
            break;

        case SRL:
            ret = unsigned(x[rs1]) >> x[rs2];
            break;

        case OR:
            ret = x[rs1] | x[rs2];
            break;
        }
    }

    void MEM()
    {
        switch (inst)
        {
            char opt;
            unsigned int tmp;
        case LB:
            ret = memory[immediate];
            signed_extend(ret, 8);
            break;

        case LH:
            ret = (memory[immediate + 1] << 8) + memory[immediate];
            signed_extend(ret, 16);
            break;

        case LW:
            ret = (memory[immediate + 3] << 24) + (memory[immediate + 2] << 16) + (memory[immediate + 1] << 8) + memory[immediate];
            break;

        case LBU:
            ret = memory[immediate];
            break;

        case LHU:
            ret = (unsigned(memory[immediate + 1]) << 8) + memory[immediate];
            break;

        case SB:
            end();
            tmp = x[rs2];
            memory[immediate] = tmp & 255;
            break;

        case SH:
            end();
            tmp = x[rs2];
            memory[immediate] = tmp & 255;
            memory[immediate + 1] = (tmp >> 8) & 255;
            break;

        case SW:
            end();
            tmp = x[rs2];
            for (int i = 0; i < 4; i++)
            {
                opt = tmp >> (8 * i);
                opt = opt & 255;
                memory[immediate + i] = opt;
            }
            break;

        default:
            Three_cycle = false;
            break;
        }
    }

    void WB()
    {
        if (rd)
            x[rd] = ret;
        //out << "x[rd]=" << x[rd] << "  x[rs1]=" << x[rs1] << "  x[rs2]=" << x[rs2] << '\n';
        //out << "---------------------------------------------------\n";
    }
};

void IF(instruction *it)
{
    it->reg_pc = pc;
    for (int i = 0; i < 4; i++)
    {
        unsigned int tmp = memory[pc + i];
        unsigned int x = tmp;
        it->sum += (x << 8 * i);
        for (int j = 0; j < 8; j++)
        {
            it->bin[8 * i + j] = tmp % 2;
            tmp >>= 1;
        }
    }
    pc += 4;
}

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    freopen("pi.data", "r", stdin);
    //out.open("magic_debug_unfinished.txt");

    instruction *lsq = new instruction;
    int cnt = 0;

    read_in_memory();
    pc = 0;
    while (true)
    //for (int i = 0; i < 700; i++)
    {
        lsq = new instruction;
        //cycle[cnt] = new instruction;
        IF(lsq);
        lsq->ID();
        lsq->show_information();
        if (lsq->sum == unsigned(0x0ff00513))
        {
            cout << (((unsigned int)x[10]) & 255);
            break;
        }
        lsq->EXE();
        lsq->MEM();
        lsq->WB();
        delete lsq;
    }

    return 0;
}
