#pragma once
#include "common_types.hpp"
#include "YS.hpp"
#include "fmatrix.hpp"


struct obj_camera
{
	Vector4 m_at;
	Vector4 m_eye;
	Vector4 m_fovV;
	float m_fov;
	float m_roll;
	bool m_fixed;
	bool m_homeQuick;
	bool m_kind;
	bool m_lockon;
	int m_type;
	int m_mode;
	YS::OBJ* m_obj;
	float m_radius;
	float m_yRot;
	float m_backYRot;
	Vector4 m_eyeTarget;
	Vector4 m_eyeTargetPrev;
	Vector4 m_atTarget;
	Vector4 m_atTargetPrev;
	Vector4 m_fovVTarget;
	Vector4 m_fovVTargetPrev;
	float m_Yaw;
	float m_Pitch;
	float m_objectiveInitRadius;
	float m_objectiveLockRadius;
	float m_objectiveRadiusMin;
	float m_objectiveRadiusMax;
	float m_objectiveUpCurve;
	float m_defaultFov;
	float m_defaultRoll;
};




struct screen_projection : Matrix4x4
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