
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


	void WriteDeclProtocolList(ofstream &fs, sProtocol *pProtocol
		, bool isVirtual, bool isImpl, bool isTarget, bool isBinary);
	void WriteFirstArg(ofstream &fs, sArg*p, bool isTarget, bool isBinary);
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
	//void WriteDispatchImpleArg(ofstream &fs, sArg*p);
	//void WriteLastDispatchSwitchCase(ofstream &fs, sProtocol *pProtocol);
	void WriteDispatchImpleArg2(ofstream &fs, sArg*p, const string &tab, bool isJson, bool isBinary);
	void WriteLastDispatchSwitchCase2(ofstream &fs, sProtocol *pProtocol, const string &tab);


	// Write Protocol Code
	bool WriteFirstProtocolClassHeader(sRmi *rmi, const string &marshallingName);
	bool WriteProtocolClassHeader(ofstream &fs, sRmi *rmi);
	bool WriteFirstProtocolCpp(sRmi *rmi, const string &pchFileName, bool isBinary);
	bool WriteProtocolCpp(ofstream &fs, sRmi *rmi, bool isBinary);

	void WriteImplProtocolList(ofstream &fs, sProtocol *pProtocol, bool isBinary);
	void WriteFirstImpleProtocol(ofstream &fs, sProtocol *pProtocol, sArg*p);
	void WriteImpleArg(ofstream &fs, sArg*p, const string &tab, bool isJson, bool isBinary);
	void WriteLastImpleProtocol(ofstream &fs, const string &tab);

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
	string g_format; // json format?
}

using namespace compiler;


//------------------------------------------------------------------------
// ProtocolHandler Ŭ���� ��������� �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteProtocolCode(const string &protocolFileName, sRmi *rmi
	, const string &pchFileName
)
{
	if (!rmi)
		return true;

	const std::string fileName = common::GetFileNameExceptExt(protocolFileName);
	const std::string path = common::GetFilePathExceptFileName(protocolFileName);
	const std::string folder = (path.empty())? n_SrcFolderName : path + "\\" + n_SrcFolderName;
	_mkdir(folder.c_str());
	n_OrigianlFileName = folder + "\\" + fileName;

	string marshallingName = "marshalling"; // default binary marshalling
	if (rmi->format == "ascii")
		marshallingName = "marshalling_ascii";
	else if (rmi->format == "json")
		marshallingName = "marshalling_json";

	g_format = (marshallingName == "marshalling_json")? "json" : "";

	g_protocolName = GetProtocolName(protocolFileName);

	WriteFirstProtocolClassHeader(rmi, marshallingName);
	WriteFirstProtocolCpp(rmi, pchFileName, (rmi->format == "json"));

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

	string format = "ePacketFormat::BINARY";
	if (rmi->format == "ascii")
		format = "ePacketFormat::ASCII";
	else if (rmi->format == "json")
		format = "ePacketFormat::JSON";

	fs << "static const int " << g_protocolId << " = " << rmi->number << ";\n";
	fs << endl;
	fs << "class " << g_className <<  " : public network2::iProtocol\n";
	fs << "{" << endl;
	fs << "public:" << endl;
	fs << "\t" << g_className << "() : iProtocol(" << g_protocolId 
		<< ", " << format << ") {}\n";
	// print protocol list
	WriteDeclProtocolList( fs, rmi->protocol, false, false, true, (rmi->format == "json"));
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

	string format = "ePacketFormat::BINARY";
	if (rmi->format == "ascii")
		format = "ePacketFormat::ASCII";
	else if (rmi->format == "json")
		format = "ePacketFormat::JSON";

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
	fs << "public:\n";
	fs << "\tfriend class " << dispatcherClassName << ";\n";
	fs << "\t" << g_className << "() { m_format = " << format << "; }\n";
	WriteDeclProtocolList( fs, rmi->protocol, true, true, false, false);
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
	string format = "ePacketFormat::BINARY";
	if (rmi->format == "ascii")
		format = "ePacketFormat::ASCII";
	else if (rmi->format == "json")
		format = "ePacketFormat::JSON";

	fs << "\n";
	fs << g_protocolName << "::" << g_className << "::" << g_className << "()\n";
	fs << "\t: cProtocolDispatcher(" << g_protocolName << "::" << g_protocolId 
		<< ", " << format << ")\n";
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
void compiler::WriteDeclProtocolList(ofstream &fs, sProtocol *pProtocol
	, bool isVirtual, bool isImpl, bool isTarget, bool isBinary)
{
	if (!pProtocol) return;
	
	if (!isImpl) // protocol header file?
	{
		fs << "\t";
		if (isVirtual)
			fs << "virtual ";

		fs << "void ";

		fs << pProtocol->name << "(";
		WriteFirstArg(fs, pProtocol->argList, isTarget, isBinary);
		fs << ")";
		if (isImpl)
			fs << "{ return true; }"; // Handler header file
		else
			fs << ";"; // protocol header file
		fs << endl;
	}

	// Second Interface
	if (isVirtual && isImpl) // handler header file?
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

	WriteDeclProtocolList(fs, pProtocol->next, isVirtual, isImpl, isTarget, isBinary);
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� ���� �������� ����Ʈ�� ����Ѵ�.
//------------------------------------------------------------------------
void compiler::WriteImplProtocolList(ofstream &fs, sProtocol *pProtocol, bool isBinary)
{
	if (!pProtocol) return;
	fs << "//------------------------------------------------------------------------\n";
	fs << "// Protocol: " << pProtocol->name << endl;
	fs << "//------------------------------------------------------------------------\n";
	fs << "void " << g_protocolName << "::" << g_className << "::" << pProtocol->name << "(";
	WriteFirstArg(fs, pProtocol->argList, true, isBinary);
	fs << ")\n";
	fs << "{\n";
	WriteFirstImpleProtocol(fs, pProtocol, pProtocol->argList);
	fs << "}\n";
	fs << "\n";
	WriteImplProtocolList(fs, pProtocol->next, isBinary);
}


//------------------------------------------------------------------------
// �������� ���ڸ���Ʈ ���
//------------------------------------------------------------------------
void compiler::WriteFirstArg(ofstream &fs, sArg*p, bool isTarget, bool isBinary)
{
	if (isTarget)
	{
		fs <<"netid targetId";

		if (isBinary) {
			fs << ", bool isBinary";
		}
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

	const bool isJson = g_format == "json";
	if (isJson) // json format?
	{
		fs << "\tpacket.SetPacketOption(0x01, (uint)isBinary);\n";
		fs << "\tif (isBinary)\n"; // begin if (isBinary)
		fs << "\t{\n";

		// marshalling binary
		fs << "\t\t// marshaling binary\n";
		WriteImpleArg(fs, p, "\t\t", true, true);
		fs << "\t\tpacket.EndPack();\n";
		WriteLastImpleProtocol(fs, "\t\t");

		fs << "\t}\n";
		fs << "\telse\n"; // else
		fs << "\t{\n";

		// marshalling json
		fs << "\t\t// marshaling json\n";
		fs << "\t\tusing boost::property_tree::ptree;\n";
		fs << "\t\tptree props;\n";
		fs << "\t\ttry {\n";
		WriteImpleArg(fs, p, "\t\t\t", true, false);
		fs << "\t\t\tstringstream ss;\n";
		fs << "\t\t\tboost::property_tree::write_json(ss, props);\n";
		fs << "\t\t\tpacket << ss.str();\n";
		fs << "\t\t\tpacket.EndPack();\n";
		WriteLastImpleProtocol(fs, "\t\t\t");
		fs << "\t\t} catch (...) {\n";
		fs << "\t\t\tdbg::Logp(\"json packet maker error\\n\");\n";
		fs << "\t\t}\n";
		fs << "\t}\n"; // end else if (isBinary)
	}
	else
	{
		// binary or ascii format packet
		WriteImpleArg(fs, p, "\t\t", false, false);
		fs << "\tpacket.EndPack();\n";
		WriteLastImpleProtocol(fs, "\t");
	}
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� �� �������� �ڵ忡�� ��Ŷ�� ���ڰ��� �ִ� �ڵ�
//------------------------------------------------------------------------
void compiler::WriteImpleArg(ofstream &fs, sArg*p, const string &tab, bool isJson, bool isBinary)
{
	if (!p) return;

	// json format?
	if (isJson)
	{
		if (isBinary)
		{
			fs << tab << "marshalling::operator<<(packet, " << p->var->var << ");\n";
		}
		else
		{
			fs << tab << "put(props, \"" << p->var->var << "\", " << p->var->var << ");\n";
		}
	}
	else
	{
		fs << "\tpacket << " << p->var->var << ";\n";
		fs << "\tAddDelimeter(packet);\n";
	}

	WriteImpleArg(fs, p->next, tab, isJson, isBinary);
}


//------------------------------------------------------------------------
// Cpp ���Ͽ� �� �������� �Լ��� �������� ���� �ڵ�
//------------------------------------------------------------------------
void compiler::WriteLastImpleProtocol(ofstream &fs, const string &tab)
{
	fs << tab << "GetNode()->Send(targetId, packet);\n";
}


//------------------------------------------------------------------------
// Protocol Cpp �ڵ带 �����Ѵ�.
//------------------------------------------------------------------------
bool compiler::WriteFirstProtocolCpp(sRmi *rmi, const string &pchFileName, bool isBinary)
{
	n_fileName = n_OrigianlFileName + "_Protocol.cpp";
	string headerFileName = g_protocolName + "_Protocol.h";

	ofstream fs(n_fileName.c_str());
	if (!fs.is_open()) return false;

	if (!pchFileName.empty())
		fs << "#include \"" << pchFileName << "\"\n";
	fs << "#include \"" << headerFileName << "\"\n";

	fs << "using namespace " << g_protocolName << ";\n";
	fs << endl;

	WriteProtocolCpp(fs, rmi, isBinary);

	return true;
}


//------------------------------------------------------------------------
// ProtocolHandler Cpp ���� ����
//------------------------------------------------------------------------
bool compiler::WriteProtocolCpp(ofstream &fs, sRmi *rmi, bool isBinary)
{
	if (!rmi) return true;
	
	g_className = GetProtocolClassName(g_protocolName, rmi->name);
	WriteImplProtocolList( fs, rmi->protocol, isBinary );
	fs << endl;
	fs << endl;

	return WriteProtocolCpp(fs, rmi->next, isBinary);
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

	// json packet parsing object
	string tab = "\t\t\t";
	if (g_format == "json")
	{
		// check option bit, whether it is binary or json string
		fs << tab << "const bool isBinary = packet.GetPacketOption(0x01) > 0;\n";

		fs << tab << "if (isBinary)\n";
		fs << tab << "{\n";
		tab += "\t";

		// binary packet parsing
		fs << tab << "// binary parsing\n";

		fs << tab << pProtocol->name << "_Packet data;\n";
		fs << tab << "data.pdispatcher = this;\n";
		fs << tab << "data.senderId = packet.GetSenderId();\n";
		WriteDispatchImpleArg2(fs, pProtocol->argList, tab, true, true);
		WriteLastDispatchSwitchCase2(fs, pProtocol, tab);

		tab = "\t\t\t";
		fs << tab << "}\n";
		fs << tab << "else\n"; // else
		fs << tab << "{\n";
		tab += "\t";

		// json packet parsing
		fs << tab << "// json format packet parsing using property_tree\n";
		fs << tab << "using boost::property_tree::ptree;\n";
		fs << tab << "ptree root;\n\n";
		fs << tab << "try {\n";
		fs << tab << "\tstring str;\n";
		fs << tab << "\tpacket >> str;\n";
		fs << tab << "\tstringstream ss(str);\n";
		fs << tab << "\t\n";
		fs << tab << "\tboost::property_tree::read_json(ss, root);\n";
		fs << tab << "\tptree &props = root.get_child(\"\");\n\n";
		tab += "\t";

		fs << tab << pProtocol->name << "_Packet data;\n";
		fs << tab << "data.pdispatcher = this;\n";
		fs << tab << "data.senderId = packet.GetSenderId();\n";
		WriteDispatchImpleArg2(fs, pProtocol->argList, tab, true, false);
		WriteLastDispatchSwitchCase2(fs, pProtocol, tab);

		tab = "\t\t\t\t";
		fs << tab << "} catch (...) {\n";
		fs << tab << "\tdbg::Logp(\"json packet parsing error\\n\");\n";
		fs << tab << "}\n";

		fs << "\t\t\t}\n";
	}
	else
	{
		fs << tab << pProtocol->name << "_Packet data;\n";
		fs << tab << "data.pdispatcher = this;\n";
		fs << tab << "data.senderId = packet.GetSenderId();\n";
		WriteDispatchImpleArg2(fs, pProtocol->argList, tab, false, true);
		WriteLastDispatchSwitchCase2(fs, pProtocol, tab);
	}

	fs << "\t\t}\n";
	fs << "\t\tbreak;\n";
	fs << "\n";

	WriteDispatchSwitchCase(fs, pProtocol->next);
}


//------------------------------------------------------------------------
// Dispatch �Լ��� switch case ���� ���� �� ��Ŷ ���ڸ� ������ �ڵ带
// �����Ѵ�.
//------------------------------------------------------------------------
//void compiler::WriteDispatchImpleArg(ofstream &fs, sArg*p)
//{
//	if (!p) return;
//	// ���� ����
//	fs << "\t\t\t" << p->var->type << " " << p->var->var << ";\n";
//	// ��Ŷ���� ����Ÿ ����
//	fs << "\t\t\tpacket >> " << p->var->var << ";\n";
//	WriteDispatchImpleArg(fs, p->next);
//}
void compiler::WriteDispatchImpleArg2(ofstream &fs, sArg*p, const string &tab
	, bool isJson, bool isBinary)
{
	if (!p) return;
	// ��Ŷ���� ����Ÿ ����
	if (isJson)
	{
		if (isBinary)
		{
			fs << tab << "marshalling::operator>>(packet, "
				<< "data." << p->var->var << ");\n";
		}
		else
		{
			fs << tab << "get(props, \"" << p->var->var <<
				"\", data." << p->var->var << ");\n";
		}
	}
	else 
	{
		fs << tab << "packet >> " << "data." << p->var->var << ";\n";
	}	
	WriteDispatchImpleArg2(fs, p->next, tab, isJson, isBinary);
}

//------------------------------------------------------------------------
// Dispatch �Լ��� switch case ���� �������� ���� �ڵ��, �������ݿ� �ش��ϴ�
// �ڵ鷯�� ȣ���ϴ� �ڵ带 �����Ѵ�.
//------------------------------------------------------------------------
//void compiler::WriteLastDispatchSwitchCase(ofstream &fs, sProtocol *pProtocol)
//{
//	fs << "\t\t\tSEND_HANDLER(" << g_handlerClassName << ", prtHandler, " << pProtocol->name << "(*this, packet.GetSenderId()";
//	WriteArgVar(fs, pProtocol->argList, true );
//	fs << ") );\n";
//}
void compiler::WriteLastDispatchSwitchCase2(ofstream &fs, sProtocol *pProtocol, const string &tab)
{
	fs << tab << "SEND_HANDLER(" << g_handlerClassName << ", prtHandler, " << pProtocol->name << "(data));\n";
}
