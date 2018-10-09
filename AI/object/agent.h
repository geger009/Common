//
// �ΰ����� ��ü �������̽�
// �ΰ������� �����Ϸ��� ��ü�� �� �������̽��� ��ӹ޾ƾ� �Ѵ�.
//
// 2018-10-03
//		- agent Ŭ������ ai���� ������. ������ ����������, �߻�Ŭ������ �ǵ��� ���̰� ����
//
#pragma once


namespace graphic {
	class cNode;
}

namespace ai
{
	using namespace common;
	static Transform g_tempTransform;

	template<class T>
	class iAgent
	{
	public:
		iAgent(T *p)  : m_ptr(p) {}
		virtual ~iAgent() {}

		virtual Transform& aiGetTransform() { return g_tempTransform; }
		virtual void aiSetAnimation(const Str64 &animationName) {}


	public:
		T *m_ptr;
	};

}
