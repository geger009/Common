//
// 2019-01-08, jjuiddong
// Protocol Dispatcher
//	packet dispatcher 
//
// ��Ʈ��ũ�κ��� ���� ��Ŷ�� �ش��ϴ� �������ݿ� �°� �м��ؼ� Handler�� ȣ���Ѵ�.
// Dispatch �Լ����� ���� �������� �Լ����� ȣ���ϰ� �ȴ�.
//
// Network Protocol Generation Compiler generates Dispatch () as a source file.
//
// 2020-11-11
//	- add packet format type (ePacketFormat)
//
#pragma once


namespace network2
{
	
	class cProtocolDispatcher
	{
	public:
		cProtocolDispatcher(const int id, const ePacketFormat format = ePacketFormat::BINARY);
		virtual ~cProtocolDispatcher();

		int GetId() const { return m_id; }
		void SetCurrentDispatchPacket(cPacket *pPacket);
		virtual bool Dispatch(cPacket &packet, const vector<iProtocolHandler*> &handlers) = 0;


	public:
		int m_id; // dispatcher id, same as protocol id
		ePacketFormat m_format;
		cPacket *m_packet; // reference

		//-----------------------------------------------------------------
		// singleton dispatchermap
		static map<int, cProtocolDispatcher*>* GetDispatcherMap() {
			if (!s_dispatchers)
				s_dispatchers = new map<int, cProtocolDispatcher*>();
			return s_dispatchers;
		}
		static void ReleaseDispatcherMap() {
			SAFE_DELETE(s_dispatchers);
		}
		static map<int, cProtocolDispatcher*> *s_dispatchers;
	};

}
