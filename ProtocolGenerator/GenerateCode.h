//------------------------------------------------------------------------
// Name:    GenerateProtocolCode.h
// Author:  jjuiddong
// Date:    12/26/2012
// 
// �������� ����Ʈ���� Protocol �ҽ� ������ �����Ѵ�.
//------------------------------------------------------------------------
#pragma once

namespace compiler
{

	bool WriteProtocolCode(const string &protocolFileName
		, network2::sRmi *rmi
		, const string &pchFileName, const string &marshallingName);

}
