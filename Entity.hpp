#pragma once

#include "common_types.hpp"
#include "fmatrix.hpp"

// Created with ReClass.NET 1.2 by KN4CK3R

class Entity
{
public:
	uint32_t pVTable; //0x0000
	uint32_t MySelf; //0x0004
	uint32_t Entry; //0x0008
	uint32_t Action; //0x000C
	Vector4 Movement; //0x0010
	Vector4 Inertia; //0x0020
	Vector4 Scale; //0x0030
	Matrix4x4 Matrix; //0x0040
	uint64_t Pax; //0x0080
	char pad_0088[160]; //0x0088
	float WalkSpeed; //0x0128
	float RunSpeed; //0x012C
	char pad_0130[344]; //0x0130
	Vector3 MinBBOX; //0x0288
	Vector3 MaxBBOX; //0x0294
	char pad_02A0[976]; //0x02A0
	Vector4 ent_pos; //0x0670
	float Direction; //0x0680
	char pad_0684[532]; //0x0684
	Vector3 RBBOXMIN; //0x0898
	Vector3 RBBOXMAX; //0x08A4
	char pad_08B0[112]; //0x08B0
	uint64_t mdlxPointer; //0x0920
	char pad_0928[400]; //0x0928
	uint64_t ClsnData; //0x0AB8
	char pad_0AC0[504]; //0x0AC0
}; //Size: 0x0CB8

