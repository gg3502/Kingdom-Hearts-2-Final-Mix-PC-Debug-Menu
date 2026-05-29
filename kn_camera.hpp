#pragma once
#include <cstdint>
#include "fmatrix.hpp"
#include "common_types.hpp"

class kn_camera
{
public:
	uint64_t Vptr; //0x0000
	char pad_0008[8]; //0x0008
	Matrix4x4 Matrix; //0x0010
	Vector4 Pos; //0x0050
	Vector4 Ref; //0x0060
	Vector4 UP; //0x0070
	float Roll; //0x0080
	char pad_0084[92]; //0x0084
}; //Size: 0x00E0


struct Projection : Matrix4x4
{
	float scrz_;
	float fov_[2];
	float aspect_[3];
	float center_[3];
	float zmin_;
	float zmax_;
	float clipNear_;
	float clipFar_;
	float dispNear_;
	float dispFar_;
	float clipX_;
	float clipY_;
	Matrix4x4 viewClip_;
	Matrix4x4 viewClipGs_;
	Matrix4x4 viewProjection;
	Matrix4x4 projectionScreen;
};

