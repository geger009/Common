
#include "stdafx.h"
#include "CubeCar.h"

using namespace graphic;


cCubeCar::cCubeCar()
{
}

cCubeCar::~cCubeCar()
{

}


// Ŭ���� �ʱ�ȭ.
bool cCubeCar::Init(cRenderer &renderer)
{
	cBoundingBox bbox;
	sMinMax mmax;

	mmax._min = Vector3(-1, -1, -1);
	mmax._max = Vector3(1, 1, 1);
	bbox.SetBoundingBox(mmax);
	m_mesh[0].Create(renderer, bbox);

	mmax._min = Vector3(1, -1, -1);
	mmax._max = Vector3(3, 1, 1);
	bbox.SetBoundingBox(mmax);
	m_mesh[1].Create(renderer, bbox);

	mmax._min = Vector3(-1, 1, -1);
	mmax._max = Vector3(1, 2, 1);
	bbox.SetBoundingBox(mmax);
	m_mesh[2].Create(renderer, bbox);

	const float weight = 0.15f;
	mmax._min = Vector3(-4, -weight, -weight);
	mmax._max = Vector3(6, weight, weight);
	bbox.SetBoundingBox(mmax);
	m_mesh[3].Create(renderer, bbox);

	mmax._min = Vector3(-weight, -6, -weight);
	mmax._max = Vector3(weight, 6, weight);
	bbox.SetBoundingBox(mmax);
	m_mesh[4].Create(renderer, bbox);

	mmax._min = Vector3(-weight, -weight, -6);
	mmax._max = Vector3(weight, weight, 6);
	bbox.SetBoundingBox(mmax);
	m_mesh[5].Create(renderer, bbox);

	m_mesh[0].SetColor(cColor::BLUE);
	m_mesh[1].SetColor(cColor::BLUE);
	m_mesh[2].SetColor(cColor::BLUE);
	m_mesh[3].SetColor(cColor::RED);
	m_mesh[4].SetColor(cColor::GREEN);
	m_mesh[5].SetColor(cColor::WHITE);
	return true;
}


// ȭ�鿡 ����Ѵ�.
void cCubeCar::Render(graphic::cRenderer &renderer)
{
	const XMMATRIX tm = m_tm.GetMatrixXM();
	m_mesh[0].Render(renderer, tm);
	m_mesh[1].Render(renderer, tm);
	m_mesh[2].Render(renderer, tm);
	m_mesh[3].Render(renderer, tm);
	m_mesh[4].Render(renderer, tm);
	m_mesh[5].Render(renderer, tm);
}


// �� �����Ӹ��� ������ ���� �߰��Ѵ�.
void cCubeCar::Update(const float deltaSeconds)
{
}


// ť�� ������ ���Ϸ� ������ �����Ѵ�.
// Radian ���� ���ڷ� �Ѵ�.
void cCubeCar::SetEulerAngle(const float roll, const float pitch, const float yaw, const float heave)
{
	m_rotate.Euler2(Vector3(roll, yaw, pitch));
	//m_tm = m_rotate.GetMatrix();

	Matrix44 t;
	t.SetPosition(Vector3(0, heave, 0));

	m_tm = m_rotate.GetMatrix() * t;
}

