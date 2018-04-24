
#include "stdafx.h"
#include "color.h"

using namespace graphic;


const cColor cColor::BLACK(0.f, 0.f, 0.f, 1.f);
const cColor cColor::WHITE(1.f, 1.f, 1.f, 1.f);
const cColor cColor::GRAY(.5f, .5f, .5f, 1.f);
const cColor cColor::BLUE(0.f, 0.f, 1.f, 1.f);
const cColor cColor::GREEN(0.f, 1.f, 0.f, 1.f);
const cColor cColor::RED(1.f, 0.f, 0.f, 1.f);
const cColor cColor::YELLOW(1.f, 1.f, 0.f, 1.f);



cColor::cColor()
	: m_color(0)
{
}

cColor::cColor(const float r, const float g, const float b
	, const float a //= 1.f
)
{
	SetColor(r, g, b, a);
}

cColor::cColor(const BYTE r, const BYTE g, const BYTE b
	, const BYTE a //= 255
)
{
	SetColor(r, g, b, a);
}

cColor::cColor(const Vector3 &color)
{
	SetColor(color);
}
cColor::cColor(const Vector4 &color)
{
	SetColor(color);
}

cColor::cColor(const DWORD color)
	: m_color(color)
{	
}

cColor::~cColor()
{
}


// r, g, b : 0 ~ 1 
void cColor::SetColor(const float r, const float g, const float b
	, const float a //= 1.f
)
{
	m_color = COLOR_COLORVALUE(r, g, b, a);
}


// r, g, b : 0 ~ 255
void cColor::SetColor(const BYTE r, const BYTE g, const BYTE b
	, const BYTE a //= 255
)
{
	m_color = COLOR_ARGB(a, r, g, b);
}


// x-r, y-g, z-b : 0 ~ 1 
void cColor::SetColor(const Vector3 &color)
{
	SetColor(color.x, color.y, color.z);
}

// x-r, y-g, z-b, w-a : 0 ~ 1 
void cColor::SetColor(const Vector4 &color)
{
	SetColor(color.x, color.y, color.z, color.w);
}


// red-green-blue-alpha : 0 ~ 1
Vector4 cColor::GetColor() const
{
	Vector4 v;
	v.w = (m_color >> 24) / 255.f; // A
	v.x = ((m_color >> 16) & 0xFF) / 255.f; // R
	v.y = ((m_color >> 8) & 0xFF) / 255.f; // G
	v.z = (m_color & 0xFF) / 255.f; // B
	return v;
}


void cColor::GetColor(OUT float out[4]) const
{
	const Vector4 v = GetColor();
	out[0] = v.x;
	out[1] = v.y;
	out[2] = v.z;
	out[3] = v.w;
}


DWORD cColor::GetAbgr() const
{
	DWORD r = (m_color >> 24) << 24 
		| (m_color & 0xFF) << 16
		| ((m_color >> 8) & 0xFF) << 8 
		| ((m_color >> 16) & 0xFF);
	return r;
}


cColor::operator DWORD ()
{
	return m_color;
}


bool cColor::operator == (const cColor &color) {
	return m_color == color.m_color;
}


bool cColor::operator != (const cColor &color) {
	return !(operator==(color));
}
