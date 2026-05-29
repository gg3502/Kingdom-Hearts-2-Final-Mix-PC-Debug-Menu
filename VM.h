#pragma once
#include "YS.hpp"
#include <vector>


struct BD_HEADER
{
    char filename[16];   // BD_CHAR[16]
    int work_size;       // BD_INT
    int stack_size;      // BD_INT
    int term_size;       // BD_INT
};

enum BD_STATUS : __int32 { BD_STATUS_EXEC = 0x0, BD_STATUS_HALT = 0x1, BD_STATUS_EXIT = 0x2, BD_STATUS_RET = 0x3, BD_STATUS_IGNORE = 0x4, BD_STATUS_ERROR = 0x5, };

struct OpcodeEntry {
    int main;
    int sub;
    int ssub;
    const char* name;
    const char* description;
    int argWords; // number of 16-bit words following the opcode
};

struct BUFFER16
{
    alignas(16) char data[16]; // or whatever size a single "block" is
};

struct VM_WORK
{
    int RefCount;        // reference count
    alignas(16) BUFFER16 Buff[]; // flexible array at the end
};


struct OpcodeHistoryEntry {
    uint32_t pc;
    uint16_t rawOpcode;
};

union BD_VALUE
{
    __int8 gap0[4];
};



struct BD_OPCODE {
    unsigned __int16 param : 10;
    unsigned __int16 mode : 2;
    unsigned __int16 inst : 4;
};

struct BD_PROCESS {
    unsigned short* top;   // start of instruction buffer
    uint32_t pc;    // current instruction pointer
    unsigned short* sp;    // stack pointer
    unsigned short* tp;    // temp pointer
    unsigned short* wp;    // write pointer
};

struct VM {
    BD_PROCESS Process;
    int* StackBuff;
    int* TermBuff;
    int Flag;
    int Group;
    int Finalizer;
    void* SList;  // simplified
    void* ChildList;
    VM_WORK* Work;
    YS::OBJ* Owner;
    int DebugCounter;
};

struct VMTrace {
    VM* vm;
    YS::OBJ* owner;
};

std::vector<VMTrace> ActiveVMs;