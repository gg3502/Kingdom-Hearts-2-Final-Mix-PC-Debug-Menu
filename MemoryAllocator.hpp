#pragma once

struct MemoryBlockHeader;

struct Allocator {
    const int* vptr;
};

typedef unsigned int UInt;

struct MemoryBlockHeader {
    MemoryBlockHeader* next;
    MemoryBlockHeader* prev;
    UInt size;
    unsigned int* returnAddress;
    Allocator* allocator;
    UInt padding[3];
};

struct MemoryAllocator : Allocator {
    const UInt heapTop;
    const UInt heapEnd;
    UInt usedSize;
    MemoryBlockHeader* last;
    MemoryBlockHeader head;
};

typedef __int8 ALLOCATOR[4];