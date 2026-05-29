#pragma once
#include <winsock.h>

struct COLOR
{
	u_char R;
	u_char G;
	u_char B;
	u_char A;
};



struct Fog
{
	COLOR m_color;
	float m_near;
	float m_far;
	float m_min;
	float m_max;
};
