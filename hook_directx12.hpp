#pragma once
#include "Entity.hpp"
#include "YS.hpp"

namespace DX12 {
	void Hook(HWND hwnd);
	void Unhook();
	
}

extern uint64_t CustomDecodePointer(unsigned int encodedAddress, uintptr_t moduleBase);

Entity* GetPlayer1();

extern float g_currentFormTime;
extern float g_currentFormMaxTime;

inline Entity* ToEntity(YS::OBJ* obj) {
	return reinterpret_cast<Entity*>(obj);
}