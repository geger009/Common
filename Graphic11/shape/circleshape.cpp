
#include "stdafx.h"
#include "circleshape.h"

using namespace graphic;


cCircleShape::cCircleShape()
{
}

cCircleShape::~cCircleShape()
{
}


bool cCircleShape::Create(cRenderer &renderer, const Vector3 &center, const float radius, const int slice
	, const cColor &color //=cColor::BLACK
)
{
	const int vtxType = eVertexType::POSITION | eVertexType::COLOR;
	cVertexLayout vtxLayout;
	vtxLayout.Create(vtxType);
	const int posOffset = vtxLayout.GetOffset("POSITION");
	const int normOffset = vtxLayout.GetOffset("NORMAL");
	const int colorOffset = vtxLayout.GetOffset("COLOR");
	const int texOffset = vtxLayout.GetOffset("TEXCOORD");
	const int vertexStride = vtxLayout.GetVertexSize();

	vector<BYTE> buffer(vertexStride * (slice+1));
	BYTE *initVertices = &buffer[0];
	const Vector4 vColor = color.GetColor();
	BYTE *pvtx = initVertices;

	if (vtxType & eVertexType::POSITION)
		*(Vector3*)(pvtx + posOffset) = center;
	//if (vtxType & eVertexType::NORMAL)
	//	*(Vector3*)(pvtx + normOffset) = normal;
	if (vtxType & eVertexType::COLOR)
		*(Vector4*)(pvtx + colorOffset) = vColor;
	pvtx += vertexStride;

	const float inc = MATH_PI * 2.f / (float)(slice);
	for (int i=0; i < slice; ++i)
	{
		if (vtxType & eVertexType::POSITION)
			*(Vector3*)(pvtx + posOffset) = center + Vector3(cosf(inc*i) * radius, sinf(inc*i) * radius, 0);
		//if (vtxType & eVertexType::NORMAL)
		//	*(Vector3*)(pvtx + normOffset) = normal;
		if (vtxType & eVertexType::COLOR)
			*(Vector4*)(pvtx + colorOffset) = vColor;
		pvtx += vertexStride;
	}

	m_vtxBuff.Create(renderer, slice+1, vertexStride, initVertices);


	vector<WORD> buffer2(slice*3);
	WORD *initIndices = &buffer2[0];
	for (int i = 0; i < slice; ++i)
	{
		initIndices[i * 3] = 0;
		initIndices[i * 3 + 1] = i+1;
		initIndices[i * 3 + 2] = (i==(slice-1))? 1 : i+2;
	}

	m_idxBuff.Create(renderer, slice, (BYTE*)initIndices);

	m_vtxType = vtxType;

	return true;
}


void cCircleShape::Render(cRenderer &renderer)
{
	m_vtxBuff.Bind(renderer);
	m_idxBuff.Bind(renderer);
	renderer.GetDevContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	renderer.GetDevContext()->DrawIndexed(m_idxBuff.GetFaceCount() * 3, 0, 0);
}
