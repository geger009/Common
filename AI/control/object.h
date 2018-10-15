//
// 2016-05-14, jjuiddong
//
// �޼����� ó���ϴ� �⺻ Ŭ����
//
#pragma once


namespace ai
{

	class cObject
	{
	public:
		cObject(const int id=-1) : m_id( ((id == -1)? common::GenerateId() : id) ) {}
		virtual ~cObject() {}
		virtual bool Update(const float deltaSeconds) { return true; }
		virtual void DispatchMsg(const sMsg &msg) {}

	public:
		int m_id; // auto generate id
	};

}
