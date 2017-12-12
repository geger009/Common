
#include "stdafx.h"
#include "frustum.h"

using namespace graphic;

cFrustum::cFrustum()
	//:	m_fullCheck(false)
	: m_plane(6) // ����ü ��� 6��
	, m_epsilon(0.5f)
{
}

cFrustum::~cFrustum()
{
}


bool cFrustum::SetFrustum(const Matrix44 &matViewProj)
{
	//      4 --- 5
	//    / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /   | /
	//   2 --- 3
	//
	// ������ı��� ��ġ�� ��� 3���� ������ǥ�� ���� (-1,-1,0) ~ (1,1,1)������ ������ �ٲ��.
	Vector3 vertices[8] = {
		Vector3(-1,1,0), Vector3(1,1,0), Vector3(-1,-1,0), Vector3(1,-1,0),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};

	// view * proj�� ������� ���Ѵ�.
	Matrix44 matInv = matViewProj.Inverse();

	for (int i = 0; i < 8; i++)
		vertices[i] *= matInv;

	m_pos = (vertices[0] + vertices[3]) / 2.0f;

	m_plane[0].Init(vertices[0], vertices[1], vertices[2]);	// �� ���(near)
	m_plane[1].Init(vertices[0], vertices[4], vertices[1]);	// �� ���(up)
	m_plane[2].Init(vertices[2], vertices[3], vertices[7]);	// �Ʒ� ���(down)
	m_plane[3].Init(vertices[4], vertices[6], vertices[5]);	// �� ���(far)
	m_plane[4].Init(vertices[0], vertices[2], vertices[6]);	// �� ���(left)
	m_plane[5].Init(vertices[1], vertices[5], vertices[7]);	// �� ���(right)

	m_viewProj = matViewProj;

	return true;
}


//-----------------------------------------------------------------------------//
// ���� point�� �������Ҿȿ� ������ TRUE�� ��ȯ, �ƴϸ� FALSE�� ��ȯ�Ѵ�.
//-----------------------------------------------------------------------------//
bool cFrustum::IsIn( const Vector3 &point ) const
{
	for (int i = 0; i < 6; ++i)
	{
		// m_fullCheck �� false ��� near, top, bottom  ��� üũ�� ���� �ȴ�.
		//if (!m_fullCheck && (i < 3))
		//	continue;

		// ���� �߽����� �Ÿ��� ���������� ũ�� �������ҿ� ����
		const float dist = m_plane[i].Distance(point);
		if (dist > m_epsilon)
			return false;
	}
	return true;
}



bool cFrustum::IsInSphere(const cBoundingSphere &sphere) const
{
	const Vector3 pos = sphere.GetPos();
	for (int i = 0; i < 6; ++i)
	{
		// m_fullCheck �� false ��� near, top, bottom  ��� üũ�� ���� �ȴ�.
		//if (!m_fullCheck && (i < 3))
		//	continue;

		// ���� �߽����� �Ÿ��� ���������� ũ�� �������ҿ� ����
		const float dist = m_plane[i].Distance(pos);
		if (dist > (sphere.GetRadius() + m_epsilon))
			return false;
	}
	return true;
}


bool cFrustum::IsInBox(const cBoundingBox &bbox) const
{
	//assert(0);

	return DISJOINT != m_frustum.Contains(bbox.m_bbox);

	//const Vector3 vertices[] = {
	//	bbox.m_min
	//	, Vector3(bbox.m_max.x, bbox.m_min.y, bbox.m_min.z)
	//	, Vector3(bbox.m_min.x, bbox.m_max.y, bbox.m_min.z)
	//	, Vector3(bbox.m_min.x, bbox.m_max.y, bbox.m_max.z)
	//	, bbox.m_max
	//	, Vector3(bbox.m_min.x, bbox.m_max.y, bbox.m_max.z)
	//	, Vector3(bbox.m_max.x, bbox.m_min.y, bbox.m_max.z)
	//	, Vector3(bbox.m_max.x, bbox.m_max.y, bbox.m_min.z)
	//};

	//for (int i = 0; i < 8; ++i)
	//{
	//	if (IsIn(vertices[i]))
	//		return true;
	//}

	//Vector3 size = bbox.m_max - bbox.m_min;
	//const float radius = size.Length() * 0.5f;
	//return IsInSphere(bbox.Center() * tm, radius);
	return false;
}


// Split Frustum
// f : 0 ~ 1
void cFrustum::Split2(const cCamera &cam, const float f1, const float f2
	, cFrustum &out1, cFrustum &out2)
{
}


// Split Frustum
// f : 0 ~ 1 (near-far plane ratio)
void cFrustum::Split3(const cCamera &cam, const float f1, const float f2, const float f3
	, cFrustum &out1, cFrustum &out2, cFrustum &out3)
{
	const float oldNearPlane = cam.m_near;
	const float oldFarPlane = cam.m_far;
	const float far1 = common::lerp(cam.m_near, cam.m_far, f1);
	const float far2 = common::lerp(cam.m_near, cam.m_far, f2);
	const float far3 = common::lerp(cam.m_near, cam.m_far, f3);

	cCamera tmpCam = cam;

	tmpCam.ReCalcProjection(tmpCam.m_near, far1);
	out1.SetFrustum(tmpCam.GetViewProjectionMatrix());

	tmpCam.ReCalcProjection(far1, far2);
	out2.SetFrustum(tmpCam.GetViewProjectionMatrix());

	tmpCam.ReCalcProjection(far2, far3);
	out3.SetFrustum(tmpCam.GetViewProjectionMatrix());
}


// Split Frustum
// f1,2,3 : meter
void cFrustum::Split3_2(const cCamera &cam, const float f1, const float f2, const float f3
	, cFrustum &out1, cFrustum &out2, cFrustum &out3)
{
	const float oldNearPlane = cam.m_near;
	const float oldFarPlane = cam.m_far;

	cCamera tmpCam = cam;

	tmpCam.ReCalcProjection(tmpCam.m_near, f1);
	out1.SetFrustum(tmpCam.GetViewProjectionMatrix());

	tmpCam.ReCalcProjection(f1, f2);
	out2.SetFrustum(tmpCam.GetViewProjectionMatrix());

	tmpCam.ReCalcProjection(f2, f3);
	out3.SetFrustum(tmpCam.GetViewProjectionMatrix());
}


// return Frustum & Plane Collision Vertex
// Plane is Usually Ground Plane
void cFrustum::GetGroundPlaneVertices(const Plane &plane, OUT Vector3 outVertices[4]) const
{
	//        4 --- 5
	//      / |  |  /|
	//   0 --- 1   |
	//   |   6-|- -7
	//   | /     | /
	//   2 --- 3
	//
	Vector3 vertices[8] = {
		Vector3(-1,1,0), Vector3(1,1,0), Vector3(-1,-1,0), Vector3(1,-1,0),
		Vector3(-1,1, 1), Vector3(1,1, 1), Vector3(-1,-1,1), Vector3(1,-1,1),
	};
	Matrix44 matInv = m_viewProj.Inverse();
	for (int i = 0; i < 8; ++i)
		vertices[i] *= matInv;

	// Far Plane
	Vector3 p0, p1;
	if (plane.LineCross(vertices[0], vertices[4], &p0) == 0)
		p0 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	if (plane.LineCross(vertices[1], vertices[5], &p1) == 0)
		p1 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 p4 = plane.Pick(vertices[4], (vertices[6] - vertices[4]).Normal());
	Vector3 p5 = plane.Pick(vertices[5], (vertices[7] - vertices[5]).Normal());

	// Near Plane
	Vector3 p2, p3;
	if (plane.LineCross(vertices[3], vertices[7], &p2) == 0)
		p2 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	if (plane.LineCross(vertices[2], vertices[6], &p3) == 0)
		p3 = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 p6 = plane.Pick(vertices[1], (vertices[3] - vertices[1]).Normal());
	Vector3 p7 = plane.Pick(vertices[0], (vertices[2] - vertices[0]).Normal());

	outVertices[0] = (LengthRoughly(p0) < LengthRoughly(p4)) ? p0 : p4;
	outVertices[1] = (LengthRoughly(p1) < LengthRoughly(p5)) ? p1 : p5;
	outVertices[2] = (LengthRoughly(p2) < LengthRoughly(p6)) ? p2 : p6;
	outVertices[3] = (LengthRoughly(p3) < LengthRoughly(p7)) ? p3 : p7;
}


float cFrustum::LengthRoughly(const Vector3 &pos) const
{
	return (*(Vector3*)&m_frustum.Origin).LengthRoughly(pos);
}


cFrustum& cFrustum::operator=(const cFrustum &rhs)
{
	if (this != &rhs)
	{
		m_viewProj = rhs.m_viewProj;
		m_frustum = rhs.m_frustum;
	}
	return *this;
}
