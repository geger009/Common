//
// 2017-08-08, jjuiddong
//	- Upgrade DX11
//		������ Ŭ����.
//		�ذ� å ������ ��ƼŬ�� ������.
//
#pragma once


namespace graphic
{

	// ������ ���� ��.
	namespace BILLBOARD_TYPE {
		enum TYPE
		{
			NONE,
			Y_AXIS,		// Y Axis Rotation
			ALL_AXIS,	// X-Y-Z Axis Rotation
			ALL_AXIS_Z, // X-Y-Z Axis Rotation and Z Axis is Up
			DYN_SCALE,
		};
	};


	class cBillboard : public cQuad
	{
	public:		
		cBillboard();
		virtual ~cBillboard();

		bool Create(cRenderer &renderer, const BILLBOARD_TYPE::TYPE type
			, const float width, const float height, const Vector3 &pos
			, const StrPath &textureFileName = ""
			, const bool isSizePow2 = true
			, const float dynScaleMin = 0.5f
			, const float dynScaleMax = 200.5f
			, const float dynScaleAlpha = 1.f
		);

		virtual bool Render(cRenderer &renderer, const XMMATRIX &parentTm = XMIdentity, const int flags = 1) override;
		void Rotate();


	public:
		BILLBOARD_TYPE::TYPE m_type;
		Vector3 m_scale; // for dynamic scale
		Vector3 m_normal;
		float m_dynScaleMin; // default: 0.5f
		float m_dynScaleMax; // default: 200.5f
		float m_dynScaleAlpha; // default: 1.f;
	};

}
