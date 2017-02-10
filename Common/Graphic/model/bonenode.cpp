
#include "stdafx.h"
#include "bonenode.h"


using namespace graphic;


cBoneNode::cBoneNode(cRenderer &renderer, const int id, vector<Matrix44> &palette, const sRawBone &rawBone) :
	cNode(id, rawBone.name)
,	m_track(NULL)
,	m_mesh(NULL)
,	m_palette(palette)
{
	m_offset = rawBone.worldTm.Inverse();
	m_localTM = rawBone.localTm;

	m_track = new cBlendTrack();

	// debug 용
	m_mesh = new cMesh(renderer, id, rawBone);

}

cBoneNode::~cBoneNode()
{
	SAFE_DELETE( m_track );
	SAFE_DELETE( m_mesh );
}


void cBoneNode::SetAnimation( const sRawAni &rawAni, 
	const int totalAniFrame,  //= 0;
	const bool isLoop, //bLoop=false
	const bool isBlend // true
	)
{
	m_track->PlayAnimation(m_localTM, rawAni, totalAniFrame, isLoop, isBlend);
}


// 애니메이션 설정.
void cBoneNode::SetAnimation( const sRawBone &rawBone, const sRawAni &rawAni, 
	const int totalAniFrame,  //= 0;
	const bool isLoop, //bLoop=false
	const bool isBlend // true
	)
{
	m_track->PlayAnimation(rawBone.localTm, rawAni, totalAniFrame, isLoop, isBlend);
}


// 에니메이션.
bool cBoneNode::Update(const float deltaSeconds)
{
	if (m_id < 0)
	{ // 최상위 루트 노드일 때는 애니메이션 처리를 하지 않는다.
		for each (auto p in m_children)
			p->Update(deltaSeconds);
		return true;
	}

	// 애니메이션 중이 아니면 종료.
	if (!m_track || (m_track && !m_track->IsAnimation()))
		return true;

	// 애니메이션 행렬 계산.
	if (m_track->Move(deltaSeconds, m_TM, m_aniTM, m_accTM))
	{
		if (m_parent)
			m_accTM = m_accTM * ((cBoneNode*)m_parent)->m_accTM;
		m_palette[ m_id] = m_offset * m_accTM;
	}

	// 자식 노드 애니메이션 처리.
	for each (auto p in m_children)
		p->Update(deltaSeconds);

	return true;
}


// 뼈 노드 출력
void cBoneNode::Render(cRenderer &renderer, const Matrix44 &parentTm)
{
	RET(!m_mesh);

	// 본 노드 메쉬는 world 좌표계로 되어있기 때문에 그냥 그리면 된다.
	if (m_track)
		m_mesh->Render(renderer, m_offset * m_accTM * parentTm);
	else
		m_mesh->Render(renderer, parentTm);

	for each (auto p in m_children)
		p->Render( renderer, parentTm );
}


void cBoneNode::SetCurrentFrame(const int curFrame)
{ 
	RET(!m_track);

	m_track->SetCurrentFrame(curFrame, m_TM, m_aniTM, m_accTM);

	// 애니메이션 행렬 업데이트. 
	if (m_id >= 0)
	{
		if (m_parent)
			m_accTM = m_accTM * ((cBoneNode*)m_parent)->m_accTM;
		m_palette[ m_id] = m_offset * m_accTM;
	}
}


// m_accTM 을 업데이트 하고, 팔레트를 업데이트 한다.
void cBoneNode::UpdateAccTM()
{
	RET(!m_track);
	RET(m_id < 0);

	m_track->GetCalculateTM(m_localTM, m_aniTM, m_TM, m_accTM);

	if (m_parent)
		m_accTM = m_accTM * ((cBoneNode*)m_parent)->m_accTM;
	m_palette[ m_id] = m_offset * m_accTM;
}


Matrix44 cBoneNode::GetCalculateAniTM()
{
	RETV(!m_track, Matrix44::Identity);
	m_track->GetCalculateTM(m_localTM, m_aniTM, m_TM, m_accTM);
	return m_accTM;
}


int cBoneNode::GetCurrentFrame() const
{
	RETV(!m_track, 0);
	return m_track->GetCurrentFrame();
}


// cBlendTrack 에서 localTM 을 계산하고 있기 때문에
// cBlendTrack 값도 바꿔줘야 한다.
void cBoneNode::UpdateLocalTM(const Matrix44 &tm)
{
	RET(!m_track);
	SetLocalTM(tm);
	m_track->UpdateLocalTM(tm);
}
