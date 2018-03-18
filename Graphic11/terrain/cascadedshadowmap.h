//
// 2017-09-27, jjuiddong
// Cascaded ShadowMap for Terrain
//
#pragma once


namespace graphic
{
	using namespace common;

	class cCascadedShadowMap
	{
	public:
		cCascadedShadowMap();
		virtual ~cCascadedShadowMap();

		bool Create(cRenderer &renderer
			, const Vector2 &shadowMapSize = Vector2(1024,1024)
			, const float z0 = 30.f
			, const float z1 = 100.f
			, const float z2 = 300.f
		);
		bool UpdateParameter(cRenderer &renderer, const cCamera &camera);
		bool Bind(cRenderer &renderer);
		bool Begin(cRenderer &renderer, const int shadowMapIndex, const bool isClear=true);
		bool End(cRenderer &renderer, const int shadowMapIndex);
		void BuildShadowMap(cRenderer &renderer, cNode *node, const XMMATRIX &parentTm = XMIdentity, const bool isClear = true);
		void RenderShadowMap(cRenderer &renderer, cNode *node, const XMMATRIX &parentTm  = XMIdentity);
		void DebugRender(cRenderer &renderer);


	public:
		enum { SHADOWMAP_COUNT = 3 };
		Vector2 m_shadowMapSize;
		float m_splitZ[3];
		cCamera3D m_lightCams[SHADOWMAP_COUNT];
		cDbgFrustum m_frustums[SHADOWMAP_COUNT];
		cDepthBuffer m_shadowMaps[SHADOWMAP_COUNT];
		cQuad2D m_shadowMapQuads[SHADOWMAP_COUNT];
		cDbgFrustum m_dbgLightFrustums[SHADOWMAP_COUNT];
	};

}
