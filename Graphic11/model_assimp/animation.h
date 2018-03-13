//
// 2017-03-25, jjuiddong
// �� ĳ������ ��� �ִϸ��̼��� �����Ѵ�.
//
#pragma once


namespace graphic
{
	using namespace common;

	class cAnimation
	{
	public:
		cAnimation();
		virtual ~cAnimation();

		bool Create(cSkeleton *skeleton);
		bool Create(const sRawAniGroup &aniGroup, cSkeleton *skeleton, const bool isMerge=false);
		bool Update(const float increasedTime);
		bool SetAnimation(const StrId &animationName, const bool isMerge = false);
		bool SetAnimation(const int animationIndex, const bool isMerge = false);
		void Stop();
		void Play();


	public:
		struct eState {enum Enum {STOP,PLAY,};};
		eState::Enum m_state;
		cSkeleton *m_skeleton; // reference
		const sRawAniGroup *m_aniGroup; // reference
		vector<vector<cAnimationNode>> m_anies;
		int m_curAniIdx;
		bool m_isMergeAni;
		float m_start;
		float m_end;
	};

}