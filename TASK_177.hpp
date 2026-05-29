#pragma once

typedef __int8 TASK_177[84];

struct TASK;


typedef void (*TASK_FUNC)(TASK*);


#include <cstdint>  // Include the necessary header for uint8_t if not already included

// Forward declaration for TASK_MANAGER_0
struct TASK_MANAGER_0;

// Define INT and UINT if they are not already defined
typedef int INT;
typedef unsigned int UINT;

struct Axa_CSJointT
{
    const int* _vptr_;
    Axa_CSJointT* m_pNext;
};

struct TaskALLOCATOR
{
    const int* _vptr_;
};

struct CellFiberPpuContext_0
{
    unsigned __int8 skip[640];
};

union TASK_ARGS
{
    int i;
    bool b;
    float f;
    unsigned int* p;
};

struct Axa_FiberPS3
{
    CellFiberPpuContext_0 context;
    void (*func)(unsigned int*);
    unsigned int* arg;
    size_t stackSize;
    INT disposeFlg;
    INT dum[3];
    UINT _pad;
    uint8_t stack[73728];
    UINT _check[4];
};

typedef Axa_FiberPS3* FiberHandle;

struct YS_LIST_0
{
    int* Head; // unknown type
    int* Tail; // unknown type
};

struct kingdom_CONTEXT
{
    FiberHandle fiberHandle;
    UINT dummy[3];
};

struct TASK_THREAD
{
    __int8 gap0[32];
};

struct YS_LIST_ELEM
{
    int* Next[1]; // unknown type
    int* Prev[1]; // unknown type
};

struct TASK_MANAGER_0
{
    TaskALLOCATOR* Allocator;
    YS_LIST_0 TaskList;
    //TASK_0* Current;
    kingdom_CONTEXT Context;
    //TASK_0* Next;
};

struct TASK_0
{
    TASK_FUNC EntryFunc;
    unsigned int* m_nThreadID;
    int m_nThreadSemaID;
    bool m_nSuspend;
    TASK_ARGS Args[8];
    TASK_MANAGER_0* Manager;
    int Id;
    int Priority;
    TASK_FUNC Finalizer;
    TASK_THREAD* Thread;
    YS_LIST_ELEM List;
    TASK_0* Prev;
    TASK_0* Next;
};

typedef __int8 TASK_MANAGER_11[40];

typedef __int8 TASK_5[84];
