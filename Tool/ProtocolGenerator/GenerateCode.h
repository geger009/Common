//------------------------------------------------------------------------
// Name:    GenerateProtocolCode.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// �������� ����Ʈ���� Protocol �ҽ� ������ �����Ѵ�.
//
// 2020-11-10
//	- add json format string send/recv
//	- remove marshallingName
//
//------------------------------------------------------------------------
#pragma once

namespace compiler
{

	bool WriteProtocolCode(const string &protocolFileName
		, network2::sRmi *rmi
		, const string &pchFileName
	);

}
