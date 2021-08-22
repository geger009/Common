#include "pch.h"
#include "remotedbg2_ProtocolHandler.h"

using namespace remotedbg2;


remotedbg2::h2r_Dispatcher::h2r_Dispatcher()
	: cProtocolDispatcher(remotedbg2::h2r_Dispatcher_ID, ePacketFormat::JSON)
{
	cProtocolDispatcher::GetDispatcherMap()->insert({h2r_Dispatcher_ID, this });
}

//------------------------------------------------------------------------
// ��Ŷ�� �������ݿ� ���� �ش��ϴ� �ڵ鷯�� ȣ���Ѵ�.
//------------------------------------------------------------------------
bool remotedbg2::h2r_Dispatcher::Dispatch(cPacket &packet, const ProtocolHandlers &handlers)
{
	const int protocolId = packet.GetProtocolId();
	const int packetId = packet.GetPacketId();
	switch (packetId)
	{
	case 4005257575: // AckUploadIntermediateCode
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckUploadIntermediateCode_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckUploadIntermediateCode(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckUploadIntermediateCode_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckUploadIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1397310616: // AckIntermediateCode
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckIntermediateCode_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.index);
				marshalling::operator>>(packet, data.data);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckIntermediateCode(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckIntermediateCode_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					get(props, "count", data.count);
					get(props, "index", data.index);
					get(props, "data", data.data);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckIntermediateCode(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4148808214: // AckRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckRun_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckRun(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckRun_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3643391279: // AckOneStep
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckOneStep_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckOneStep(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckOneStep_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckOneStep(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1012496086: // AckResumeRun
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckResumeRun_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckResumeRun(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckResumeRun_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckResumeRun(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2129545277: // AckBreak
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckBreak_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreak(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckBreak_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreak(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 2045074648: // AckBreakPoint
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckBreakPoint_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.enable);
				marshalling::operator>>(packet, data.id);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreakPoint(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckBreakPoint_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "enable", data.enable);
					get(props, "id", data.id);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckBreakPoint(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 114435221: // AckStop
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckStop_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStop(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckStop_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStop(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1658444570: // AckInput
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckInput_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckInput(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckInput_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckInput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4225702489: // AckStepDebugType
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckStepDebugType_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.stepDbgType);
				marshalling::operator>>(packet, data.result);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStepDebugType(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckStepDebugType_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "stepDbgType", data.stepDbgType);
					get(props, "result", data.result);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckStepDebugType(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 4206107288: // SyncVMInstruction
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMInstruction_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmIdx);
				marshalling::operator>>(packet, data.indices);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMInstruction(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					SyncVMInstruction_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmIdx", data.vmIdx);
					get(props, "indices", data.indices);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMInstruction(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3001685594: // SyncVMRegister
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMRegister_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmIdx);
				marshalling::operator>>(packet, data.infoType);
				marshalling::operator>>(packet, data.reg);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMRegister(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					SyncVMRegister_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmIdx", data.vmIdx);
					get(props, "infoType", data.infoType);
					get(props, "reg", data.reg);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMRegister(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 3045798844: // SyncVMSymbolTable
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMSymbolTable_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmIdx);
				marshalling::operator>>(packet, data.start);
				marshalling::operator>>(packet, data.count);
				marshalling::operator>>(packet, data.symbol);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMSymbolTable(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					SyncVMSymbolTable_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmIdx", data.vmIdx);
					get(props, "start", data.start);
					get(props, "count", data.count);
					get(props, "symbol", data.symbol);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMSymbolTable(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1348120458: // SyncVMOutput
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				SyncVMOutput_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				marshalling::operator>>(packet, data.itprId);
				marshalling::operator>>(packet, data.vmIdx);
				marshalling::operator>>(packet, data.output);
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMOutput(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					SyncVMOutput_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					get(props, "itprId", data.itprId);
					get(props, "vmIdx", data.vmIdx);
					get(props, "output", data.output);
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, SyncVMOutput(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	case 1133387750: // AckHeartBeat
		{
			ProtocolHandlers prtHandler;
			if (!HandlerMatching<h2r_ProtocolHandler>(handlers, prtHandler))
				return false;

			SetCurrentDispatchPacket( &packet );

			const bool isBinary = packet.GetPacketOption(0x01) > 0;
			if (isBinary)
			{
				// binary parsing
				AckHeartBeat_Packet data;
				data.pdispatcher = this;
				data.senderId = packet.GetSenderId();
				packet.Alignment4(); // set 4byte alignment
				SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckHeartBeat(data));
			}
			else
			{
				// json format packet parsing using property_tree
				using boost::property_tree::ptree;
				ptree root;

				try {
					string str;
					packet >> str;
					stringstream ss(str);
					
					boost::property_tree::read_json(ss, root);
					ptree &props = root.get_child("");

					AckHeartBeat_Packet data;
					data.pdispatcher = this;
					data.senderId = packet.GetSenderId();
					SEND_HANDLER(h2r_ProtocolHandler, prtHandler, AckHeartBeat(data));
				} catch (...) {
					dbg::Logp("json packet parsing error packetid = %lu\n", packetId);
				}
			}
		}
		break;

	default:
		assert(0);
		break;
	}
	return true;
}


