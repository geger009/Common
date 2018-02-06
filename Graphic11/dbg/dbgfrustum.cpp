
#include "stdafx.h"
#include "dbgfrustum.h"

using namespace graphic;


cDbgFrustum::cDbgFrustum()
{
}

cDbgFrustum::~cDbgFrustum()
{
}


bool cDbgFrustum::Create(cRenderer &renderer, const Matrix44 &viewProj
	, const cColor &color //= cColor::BLACK
)
{
	cFrustum::SetFrustum(viewProj);
	m_box.Create(renderer);
	SetFrustum(renderer, viewProj, color);

	return true;
}


bool cDbgFrustum::SetFrustum(cRenderer &renderer, const Matrix44 &viewProj
	, const cColor &color //= cColor::BLACK
)
{
	cFrustum::SetFrustum(viewProj);

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
	Matrix44 matInv = viewProj.Inverse();

	for (int i = 0; i < 8; i++)
		vertices[i] *= matInv;

	m_box.SetBox(renderer, vertices, color);

	return true;
}


bool cDbgFrustum::SetFrustum(cRenderer &renderer, const cFrustum &frustum)
{
	__super::operator=(frustum);
	SetFrustum(renderer, m_viewProj);
	return true;
}


void cDbgFrustum::Render(cRenderer &renderer)
{
	m_box.Render(renderer);

	//renderer.GetDevice()->SetTexture( 0, NULL );
	//renderer.GetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	//renderer.GetDevice()->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	//renderer.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	//renderer.GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	//renderer.GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	//renderer.GetDevice()->SetTransform(D3DTS_WORLD, (D3DXMATRIX*)&m_box.m_tm);

	//// �Ķ������� ��,�� ����� �׸���
	//D3DMATERIAL9 mtrl;
	//DWORD lightMode;
	//renderer.GetDevice()->GetRenderState(D3DRS_LIGHTING, &lightMode);
	//renderer.GetDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	//ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	//mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	//renderer.GetDevice()->SetMaterial( &mtrl );
	//m_box.m_vtxBuff.Bind(renderer);
	//m_box.m_idxBuff.Bind(renderer);
	//renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_box.m_vtxBuff.GetVertexCount(), 12, 4);

	//// ������� ��,�� ����� �׸���.
	//ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	//mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	//renderer.GetDevice()->SetMaterial( &mtrl );
	//renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_box.m_vtxBuff.GetVertexCount(), 24, 4);

	//renderer.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	//renderer.GetDevice()->SetRenderState(D3DRS_LIGHTING, lightMode);
}

