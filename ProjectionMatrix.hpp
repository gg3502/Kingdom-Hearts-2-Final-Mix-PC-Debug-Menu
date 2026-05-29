#pragma once
#include "fmatrix.hpp"
#include <DirectXMath.h>

struct ProjectionMatrix : DirectX::XMMATRIX
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
	DirectX::XMMATRIX viewClip_;
	DirectX::XMMATRIX viewClipGs_;
	DirectX::XMMATRIX viewProjection;
	DirectX::XMMATRIX projectionScreen;
};
