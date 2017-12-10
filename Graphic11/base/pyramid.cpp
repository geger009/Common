
#include "stdafx.h"
#include "pyramid.h"


using namespace graphic;


cPyramid::cPyramid()
	: cNode(common::GenerateId(), "pyramid", eNodeType::MODEL)
{
}

cPyramid::~cPyramid()
{
}


bool cPyramid::Create(cRenderer &renderer
	, const float width //=1
	, const float height//=1
	, const Vector3 &pos //=Vector3(0,0,0)
	, const int vtxType //= (eVertexType::POSITION)
	, const cColor &color //= cColor::BLACK
)
{
	m_shape.Create(renderer, width, height, pos, vtxType, color);
	m_transform.scale = Vector3(width, height, width);
	m_transform.pos = pos;
	m_boundingBox.SetBoundingBox(m_transform);
	m_color = color;
	m_vtxType = vtxType;
	return true;
}


bool cPyramid::Render(cRenderer &renderer
	, const XMMATRIX &parentTm //= XMIdentity
	, const int flags //= 1
)
{
	cShader11 *shader = (m_shader) ? m_shader : renderer.m_shaderMgr.FindShader(m_shape.m_vtxType);
	assert(shader);
	shader->SetTechnique("Unlit");
	shader->Begin();
	shader->BeginPass(renderer, 0);

	renderer.m_cbPerFrame.m_v->mWorld = XMMatrixTranspose(m_transform.GetMatrixXM() * parentTm);
	renderer.m_cbPerFrame.Update(renderer);
	renderer.m_cbLight.Update(renderer, 1);

	const Vector4 color = m_color.GetColor();
	renderer.m_cbMaterial.m_v->diffuse = XMVectorSet(color.x, color.y, color.z, color.w);
	renderer.m_cbMaterial.Update(renderer, 2);

	m_shape.Render(renderer);

	return __super::Render(renderer, parentTm, flags);
}


void cPyramid::SetDimension(const float width, const float height)
{
	m_transform.scale = Vector3(width, height, width);
}


void cPyramid::SetPos(const Vector3 &pos)
{
	m_transform.pos = pos;
}


// Rotation Head to (p1-p0) Vector
void cPyramid::SetDirection(const Vector3 &p0, const Vector3 &p1
	, const float width //= 1
)
{
	Vector3 v = p1 - p0;
	Quaternion q;
	q.SetRotationArc(Vector3(0, 1, 0), v.Normal());
	m_transform.rot = q;
	m_transform.pos = p0;
	m_transform.scale = Vector3(width, v.Length(), width);
	m_boundingBox.SetBoundingBox(m_transform);
}


void cPyramid::SetDirection(const Vector3 &p0, const Vector3 &p1, const Vector3 &from
	, const float width //= 1
	, const float lengthRatio //= 1.f
)
{
	Vector3 v = p1 - p0;
	Quaternion q;
	q.SetRotationArc(Vector3(0, 1, 0), v.Normal());
	m_transform.rot = q;
	m_transform.pos = from;
	m_transform.scale = Vector3(width, v.Length()*lengthRatio, width);
	m_boundingBox.SetBoundingBox(m_transform);
}
