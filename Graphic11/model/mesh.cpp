
#include "stdafx.h"
#include "mesh.h"

using namespace graphic;


cMesh::cMesh(cRenderer &renderer, const int id, const sRawMesh &rawMesh) :
	cNode(id, rawMesh.name)
,	m_buffers(NULL)
,	m_isBoneMesh(false)
{
	m_localTM = rawMesh.localTm;
	CreateMaterials(renderer, rawMesh);

	m_buffers = cResourceManager::Get()->LoadMeshBuffer(renderer, rawMesh.name);
}

cMesh::cMesh(cRenderer &renderer, const int id, const sRawBone &rawBone) :
	cNode(id)
,	m_buffers(NULL)
,	m_isBoneMesh(true)
{
	// 뼈 메쉬는 동적으로 메쉬버퍼를 생성한다. (디버깅용)
	m_buffers = new cMeshBuffer(renderer, rawBone);
}

cMesh::~cMesh()
{
	if (m_isBoneMesh)
		SAFE_DELETE(m_buffers);
}


// 매터리얼 생성.
void cMesh::CreateMaterials(cRenderer &renderer, const sRawMesh &rawMesh)
{
	if (rawMesh.mtrls.empty())
	{
		m_mtrls.push_back(cMaterial());
		m_mtrls.back().Init(rawMesh.mtrl);

		if (!rawMesh.mtrl.texture.empty())
		{
			m_colorMap.resize(1);
			m_colorMap[0] = cResourceManager::Get()->LoadTexture(renderer, rawMesh.mtrl.directoryPath, rawMesh.mtrl.texture);
		}
	}
	else
	{
		m_colorMap.resize(rawMesh.mtrls.size(), NULL);
		m_normalMap.resize(rawMesh.mtrls.size(), NULL);
		m_specularMap.resize(rawMesh.mtrls.size(), NULL);
		m_selfIllumMap.resize(rawMesh.mtrls.size(), NULL);
		m_mtrls.reserve(rawMesh.mtrls.size());

		for (u_int i=0; i < rawMesh.mtrls.size(); ++i)
		{
			const sMaterial &mtrl = rawMesh.mtrls[ i];

			m_mtrls.push_back(cMaterial());
			m_mtrls.back().Init(mtrl);

			m_colorMap[ i] = cResourceManager::Get()->LoadTexture(renderer, mtrl.directoryPath, mtrl.texture);

			if (!mtrl.bumpMap.empty())
				m_normalMap[i] = cResourceManager::Get()->LoadTexture(renderer, mtrl.directoryPath, mtrl.bumpMap);

			if (!mtrl.specularMap.empty())
				m_specularMap[i] = cResourceManager::Get()->LoadTexture(renderer, mtrl.directoryPath, mtrl.specularMap);

			if (!mtrl.selfIllumMap.empty())
				m_selfIllumMap[i] = cResourceManager::Get()->LoadTexture(renderer, mtrl.directoryPath, mtrl.selfIllumMap);
		}
	}
}


// Animation
bool cMesh::Update(const float deltaSeconds)
{
	return true;
}


// Render
void cMesh::Render(cRenderer &renderer, const Matrix44 &parentTm)
{
	RET(!IsRender());
	RET(!m_buffers);

	if (m_buffers->GetAttributes().empty())
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		renderer.GetDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&tm );

		if (!m_mtrls.empty())
			m_mtrls[ 0].Bind(renderer);
		if (!m_colorMap.empty())
			m_colorMap[ 0]->Bind(renderer, 0);

		m_buffers->Bind(renderer);
		m_buffers->Render(renderer);
	}
	else
	{
		m_buffers->Bind(renderer);

		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		renderer.GetDevice()->SetTransform( D3DTS_WORLD, (D3DXMATRIX*)&tm );

		for (u_int i=0; i < m_buffers->GetAttributes().size(); ++i)
		{
			const int mtrlId = m_buffers->GetAttributes()[ i].attribId;
			if ((int)m_mtrls.size() <= mtrlId)
				continue;
			
			m_mtrls[ mtrlId].Bind(renderer);
			if (m_colorMap[ mtrlId])
				m_colorMap[ mtrlId]->Bind(renderer, 0);

			m_buffers->Render( renderer,
				m_buffers->GetAttributes()[ i].faceStart*3, 
				m_buffers->GetAttributes()[ i].faceCount);
		}
	}
}


// 셰이더를 통해 화면을 그린다.
void cMesh::RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &parentTm)
{
	RET(!IsRender());
	RET(!m_buffers);

	if (m_buffers->GetAttributes().empty())
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		shader.SetMatrix("g_mWorld", tm);
		
		const bool isNormalMapping = (!m_normalMap.empty()) && 
			(m_normalMap[ 0] && m_normalMap[ 0]->GetTexture());

		const bool isSpecularMapping = (!m_specularMap.empty()) && 
			(m_specularMap[ 0] && m_specularMap[ 0]->GetTexture());

		const bool isSelfIllumMapping = (!m_selfIllumMap.empty()) && 
			(m_selfIllumMap[ 0] && m_selfIllumMap[ 0]->GetTexture());

		if (!m_mtrls.empty())
			m_mtrls[ 0].Bind(shader);
		if (!m_colorMap.empty())
			m_colorMap[ 0]->Bind(shader, "g_colorMapTexture");
		if (isNormalMapping)
			m_normalMap[ 0]->Bind(shader, "g_normalMapTexture");
		if (isSpecularMapping)
			m_specularMap[ 0]->Bind(shader, "g_specularMapTexture");
		if (isSelfIllumMapping)
			m_selfIllumMap[ 0]->Bind(shader, "g_selfIllumMapTexture");

		shader.SetRenderPass(isNormalMapping? 4 : 0);

		m_buffers->Bind(renderer);

		shader.Begin();
		shader.BeginPass();
		shader.CommitChanges();

		renderer.GetDevice()->DrawIndexedPrimitive( 
			D3DPT_TRIANGLELIST, 0, 0, 
			m_buffers->GetVertexBuffer().GetVertexCount(), 0, 
			m_buffers->GetIndexBuffer().GetFaceCount());

		shader.EndPass();
		shader.End();
	}
	else
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		shader.SetMatrix("g_mWorld", tm);

		shader.Begin();

		m_buffers->Bind(renderer);

		for (u_int i=0; i < m_buffers->GetAttributes().size(); ++i)
		{
			const int mtrlId = m_buffers->GetAttributes()[ i].attribId;
			if ((int)m_mtrls.size() <= mtrlId)
				continue;

			const bool isNormalMapping = m_normalMap[ mtrlId] && 
				m_normalMap[ mtrlId]->GetTexture();

			const bool isSpecularMapping = m_specularMap[ mtrlId] && 
				m_specularMap[ mtrlId]->GetTexture();

			const bool isSelfIllumMapping = m_selfIllumMap[ mtrlId] && 
				m_selfIllumMap[ mtrlId]->GetTexture();

			m_mtrls[ mtrlId].Bind(shader);
			if (m_colorMap[ mtrlId])
				m_colorMap[ mtrlId]->Bind(shader, "g_colorMapTexture");
			if (isNormalMapping)
				m_normalMap[ mtrlId]->Bind(shader, "g_normalMapTexture");
			if (isSpecularMapping)
				m_specularMap[ mtrlId]->Bind(shader, "g_specularMapTexture");
			if (isSelfIllumMapping)
				m_selfIllumMap[ mtrlId]->Bind(shader, "g_selfIllumMapTexture");

			shader.SetRenderPass(isNormalMapping? 4 : 0);

			shader.BeginPass();

			renderer.GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 
				m_buffers->GetVertexBuffer().GetVertexCount(), 
				m_buffers->GetAttributes()[ i].faceStart*3, 
				m_buffers->GetAttributes()[ i].faceCount);

			shader.EndPass();
		}

		shader.End();
	}
}


// 그림자 출력.
void cMesh::RenderShadow(cRenderer &renderer, const Matrix44 &viewProj,
	const Vector3 &lightPos, const Vector3 &lightDir, const Matrix44 &parentTm)
{
	RET(!IsRender());
	RET(!m_shader);
	RET(!m_buffers);

	//const cLight &mainLight = cLightManager::Get()->GetMainLight();
	//mainLight.Bind(*m_shader);

	if (m_buffers->GetAttributes().empty())
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		m_shader->SetMatrix("g_mWorld", tm);

		m_buffers->Bind(renderer);

		m_shader->Begin();
		m_shader->BeginPass(1);

		renderer.GetDevice()->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST, 0, 0, 
			m_buffers->GetVertexBuffer().GetVertexCount(), 0, 
			m_buffers->GetIndexBuffer().GetFaceCount());

		m_shader->EndPass();
		m_shader->End();
	}
	else
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		m_shader->SetMatrix("g_mWorld", tm);

		//Matrix44 wit = tm.Inverse();
		//wit.Transpose();
		//m_shader->SetMatrix("g_mWIT", wit);

		m_shader->Begin();

		m_buffers->Bind(renderer);

		for (u_int i=0; i < m_buffers->GetAttributes().size(); ++i)
		{
			const int mtrlId = m_buffers->GetAttributes()[ i].attribId;
			if ((int)m_mtrls.size() <= mtrlId)
				continue;

			m_shader->BeginPass(1);

			renderer.GetDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
				m_buffers->GetVertexBuffer().GetVertexCount(), 
				m_buffers->GetAttributes()[ i].faceStart*3, 
				m_buffers->GetAttributes()[ i].faceCount);

			m_shader->EndPass();
		}

		m_shader->End();
	}

}


void cMesh::RenderShadow(cRenderer &renderer, cShader &shader, const Matrix44 &parentTm)
{
	RET(!IsRender());
	RET(!m_buffers);

	if (m_buffers->GetAttributes().empty())
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		shader.SetMatrix("mWorld", tm);

		Matrix44 wit = tm.Inverse();
		wit.Transpose();
		shader.SetMatrix("mWIT", wit);

		//const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		//shader.SetMatrix("mWorld", tm);
		//Matrix44 wit = tm.Inverse();
		//wit.Transpose();
		//shader.SetMatrix("mWIT", wit);
		//if (!m_mtrls.empty())
		//	m_mtrls[ 0].Bind(shader);
		//if (!m_textures.empty())
		//	m_textures[ 0]->Bind(shader, "colorMapTexture");

		m_buffers->Bind(renderer);

		shader.Begin();
		shader.BeginPass();

		renderer.GetDevice()->DrawIndexedPrimitive( 
			D3DPT_TRIANGLELIST, 0, 0, 
			m_buffers->GetVertexBuffer().GetVertexCount(), 0, 
			m_buffers->GetIndexBuffer().GetFaceCount());

		shader.End();
		shader.EndPass();
	}
	else
	{
		const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		shader.SetMatrix("mWorld", tm);

		Matrix44 wit = tm.Inverse();
		wit.Transpose();
		shader.SetMatrix("mWIT", wit);

		//const Matrix44 tm = m_localTM * m_aniTM * m_TM * parentTm;
		//shader.SetMatrix("mWorld", tm);
		//Matrix44 wit = tm.Inverse();
		//wit.Transpose();
		//shader.SetMatrix("mWIT", wit);

		shader.Begin();
		m_buffers->Bind(renderer);
		//m_vtxBuff.Bind();
		//m_idxBuff.Bind();

		for (u_int i=0; i < m_buffers->GetAttributes().size(); ++i)
		{
			const int mtrlId = m_buffers->GetAttributes()[ i].attribId;
			if ((int)m_mtrls.size() <= mtrlId)
				continue;

			//m_mtrls[ mtrlId].Bind(shader);
			//if (m_textures[ mtrlId])
			//	m_textures[ mtrlId]->Bind(shader, "colorMapTexture");

			shader.BeginPass();

			renderer.GetDevice()->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 
				m_buffers->GetVertexBuffer().GetVertexCount(), 
				m_buffers->GetAttributes()[ i].faceStart*3, 
				m_buffers->GetAttributes()[ i].faceCount);

			shader.EndPass();
		}

		shader.End();
	}
}


// 경계박스 리턴.
const cBoundingBox* cMesh::GetBoundingBox()
{
	RETV (!m_buffers, NULL);
	return &m_buffers->GetBoundingBox();
}
