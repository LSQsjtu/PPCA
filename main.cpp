#include "predict.hpp"

int x[32] = {0}, pc = 0, correct_time = 0, wrong_time = 0;
bool reg_lock[32] = {0};
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

enum state
{
    avail,
    busy
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

int signed_extend(int &data, int bits)
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

int S_offset(int in)
{
    int imm = 0;
    imm = (in >> 7) & 31;
    imm += ((in >> 25) & 127) << 5;
    signed_extend(imm, 11);
    return imm;
}

int J_offset(int in)
{
    int imm = 0;
    imm = (in >> 20) & 2046;
    imm += ((in >> 20) & 1) << 11;
    imm += ((in >> 12) & 255) << 12;
    imm += ((in >> 31) & 1) << 20;
    signed_extend(imm, 20);
    return imm;
}

struct instruction
{
    int ret, rd, rs1, rs2, immediate, inst, reg_pc, shamt, state;
    unsigned int sum;
    bool Three_cycle, jump_flag;

    instruction()
    {
        ret = 0, rd = 0, rs1 = 0, rs2 = 0, immediate = 0, inst = 0, reg_pc = 0, shamt = 0;
        sum = 0;
        Three_cycle = false;
        jump_flag = false;
        state = avail;
    }

    instruction(instruction *temp)
    {
        ret = temp->ret;
        rd = temp->rd;
        rs1 = temp->rs1;
        rs2 = temp->rs2;
        immediate = temp->immediate;
        inst = temp->inst;
        reg_pc = temp->reg_pc;
        shamt = temp->shamt;
        Three_cycle = temp->Three_cycle;
    }

    ~instruction()
    {
        ret = 0, rd = 0, rs1 = 0, rs2 = 0, immediate = 0, inst = 0, reg_pc = 0, shamt = 0;
        sum = 0;
        Three_cycle = true;
    }

    instruction operator=(const instruction *temp)
    {
        ret = temp->ret;
        rd = temp->rd;
        rs1 = temp->rs1;
        rs2 = temp->rs2;
        immediate = temp->immediate;
        inst = temp->inst;
        reg_pc = temp->reg_pc;
        shamt = temp->shamt;
        Three_cycle = temp->Three_cycle;
        jump_flag = temp->jump_flag;
        return *this;
    }

    void show_information()
    {
        cout << "after ID: pc:" << hex << reg_pc << dec << "  inst:" << inst_name[inst] << "\nrd:" << rd << "  rs1:" << rs1 << "  rs2:" << rs2 << "  imm:" << hex << immediate << "  inst:" << sum << endl;
    };

    void end()
    {
        if (sum == unsigned(0x0ff00513))
        {
            cout << dec << (((unsigned int)x[10]) & 255);
            //cout << '\n'
              //   << wrong_time << "   " << correct_time;
            exit(0);
        }
    }

    void ID()
    {
        int opt = memory[reg_pc] & 127;
        switch (opt)
        {
        case 55: //LUI
            inst = LUI;
            rd = (sum >> 7) & 0x1f;
            immediate = sum >> 12 << 12;
            break;

        case 23: //AUIPC
            inst = AUIPC;
            rd = (sum >> 7) & 0x1f;
            immediate = sum >> 12 << 12;
            jump_flag = true;
            break;

        case 111: //JAL
            inst = JAL;
            rd = (sum >> 7) & 0x1f;
            immediate = J_offset(sum);
            jump_flag = true;
            break;

        case 103: //JALR
            inst = JALR;
            rd = (sum >> 7) & 0x1f;
            rs1 = (sum >> 15) & 0x1f;
            immediate = I_offset(sum);
            jump_flag = true;
            break;

        case 99: //BEQ……
            rs1 = (sum >> 15) & 0x1f;
            rs2 = (sum >> 20) & 0x1f;
            jump_flag = pred.get_predict(reg_pc);
            immediate = B_offset(sum);
            {
                int function = (sum >> 12) & 7;
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
            rd = (sum >> 7) & 0x1f;
            rs1 = (sum >> 15) & 0x1f;
            Three_cycle = true;
            immediate = I_offset(sum);
            {
                int function = (sum >> 12) & 7;
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
            rs1 = (sum >> 15) & 0x1f;
            rs2 = (sum >> 20) & 0x1f;
            Three_cycle = true;
            immediate = S_offset(sum);
            {
                int function = (sum >> 12) & 7;
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
            rd = (sum >> 7) & 0x1f;
            rs1 = (sum >> 15) & 0x1f;
            immediate = I_offset(sum);
            {
                int function = (sum >> 12) & 7;
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
                    shamt = (sum >> 20) & 63;
                    break;

                case 5:
                    shamt = (sum >> 20) & 63;
                    if ((sum >> 30) & 1)
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
            rd = (sum >> 7) & 0x1f;
            rs1 = (sum >> 15) & 0x1f;
            rs2 = (sum >> 20) & 0x1f;
            {
                int function = (sum >> 12) & 7;
                switch (function)
                {
                case 0:
                    if ((sum >> 30) & 1)
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
                    if ((sum >> 30) & 1)
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

        end();
    }

    void EXE()
    {
        ret = x[rd];

        if (rd != 0)
        {
            reg_lock[rd] = true;
        }
        switch (inst)
        {
        case LUI:
            ret = immediate;
            break;

        case AUIPC:
            pc = reg_pc + immediate;
            ret = pc;
            break;

        case JAL:
            ret = reg_pc + 4;
            pc = reg_pc + immediate;
            break;

        case JALR:
            ret = reg_pc + 4;
            pc = x[rs1] + immediate;
            pc = pc & (~1);
            break;

        case BEQ:
            if (x[rs1] == x[rs2])
            {
                if (jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = true;
                }
                pc = reg_pc + immediate;
            }
            else
            {
                if (!jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = false;
                }
            }
            history[(reg_pc >> 2) & 0xff].update(jump_flag);
            break;

        case BNE:
            if (x[rs1] != x[rs2])
            {
                if (jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = true;
                }
                pc = reg_pc + immediate;
            }
            else
            {
                if (!jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = false;
                }
            }
            history[(reg_pc >> 2) & 0xff].update(jump_flag);
            break;

        case BGE:
            if (x[rs1] >= x[rs2])
            {
                if (jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = true;
                }
                pc = reg_pc + immediate;
            }
            else
            {
                if (!jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = false;
                }
            }
            history[(reg_pc >> 2) & 0xff].update(jump_flag);
            break;

        case BLT:
            if (x[rs1] < x[rs2])
            {
                if (jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = true;
                }
                pc = reg_pc + immediate;
            }
            else
            {
                if (!jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = false;
                }
            }
            history[(reg_pc >> 2) & 0xff].update(jump_flag);
            break;

        case BLTU:
            if (unsigned(x[rs1]) < unsigned(x[rs2]))
            {
                if (jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = true;
                }
                pc = reg_pc + immediate;
            }
            else
            {
                if (!jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = false;
                }
            }
            history[(reg_pc >> 2) & 0xff].update(jump_flag);
            break;

        case BGEU:
            if (unsigned(x[rs1]) >= unsigned(x[rs2]))
            {
                if (jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = true;
                }
                pc = reg_pc + immediate;
            }
            else
            {
                if (!jump_flag)
                {
                    correct_time++;
                }
                else
                {
                    wrong_time++;
                    jump_flag = false;
                }
            }
            history[(reg_pc >> 2) & 0xff].update(jump_flag);
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
            tmp = x[rs2];
            memory[immediate] = tmp & 255;
            break;

        case SH:
            tmp = x[rs2];
            memory[immediate] = tmp & 255;
            memory[immediate + 1] = (tmp >> 8) & 255;
            break;

        case SW:
            tmp = x[rs2];
            for (int i = 0; i < 4; i++)
            {
                opt = tmp >> (8 * i);
                opt = opt & 255;
                memory[immediate + i] = opt;
            }
            break;

        default:
            break;
        }
    }

    void WB()
    {
        if (rd)
            x[rd] = ret, reg_lock[rd] = false;
        //cout << "x[rd]=" << x[rd] << "  x[rs1]=" << x[rs1] << "  x[rs2]=" << x[rs2] << '\n';
        //cout << "---------------------------------------------------\n";
    }
};

void IF(instruction *it)
{
    it->reg_pc = pc;
    for (int i = 0; i < 4; i++)
    {
        unsigned int x = memory[pc + i];
        it->sum += (x << 8 * i);
    }
    pc += 4;
}

struct IF_ID
{
    instruction inst;
    int state;

    IF_ID(instruction *temp) : inst(temp)
    {
        state = avail;
    }
    IF_ID()
    {
        state = avail;
    }
};

struct EXE_MEM
{
    instruction inst;
    int state;
    EXE_MEM()
    {
        state = avail;
    }
};

struct MEM_WB
{
    int state;
    instruction inst;
    MEM_WB()
    {
        state = avail;
    }
};

int main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    //freopen("pi.data", "r", stdin);
    //freopen("magic_debug_unfinished.txt", "w", stdout);

    instruction *lsq = new instruction;
    IF_ID fetch;
    EXE_MEM mem;
    MEM_WB wb;

    bool next_IF = true;

    read_in_memory();
    pc = 0;
    while (true)
    {
        if (wb.state == busy)
        {
            wb.inst.WB();
            wb.state = avail;
        }

        if (mem.state == busy && wb.state == avail)
        {
            if (!mem.inst.Three_cycle)
            {
                wb.inst = &mem.inst;
                mem.state = avail;
                wb.state = busy;
            }
            else
            {
                static int time = 0;

                if (time == 2)
                {
                    time = 0;
                    mem.inst.MEM();
                    wb.inst = &mem.inst;
                    mem.state = avail;
                    wb.state = busy;
                }
                else
                {
                    time++;
                }
            }
        }

        if (fetch.state == busy && mem.state == avail)
        {
            if (reg_lock[fetch.inst.rs1] || reg_lock[fetch.inst.rs2])
            {
            }
            else
            {
                fetch.inst.EXE();
                if (fetch.inst.jump_flag)
                {
                    lsq->state = avail;
                    delete lsq;
                    lsq = new instruction;
                }
                next_IF = true;
                mem.inst = &fetch.inst;
                fetch.state = avail;
                mem.state = busy;
            }
        }

        if (fetch.state == avail && lsq->state == busy)
        {
            lsq->ID();
            //lsq->show_information();
            fetch.inst = lsq;
            fetch.state = busy;
            if (fetch.inst.jump_flag)
            {
                next_IF = false;
            }
            delete lsq;
            lsq = new instruction;
        }

        if (next_IF && lsq->state == avail)
        {
            IF(lsq);
            lsq->state = busy;
        }
    }

    return 0;
}
