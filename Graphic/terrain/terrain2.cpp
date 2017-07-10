
#include "stdafx.h"
#include "terrain2.h"
#include "tile.h"


using namespace graphic;


cTerrain2::cTerrain2()
	: m_isShowDebug(false)
	, m_isShadow(true)
	, m_rows(0)
	, m_cols(0)
{
}

cTerrain2::~cTerrain2()
{
	Clear();
}


bool cTerrain2::Create(cRenderer &renderer, const sRectf &rect)
{
	const Vector2 center = rect.Center();
	//const Vector3 lightLookat = Vector3(center.x, 0, center.y);
	//const Vector3 lightPos = Vector3(1, -1, 1).Normal() * -400.f + lightLookat;
	const Vector3 lightLookat = Vector3(center.x, 0, center.y);
	const Vector3 lightPos = GetMainLight().GetDirection()	* -400.f + lightLookat;

	const Vector3 p0 = lightPos;
	const Vector3 p1 = (lightLookat - lightPos).Normal() * 3 + p0;
	m_dbgLight.Create(renderer, p0, p1, 0.5F);

	const int shadowWidth = 1024;
	const int shadowHeight = 1024;
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
	{
		m_shadowMap[i].Create(renderer, 1024, 1024);

		m_frustum[i].Create(renderer, GetMainCamera()->GetViewProjectionMatrix());
		m_frustum[i].m_fullCheck = true;

		m_lightCam[i].Init(&renderer);
		m_lightCam[i].SetCamera(lightPos, lightLookat, Vector3(0, 1, 0));
		m_lightCam[i].SetProjectionOrthogonal((float)shadowWidth, (float)shadowHeight, 0.1f, 1000.0f);
		m_lightCam[i].SetViewPort(100, 100);

		m_dbgLightFrustum[i].Create(renderer, m_lightCam[i].GetViewProjectionMatrix());
	}

	m_dbgPlane.SetLine(renderer, Vector3(0, 0, 0), Vector3(0, 30, 0), 0.1f);

	return true;
}


void cTerrain2::Update(cRenderer &renderer, const float deltaSeconds)
{
	for (auto &p : m_tiles)
		p->Update(renderer, deltaSeconds);
}


void cTerrain2::PreRender(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
	, const int shadowMapIdx //= 0
)
{
	cAutoCam cam(&m_lightCam[shadowMapIdx]);

	m_shadowMap[shadowMapIdx].Begin(renderer);
	for (auto &p : m_tiles)
		p->PreRender(renderer, tm);
	m_shadowMap[shadowMapIdx].End(renderer);
}


void cTerrain2::Render(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	UpdateShader(renderer);

	for (auto &p : m_tiles)
		p->Render2(renderer, m_lightView, m_lightProj, m_lightTT
			, m_shadowMap, SHADOWMAP_COUNT, tm, 0x1);

	// Alphablending 1 (Camera)
	for (auto &p : m_tiles)
		p->Render2(renderer, m_lightView, m_lightProj, m_lightTT
			, m_shadowMap, SHADOWMAP_COUNT, tm, 0x8);

	// Alphablending 2 (Transparent Wall)
	for (auto &p : m_tiles)
		p->Render2(renderer, m_lightView, m_lightProj, m_lightTT
			, m_shadowMap, SHADOWMAP_COUNT, tm, 0x4);

	if (m_isShowDebug)
	{
		for (int i = 0; i < SHADOWMAP_COUNT; ++i)
		{
			m_shadowMap[i].Render(renderer, i+1);
			//m_frustum[i].RenderShader(renderer);
			//m_dbgLightFrustum[i].RenderShader(renderer);
		}
		m_dbgLight.Render(renderer);
		m_dbgPlane.Render(renderer);
	}
}


void cTerrain2::RenderOption(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
	, const int option //= 0x1
)
{
	for (auto &p : m_tiles)
		p->Render2(renderer, m_lightView, m_lightProj, m_lightTT
			, m_shadowMap, SHADOWMAP_COUNT, tm, option);
}


void cTerrain2::RenderDebug(cRenderer &renderer
	, const Matrix44 &tm //= Matrix44::Identity
)
{
	if (m_isShowDebug)
	{
		for (int i = 0; i < SHADOWMAP_COUNT; ++i)
		{
			m_shadowMap[i].Render(renderer, i + 1);
			//m_frustum[i].RenderShader(renderer);
			//m_dbgLightFrustum[i].RenderShader(renderer);
		}
		m_dbgLight.Render(renderer);
		m_dbgPlane.Render(renderer);
	}
}



void cTerrain2::UpdateShader(cRenderer &renderer)
{
	RET(m_tiles.empty());

	cCamera *cam = GetMainCamera();

	cShader *shader = m_tiles[0]->m_ground.m_shader;
	cam->Bind(*shader);
	GetMainLight().Bind(*shader);
	cam->Bind(*m_frustum[0].m_shader);

	for (auto &shader : m_shaders)
	{
		GetMainLight().Bind(*shader);
		cam->Bind(*shader);
	}		
}


void cTerrain2::CullingTest(
	cRenderer &renderer
	, cCamera &camera
	, const bool isModel //= true
	, const int shadowMapIdx //= 0
)
{
	m_frustum[shadowMapIdx].SetFrustum(renderer, camera.GetViewProjectionMatrix());

	for (auto &p : m_tiles)
		p->CullingTest(m_frustum[shadowMapIdx], isModel);

	// Update Light Position, Direction
	Vector3 orig, dir;
	const int x = camera.m_width / 2;
	const int y = (int)(camera.m_height * 0.8f);
	camera.GetRay(x, y, orig, dir);
	const Plane ground(Vector3(0, 1, 0), 0);
	const Vector3 pos = ground.Pick(orig, dir);
	m_dbgPlane.SetLine(renderer, pos, pos + Vector3(0, 1, 0) * 10, 0.1f);

	const Vector3 lightDir = m_lightCam[shadowMapIdx].GetDirection();
	//const Vector3 lightPos = pos + Vector3(0, 1, 0) * 10 + lightDir*-30.f
	//	+ Vector3(dir.x, 0, dir.z).Normal() * 10;

	//const float f = common::clamp(0.05f, 1.f, orig.y * 0.005f);
	const float len = (pos- orig).Length();
	const float f = common::clamp(0.0003f, 1.f, len * 0.000015f);
	//const float f = common::clamp(0.003f, 1.f, len * 0.00015f);
	m_lightCam[shadowMapIdx].FitFrustum(camera, f);

	Matrix44 view, proj, tt;
	m_lightCam[shadowMapIdx].GetShadowMatrix(view, proj, tt);
	m_lightView[ shadowMapIdx] = view;
	m_lightProj[ shadowMapIdx] = proj;
	m_lightTT[ shadowMapIdx] = tt;

	if (m_isShowDebug)
	{ 
		m_dbgLight.SetDirection(m_lightCam[shadowMapIdx].GetEyePos(), m_lightCam[shadowMapIdx].GetEyePos() + lightDir*1.f, 0.5f);
		m_dbgLightFrustum[shadowMapIdx].Create(renderer, m_lightCam[shadowMapIdx].GetViewProjectionMatrix());
	}
}


void cTerrain2::CullingTest(cRenderer &renderer
	, const cFrustum &frustum
	, const bool isModel //= true
	, const int shadowMapIdx //= 0
)
{
	for (auto &p : m_tiles)
		p->CullingTest(frustum, isModel);

	m_frustum[shadowMapIdx].SetFrustum(renderer, frustum.m_viewProj);

	Vector3 vtxQuad[4];
	const Plane ground(Vector3(0, 1, 0), 0);
	frustum.GetGroundPlaneVertices(ground, vtxQuad);
	m_lightCam[shadowMapIdx].FitQuad(vtxQuad);

	Matrix44 view, proj, tt;
	m_lightCam[shadowMapIdx].GetShadowMatrix(view, proj, tt);
	m_lightView[ shadowMapIdx] = view;
	m_lightProj[ shadowMapIdx] = proj;
	m_lightTT[ shadowMapIdx] = tt;

	if (m_isShowDebug)
	{
		const Vector3 lightDir = m_lightCam[shadowMapIdx].GetDirection();
		m_dbgLight.SetDirection(m_lightCam[shadowMapIdx].GetEyePos(), m_lightCam[shadowMapIdx].GetEyePos() + lightDir*1.f, 0.5f);
		m_dbgLightFrustum[shadowMapIdx].Create(renderer, m_lightCam[shadowMapIdx].GetViewProjectionMatrix());
	}
}


void cTerrain2::CullingTestOnly(cRenderer &renderer, cCamera &camera
	, const bool isModel //= true
)
{
	cFrustum frustum;
	frustum.SetFrustum(camera.GetViewProjectionMatrix());

	for (auto &p : m_tiles)
		p->CullingTest(frustum, isModel);
}


bool cTerrain2::AddTile(cTile *tile)
{
	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() != it)
		return false; // Already Exist

	m_tiles.push_back(tile);
	m_tilemap[tile->m_name.GetHashCode()] = tile;
	m_tilemap2[tile->m_id] = tile;
	return true;
}


cTile* cTerrain2::FindTile(const Str64 &name)
{
	const hashcode hcode = name.GetHashCode();
	auto it = m_tilemap.find(hcode);
	if (it == m_tilemap.end())
		return NULL;
	return it->second;
}


cTile* cTerrain2::FindTile(const int id)
{
	auto it = m_tilemap2.find(id);
	if (it == m_tilemap2.end())
		return NULL;
	return it->second;
}


cModel2* cTerrain2::FindModel(const int modelId)
{
	for (auto &p : m_tiles)
		if (auto r = p->FindModel(modelId))
			return r;
	return NULL;
}


bool cTerrain2::RemoveTile(cTile *tile)
{
	auto it = std::find(m_tiles.begin(), m_tiles.end(), tile);
	if (m_tiles.end() == it)
		return false; // Not Exist

	common::popvector2(m_tiles, tile);
	m_tilemap.erase(tile->m_name.GetHashCode());
	m_tilemap2.erase(tile->m_id);
	return true;
}


void cTerrain2::SetDbgRendering(const bool isRender)
{
	m_isShowDebug = isRender;

	for (auto &p : m_tiles)
	{
		p->m_isDbgRender = isRender;

		for (auto &ch : p->m_children)
			ch->m_isDbgRender = isRender;
	}
}


void cTerrain2::SetShadowRendering(const bool isRender)
{
	for (auto &p : m_tiles)
		p->m_isShadow = isRender;
}


void cTerrain2::LostDevice()
{
	for (auto &p : m_tiles)
		p->LostDevice();
	for (int i = 0; i < SHADOWMAP_COUNT; ++i)
		m_shadowMap[i].LostDevice();
}


void cTerrain2::ResetDevice(cRenderer &renderer)
{
	for (auto &p : m_tiles)
		p->ResetDevice(renderer);
	for (int i=0; i < SHADOWMAP_COUNT; ++i)
		m_shadowMap[i].ResetDevice(renderer);
}


void cTerrain2::Clear()
{
	for (auto &p : m_tiles)
		delete p;
	m_tiles.clear();

	m_tilemap.clear();
	m_tilemap2.clear();
	m_shaders.clear();
}


//
// tile width, height, row, col count
// tile
//		- pos
//		- texture
// model
//		- filename
//		- name
//		- pos
//		- scale
//		- rot
//
bool cTerrain2::Write(const StrPath &fileName)
{

	return true;
}


bool cTerrain2::Read(const StrPath &fileName)
{

	return true;
}
