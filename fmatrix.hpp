#pragma once
#include "common_types.hpp"

typedef float FLOAT;

struct Axa_FVECTOR4
{
	FLOAT x;
	FLOAT y;
	FLOAT z;
	FLOAT w;
};

/* 163 */
struct Matrix4x4
{
	Vector4 v0;
	Vector4 v1;
	Vector4 v2;
	Vector4 v3;



};

struct NewMatrix4x4 {
	float m[16]; // row-major 4x4

	// Access like v0, v1, v2, v3 if needed
	float* operator[](int row) { return &m[row * 4]; }
	const float* operator[](int row) const { return &m[row * 4]; }
};


/* 163 */
struct Axa_FMATRIX44
{
	Axa_FVECTOR4 v0;
	Axa_FVECTOR4 v1;
	Axa_FVECTOR4 v2;
	Axa_FVECTOR4 v3;
};

/* 1300 */
struct kn_FMatrix : Axa_FMATRIX44
{
};

struct BITFLAG {

	__int8 Flag[4];
};