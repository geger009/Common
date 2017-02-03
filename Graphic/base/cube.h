// 정육면체를 표현하는 클래스.
// Vertext + Diffuse 로 표현된다.
// VertexBuffer + IndexBuffer
#pragma once


namespace graphic
{
	class cRenderer;

	class cCube
	{
	public:
		cCube();
		cCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax);

		void InitCube(cRenderer &renderer);
		void SetCube(cRenderer &renderer, const Vector3 &vMin, const Vector3 &vMax);
		void SetCube(cRenderer &renderer, const cCube &cube);
		void SetTransform( const Matrix44 &tm );
		void SetColor( DWORD color);
		const Matrix44& GetTransform() const;
		const Vector3& GetMin() const;
		const Vector3& GetMax() const;
		const float Length() const; // length(min - max)

		void Render(cRenderer &renderer, const Matrix44 &tm);
		void RenderShader(cRenderer &renderer, cShader &shader, const Matrix44 &tm = Matrix44::Identity);


	protected:
		cVertexBuffer m_vtxBuff;
		cIndexBuffer m_idxBuff;
		Vector3 m_min;
		Vector3 m_max;
		Matrix44 m_tm;
	};	


	inline void cCube::SetTransform( const Matrix44 &tm ) { m_tm = tm; }
	inline const Matrix44& cCube::GetTransform() const { return m_tm; }
	inline const Vector3& cCube::GetMin() const { return m_min; }
	inline const Vector3& cCube::GetMax() const { return m_max; }
	inline const float cCube::Length() const { return (m_min - m_max).Length(); }
}
