
#include "pch.h"
#include "GenerateCode.h"
#include <direct.h>

using namespace network2;
using namespace std;

namespace compiler
{
	// Write Protocol Handler Code
	bool WriteFirstHandlerHeader(sRmi *rmi, const string &marshallingName);
	bool WriteHandlerHeader(ofstream &fs, sRmi *rmi);
	bool WriteFirstHandlerCpp(sRmi *rmi, const string &pchFileName);
	bool WriteHandlerCpp(ofstream &fs, sRmi *rmi);


	void WriteDeclProtocolList(ofstream &fs, sProtocol *pProtocol, bool isVirtual, bool isImpl, bool isTarget);
	void WriteFirstArg(ofstream &fs, sArg*p, bool isTarget);
	void WriteArg(ofstream &fs, sArg *arg, bool comma);
	void WriteArgVar(ofstream &fs, sArg *arg, bool comma);


	// Write Protocol Data Code
	bool WriteFirstDataHeader(sRmi *rmi, const string &marshallingName);
	bool WriteProtocolDataHeader(ofstream &fs, sRmi *rmi);
	bool WriteProtocolData(ofstream &fs, sProtocol *pProtocol);
	void WriteProtocolDataArg(ofstream &fs, sArg *arg);



	// Write Dispatcher
	void WriteProtocolDispatchFunc(ofstream &fs, sRmi *rmi);
	void WriteDispatchSwitchCase(ofstream &fs, sProtocol *pProtocol);
	void WriteDispatchImpleArg(ofstream &fs, sArg*p);
	void WriteLastDispatchSwitchCase(ofstream &fs, sProtocol *pProtocol);
	void WriteDispatchImpleArg2(ofstream &fs, sArg*p);
	void WriteLastDispatchSwitchCase2(ofstream &fs, sProtocol *pProtocol);


	// Write Protocol Code
	bool WriteFirstProtocolClassHeader(sRmi *rmi, const string &marshallingName);
	bool WriteProtocolClassHeader(ofstream &fs, sRmi *rmi);
	bool WriteFirstProtocolCpp(sRmi *rmi, const string &pchFileName);
	bool WriteProtocolCpp(ofstream &fs, sRmi *rmi);

	void WriteImplProtocolList(ofstream &fs, sProtocol *pProtocol);
	void WriteFirstImpleProtocol(ofstream &fs, sProtocol *pProtocol, sArg*p);
	void WriteImpleArg(ofstream &fs, sArg*p);
	void WriteLastImpleProtocol(ofstream &fs);

	string GetFileNameExceptExt(const string &fileName);
	string GetProtocolName(const string &fileName);
	string GetProtocolClassName(const string &protocolName, const string &rmiName );
	string GetProtocolHandlerClassName(const string &protocolName, const string &rmiName );
	string GetProtocolDispatcherClassName(const string &protocolName, const string &rmiName );


	string g_className; // Protocol, ProtocolHandler �������� ����ϰ� �ִ�.
	string g_protocolId;
	string n_fileName;
	string g_protocolName;	// *.prt ������ Ȯ���ڿ� ��θ� ������ �����̸��� �����Ѵ�.
	string n_OrigianlFileName;
	string g_handlerClassName;
	string n_SrcFolderName = "Src";
}

using namespace compiler;


//------------------------------------------------------------------------
// ProtocolHandler Ŭ���� ��������� �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteProtocolCode(const string &protocolFileName, sRmi *rmi
	, const string &pchFileName, const string &marshallingName
)
{
	if (!rmi)
		return true;

	const std::string fileName = common::GetFileNameExceptExt(protocolFileName);
	const std::string path = common::GetFilePathExceptFileName(protocolFileName);
	const std::string folder = (path.empty())? n_SrcFolderName : path + "\\" + n_SrcFolderName;
	_mkdir(folder.c_str());
	n_OrigianlFileName = folder + "\\" + fileName;

	g_protocolName = GetProtocolName(protocolFileName);

	WriteFirstProtocolClassHeader(rmi, marshallingName);
	WriteFirstProtocolCpp(rmi, pchFileName);

	WriteFirstHandlerHeader(rmi, marshallingName);
	WriteFirstHandlerCpp(rmi, pchFileName);

	WriteFirstDataHeader(rmi, marshallingName);
	return true;
}


//------------------------------------------------------------------------
// �����̸��� ��ο� Ȯ���ڸ� ������ ���� �̸�
//------------------------------------------------------------------------
string compiler::GetProtocolName(const string &fileName)
{
	return common::GetFileNameExceptExt(fileName);
}


//------------------------------------------------------------------------
// *.prt �����̸�(protocolName)�� ���������̸�(rmiName) �� �������� 
// �������� Ŭ���� �̸��� �����Ѵ�.
//------------------------------------------------------------------------
string compiler::GetProtocolClassName(const string &protocolName, const string &rmiName )
{
	return rmiName + "_Protocol";
}


//------------------------------------------------------------------------
// *.prt �����̸�(protocolName)�� ���������̸�(rmiName) �� �������� 
// �������ݸ����� Ŭ���� �̸��� �����Ѵ�.
//------------------------------------------------------------------------
string compiler::GetProtocolHandlerClassName(const string &protocolName, const string &rmiName )
{
	return rmiName + "_ProtocolHandler";
}


//------------------------------------------------------------------------
// �������� ������ Ŭ���� �̸��� �����Ѵ�.
//------------------------------------------------------------------------
string compiler::GetProtocolDispatcherClassName(const string &protocolName, const string &rmiName )
{
	return rmiName + "_Dispatcher";
}


//------------------------------------------------------------------------
// Protocol Header ������ �����ϴ� ó���κ��� �ڵ带 �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteFirstProtocolClassHeader(sRmi *rmi, const string &marshallingName)
{
	n_fileName = n_OrigianlFileName + "_Protocol.h";

	std::ofstream fs;
	fs.open(n_fileName.c_str());
	if (!fs.is_open()) return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << common::GetFileName(n_fileName) << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";

	fs << "#pragma once\n";
	fs << endl;
	fs << "namespace " << g_protocolName << " {\n";
	fs << endl;
	fs << "using namespace network2;\n";
	fs << "using namespace " << marshallingName << ";\n";

	WriteProtocolClassHeader(fs, rmi);

	fs << "}\n";
	return true;
}


//------------------------------------------------------------------------
// Protocol ��� �ҽ��ڵ带 �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteProtocolClassHeader(ofstream &fs, sRmi *rmi)
{
	if (!rmi) return true;

	g_className = GetProtocolClassName(g_protocolName, rmi->name );
	g_protocolId = g_className + "_ID";

	fs << "static const int " << g_protocolId << " = " << rmi->number << ";\n";
	fs << endl;
	fs << "class " << g_className <<  " : public network2::iProtocol\n";
	fs << "{" << endl;
	fs << "public:" << endl;
	fs << "\t" << g_className << "() : iProtocol(" << g_protocolId << ") {}\n";
	// print protocol list
	WriteDeclProtocolList( fs, rmi->protocol, false, false, true);
	fs << "};\n";

	return WriteProtocolClassHeader(fs, rmi->next);
}


/**
 @brief WriteFirstDataHeader
 */
bool compiler::WriteFirstDataHeader(sRmi *rmi, const string &marshallingName)
{
	n_fileName = n_OrigianlFileName + "_ProtocolData.h";

	ofstream fs;
	fs.open( n_fileName.c_str());
	if (!fs.is_open()) return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << common::GetFileName(n_fileName) << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";

	fs << "#pragma once\n";
	fs << endl;
	fs << "namespace " << g_protocolName << " {\n";
	fs << endl;
	fs << "using namespace network2;\n";
	fs << "using namespace " << marshallingName << ";\n";

	WriteProtocolDataHeader(fs, rmi);

	fs << "}\n";
	return true;
}


/**
 @brief WriteDataHeader
 */
bool compiler::WriteProtocolDataHeader(ofstream &fs, sRmi *rmi)
{
	if (!rmi) return true;

	g_className = GetProtocolDispatcherClassName(g_protocolName, rmi->name);
	g_protocolId = g_className + "_ID";

	fs << endl;
	fs << endl;

	WriteProtocolData(fs, rmi->protocol );

	fs << endl;
	fs << endl;

	return WriteProtocolDataHeader(fs, rmi->next);
}


/**
 @brief 
 */
bool compiler::WriteProtocolData(ofstream &fs, sProtocol *pProtocol)
{
	if (!pProtocol) return true;

	fs << "\tstruct " << pProtocol->name << "_Packet {" << endl;
	fs << "\t\tcProtocolDispatcher *pdispatcher;\n";
	fs << "\t\tnetid senderId;\n";
	WriteProtocolDataArg(fs, pProtocol->argList );
	fs << "\t};";
	fs << endl;
	fs << endl;

	return WriteProtocolData(fs, pProtocol->next);
}


/**
 @brief 
 */
void compiler::WriteProtocolDataArg(ofstream &fs, sArg *arg)
{
	if (!arg) return;
	fs << "\t\t" << arg->var->type << " " << arg->var->var << ";" << endl;
	WriteProtocolDataArg(fs, arg->next);
}


//------------------------------------------------------------------------
// ProtocolHandler ������Ͽ��� ó�� �� �ּ� �ڵ� �߰�
//------------------------------------------------------------------------
bool compiler::WriteFirstHandlerHeader(sRmi *rmi, const string &marshallingName)
{
	n_fileName = n_OrigianlFileName + "_ProtocolHandler.h";

	ofstream fs;
	fs.open( n_fileName.c_str());
	if (!fs.is_open()) return false;

	fs << "//------------------------------------------------------------------------\n";
	fs << "// Name:    " << common::GetFileName(n_fileName) << endl;
	fs << "// Author:  ProtocolGenerator (by jjuiddong)\n";
	fs << "// Date:    \n";
	fs << "//------------------------------------------------------------------------\n";

	fs << "#pragma once\n";
	fs << endl;
	fs << "#include \"" << g_protocolName << "_ProtocolData.h\"" << endl;
	fs << endl;
	fs << "namespace " << g_protocolName << " {\n";
	fs << endl;
	fs << "using namespace network2;\n";
	fs << "using namespace " << marshallingName << ";\n";

	WriteHandlerHeader(fs, rmi);

	fs << "}\n";
	return true;
}


//------------------------------------------------------------------------
// ProtocolHandler ��� Ŭ���� �ҽ������� �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteHandlerHeader(ofstream &fs, sRmi *rmi)
{
	if (!rmi) return true;

	g_className = GetProtocolDispatcherClassName(g_protocolName, rmi->name);
	g_protocolId = g_className + "_ID";

	fs << "static const int " << g_protocolId << " = " << rmi->number << ";\n";
	fs << endl;

	// Protocol Dispatcher
	fs << "// Protocol Dispatcher\n";
	fs << "class " << g_className << ": public network2::cProtocolDispatcher\n";
	fs << "{\n";
	fs << "public:\n";
	fs << "\t" << g_className << "();\n";
	fs << "protected:\n";
	fs << "\tvirtual bool Dispatch(cPacket &packet, const ProtocolHandlers &handlers) override;\n";
	fs << "};\n";
	fs << "static " << g_className << " g_" << g_protocolName << "_" << g_className << ";\n";// �������� ����
	fs << endl;
	fs << endl;

	// CProtocolHandler class
	g_className = GetProtocolHandlerClassName(g_protocolName, rmi->name);
	string dispatcherClassName = GetProtocolDispatcherClassName(g_protocolName, rmi->name);

	fs << "// ProtocolHandler\n";
	fs << "class " << g_className << " : virtual public network2::iProtocolHandler\n";
	fs << "{\n";
	fs << "\tfriend class " << dispatcherClassName << ";\n";
	WriteDeclProtocolList( fs, rmi->protocol, true, true, false);
	fs << "};\n";
	fs << endl;
	fs << endl;

	return WriteHandlerHeader(fs, rmi->next);
}


//------------------------------------------------------------------------
// 
//------------------------------------------------------------------------
bool compiler::WriteFirstHandlerCpp(sRmi *rmi, const string &pchFileName)
{
	n_fileName = n_OrigianlFileName + "_ProtocolHandler.cpp";
	string headerFileName = g_protocolName + "_ProtocolHandler.h";

	ofstream fs(n_fileName.c_str());
	if (!fs.is_open())
		return false;

	if (!pchFileName.empty())
		fs << "#include \"" << pchFileName << "\"\n";
	fs << "#include \"" << headerFileName << "\"\n";
	//fs << "#include \"network/Controller/Controller.h\"\n";
	fs << endl;
	//fs << "using namespace network2;\n";
	//fs << "using namespace marshalling;\n";
	fs << "using namespace " << g_protocolName << ";\n";
	fs << endl;

	WriteHandlerCpp(fs, rmi);
	return true;
}


//------------------------------------------------------------------------
// ProtocolHandler Cpp ������ �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteHandlerCpp(ofstream &fs, sRmi *rmi)
{
	if (!rmi) return true;

	g_className = GetProtocolDispatcherClassName(g_protocolName, rmi->name);
	g_protocolId = g_className + "_ID";

	// Dispatcher ������ �ڵ� ����
	//fs << "static " << g_protocolName << "::" << g_className << " g_" << g_protocolName << "_" << g_className << ";\n";// �������� ����
	fs << "\n";
	fs << g_protocolName << "::" << g_className << "::" << g_className << "()\n";
	fs << "\t: cProtocolDispatcher(" << g_protocolName << "::" << g_protocolId << ")\n";
	fs << "{\n";
	fs << "\tcProtocolDispatcher::GetDispatcherMap()->insert({" << g_protocolId << ", this });\n";
	fs << "}\n";
	fs << endl;
	//

	// Dispatcher Ŭ������ Dispatch() �Լ� �ڵ� ����
	WriteProtocolDispatchFunc(fs, rmi);
	fs << endl;
	fs << endl;
	fs << endl;

	return WriteHandlerCpp(fs, rmi->next);
}


//------------------------------------------------------------------------
// ��� Ŭ�������Ͽ� ���� �������� ����Ʈ�� ����Ѵ�.
//------------------------------------------------------------------------
void compiler::WriteDeclProtocolList(ofstream &fs, sProtocol *pProtocol, bool isVirtual, bool isImpl, bool isTarget)
{
	if (!pProtocol) return;
	
	// First Interface
	if (!isImpl)
	{
		fs << "\t";
		if (isVirtual)
			fs << "virtual ";

		if (isImpl)
			fs << "bool "; // Handler header file
		else
			fs << "void "; // protocol header file

		//fs << "bool " << pProtocol->name << "(";
		fs << pProtocol->name << "(";
		WriteFirstArg(fs, pProtocol->argList, isTarget);
		fs << ")";
		if (isImpl)
			fs << "{ return true; }"; // Handler header file
		else
			fs << ";"; // protocol header file
		fs << endl;
	}

	// Second Interface
	if (isVirtual && isImpl)
	{
		fs << "\t";
		fs << "virtual ";
		fs << "bool "; // Handler header file
		fs << pProtocol->name << "(";
		fs << g_protocolName << "::";
		fs << pProtocol->name << "_Packet &packet";
		fs << ") { return true; }"; // Handler header file
		fs << endl;
	}

	WriteDeclProtocolList(fs, pProtocol->next, isVirtual, isImpl, isTarget);
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� ���� �������� ����Ʈ�� ����Ѵ�.
//------------------------------------------------------------------------
void compiler::WriteImplProtocolList(ofstream &fs, sProtocol *pProtocol)
{
	if (!pProtocol) return;
	fs << "//------------------------------------------------------------------------\n";
	fs << "// Protocol: " << pProtocol->name << endl;
	fs << "//------------------------------------------------------------------------\n";
	fs << "void " << g_protocolName << "::" << g_className << "::" << pProtocol->name << "(";
	WriteFirstArg(fs, pProtocol->argList, true);
	fs << ")\n";
	fs << "{\n";
	WriteFirstImpleProtocol(fs, pProtocol, pProtocol->argList);
	fs << "}\n";
	fs << "\n";
	WriteImplProtocolList(fs, pProtocol->next);
}


//------------------------------------------------------------------------
// �������� ���ڸ���Ʈ ���
//------------------------------------------------------------------------
void compiler::WriteFirstArg(ofstream &fs, sArg*p, bool isTarget)
{
	if (isTarget)
	{
		fs <<"netid targetId";
		//fs << ", const SEND_FLAG flag";
	}
	else
	{
		fs << "cProtocolDispatcher &dispatcher, ";
		fs << "netid senderId";
	}

	WriteArg(fs, p, true);
}
void compiler::WriteArg(ofstream &fs, sArg *arg, bool comma)
{
	if (!arg) return;
	if (comma)
		fs << ", ";
	fs << "const " << arg->var->type << " &" << arg->var->var;
	WriteArg(fs, arg->next, true);
}

//------------------------------------------------------------------------
// �������� ���ڰ��� ���� �̸��� ����Ѵ�. (�Լ� ȣ��� ���)
//------------------------------------------------------------------------
void compiler::WriteArgVar(ofstream &fs, sArg *arg, bool comma)
{
	if (!arg) return;
	if (comma)
		fs << ", ";
	fs << arg->var->var;
	WriteArgVar(fs, arg->next, true);
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� �� �������� �Լ��� ó���� ���� �ڵ�
//------------------------------------------------------------------------
void compiler::WriteFirstImpleProtocol(ofstream &fs, sProtocol *pProtocol
	, sArg*p)
{
	fs << "\tcPacket packet(m_node->GetPacketHeader());\n";
	fs << "\tpacket.SetProtocolId( GetId() );\n";
	fs << "\tpacket.SetPacketId( " << pProtocol->packetId << " );\n";
	WriteImpleArg(fs, p);
	fs << "\tpacket.EndPack();\n";
	WriteLastImpleProtocol(fs);
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� �� �������� �ڵ忡�� ��Ŷ�� ���ڰ��� �ִ� �ڵ�
//------------------------------------------------------------------------
void compiler::WriteImpleArg(ofstream &fs, sArg*p)
{
	if (!p) return;
	fs << "\tpacket << " << p->var->var << ";\n";
	fs << "\tAddDelimeter(packet);\n";
	WriteImpleArg(fs, p->next);
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� �� �������� �Լ��� �������� ���� �ڵ�
//------------------------------------------------------------------------
void compiler::WriteLastImpleProtocol(ofstream &fs)
{
	fs << "\tGetNode()->Send(targetId, packet);\n";
}


//------------------------------------------------------------------------
// Protocol Cpp �ڵ带 �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteFirstProtocolCpp(sRmi *rmi, const string &pchFileName)
{
	n_fileName = n_OrigianlFileName + "_Protocol.cpp";
	string headerFileName = g_protocolName + "_Protocol.h";

	ofstream fs(n_fileName.c_str());
	if (!fs.is_open()) return false;

	if (!pchFileName.empty())
		fs << "#include \"" << pchFileName << "\"\n";
	fs << "#include \"" << headerFileName << "\"\n";
	//fs << "using namespace network2;\n";
	//fs << "using namespace marshalling;\n";
	fs << "using namespace " << g_protocolName << ";\n";
	fs << endl;

	WriteProtocolCpp(fs, rmi);

	return true;
}


//------------------------------------------------------------------------
// ProtocolHandler Cpp ���� ����
//------------------------------------------------------------------------
bool compiler::WriteProtocolCpp(ofstream &fs, sRmi *rmi)
{
	if (!rmi) return true;
	
	g_className = GetProtocolClassName(g_protocolName, rmi->name);
	WriteImplProtocolList( fs, rmi->protocol );
	fs << endl;
	fs << endl;

	return WriteProtocolCpp(fs, rmi->next);
}


//------------------------------------------------------------------------
// Handler::Dispatch() �Լ� �ڵ� ����
//------------------------------------------------------------------------
void compiler::WriteProtocolDispatchFunc(ofstream &fs, sRmi *rmi)
{
	g_handlerClassName = GetProtocolHandlerClassName(g_protocolName, rmi->name);

	fs << "//------------------------------------------------------------------------\n";
	fs << "// ��Ŷ�� �������ݿ� ���� �ش��ϴ� �ڵ鷯�� ȣ���Ѵ�.\n";
	fs << "//------------------------------------------------------------------------\n";
	fs << "bool " << g_protocolName << "::" << g_className << "::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)\n", 
	fs << "{\n";
		fs << "\tconst int protocolId = packet.GetProtocolId();\n";
		fs << "\tconst int packetId = packet.GetPacketId();\n";

	if (rmi->protocol)
	{
			fs << "\tswitch (packetId)\n";
			fs << "\t{\n";

			WriteDispatchSwitchCase(fs, rmi->protocol);

			fs << "\tdefault:\n";
				fs << "\t\tassert(0);\n";
				fs << "\t\tbreak;\n";
			fs << "\t}\n";
	}
	fs << "\treturn true;\n";
	fs << "}\n";	
}


//------------------------------------------------------------------------
// Dispatch �Լ�����case �� �ڵ� ����
//------------------------------------------------------------------------
void compiler::WriteDispatchSwitchCase(ofstream &fs, sProtocol *pProtocol)
{
	if (!pProtocol) return;

	fs << "\tcase " << pProtocol->packetId << ":\n";
	fs << "\t\t{\n";

	// call HandlerMatching<T> function 
	fs << "\t\t\tProtocolHandlers prtHandler;\n";
	fs << "\t\t\tif (!HandlerMatching<" << g_handlerClassName << ">(handlers, prtHandler))\n";
	fs << "\t\t\t\treturn false;\n";
	fs << endl;

	// set current packet
	fs << "\t\t\tSetCurrentDispatchPacket( &packet );\n";
	fs << endl;

	fs << "\t\t\t" << pProtocol->name << "_Packet data;\n";
	fs << "\t\t\tdata.pdispatcher = this;\n";
	fs << "\t\t\tdata.senderId = packet.GetSenderId();\n";
	WriteDispatchImpleArg2(fs, pProtocol->argList);
	WriteLastDispatchSwitchCase2(fs, pProtocol);

	fs << "\t\t}\n";
	fs << "\t\tbreak;\n";
	fs << "\n";

	WriteDispatchSwitchCase(fs, pProtocol->next);
}


//------------------------------------------------------------------------
// Dispatch �Լ��� switch case ���� ���� �� ��Ŷ ���ڸ� ������ �ڵ带
// �����Ѵ�.
//------------------------------------------------------------------------
void compiler::WriteDispatchImpleArg(ofstream &fs, sArg*p)
{
	if (!p) return;
	// ���� ����
	fs << "\t\t\t" << p->var->type << " " << p->var->var << ";\n";
	// ��Ŷ���� ����Ÿ ����
	fs << "\t\t\tpacket >> " << p->var->var << ";\n";
	WriteDispatchImpleArg(fs, p->next);
}
void compiler::WriteDispatchImpleArg2(ofstream &fs, sArg*p)
{
	if (!p) return;
	// ���� ����
	//fs << "\t\t\t" << p->var->type << " " << p->var->var << ";\n";
	// ��Ŷ���� ����Ÿ ����
	fs << "\t\t\tpacket >> " << "data." << p->var->var << ";\n";
	WriteDispatchImpleArg2(fs, p->next);
}

//------------------------------------------------------------------------
// Dispatch �Լ��� switch case ���� �������� ���� �ڵ��, �������ݿ� �ش��ϴ�
// �ڵ鷯�� ȣ���ϴ� �ڵ带 �����Ѵ�.
//------------------------------------------------------------------------
void compiler::WriteLastDispatchSwitchCase(ofstream &fs, sProtocol *pProtocol)
{
	fs << "\t\t\tSEND_HANDLER(" << g_handlerClassName << ", prtHandler, " << pProtocol->name << "(*this, packet.GetSenderId()";
	WriteArgVar(fs, pProtocol->argList, true );
	fs << ") );\n";
}
void compiler::WriteLastDispatchSwitchCase2(ofstream &fs, sProtocol *pProtocol)
{
	fs << "\t\t\tSEND_HANDLER(" << g_handlerClassName << ", prtHandler, " << pProtocol->name << "(data));\n";
	//SEND_HANDLER(s2s_ProtocolHandler, prtHandler, ReqMovePlayer(data) );
	//WriteArgVar(fs, pProtocol->argList, true );
	//fs << ") );\n";
}
