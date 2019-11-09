
#include "stdafx.h"
#include "editmanager.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

using namespace framework;
using namespace vprog;

void BuildNode(cNode* node);


cEditManager::cEditManager()
	: m_editor(nullptr)
	, m_contextNodeId(0)
	, m_contextLinkId(0)
	, m_contextPinId(0)
	, m_isCreateNewNode(false)
	, m_newNodeLinkPin(nullptr)
	, m_newLinkPin(nullptr)
	, m_headerBackground(nullptr)
{
}

cEditManager::~cEditManager()
{
	Clear();

	if (m_editor)
	{
		ed::DestroyEditor(m_editor);
		m_editor = nullptr;
	}
}


// Initialize vProg Edit Manager
bool cEditManager::Init(graphic::cRenderer &renderer)
{
	ed::Config config;
	m_editor = ed::CreateEditor(&config);

	m_headerBackground = graphic::cResourceManager::Get()->LoadTexture(renderer
		, "BlueprintBackground.png");

	return true;
}


bool cEditManager::Read(const StrPath &fileName)
{
	Clear();

	m_fileName = fileName.GetFullFileName();
	ed::SetCurrentEditor(m_editor);

	cNodeFile nodeFile;
	if (!nodeFile.Read(fileName))
		return false;
	m_nodes = nodeFile.m_nodes;
	m_symbTable = nodeFile.m_symbTable;
	BuildNodes();

	// update node position and symboltable
	for (auto &node : m_nodes)
	{
		if (node.m_varName.empty())
			node.m_varName = node.m_name;
		ed::SetNodePosition(node.m_id, node.m_pos);

		// add variable type to symboltable
		if (eNodeType::Variable == node.m_type)
		{
			for (auto &pin : node.m_outputs)
			{
				cSymbolTable::sValue *value = m_symbTable.FindSymbol(pin.id);
				if (!value)
					m_symbTable.AddSymbolStr(pin, "");
			}
		}
	}

	// link id 가 중복될 수 있기 때문에, 새로 생성해야 한다.
	for (auto &link : nodeFile.m_links)
		AddLink(link.fromId, link.toId);

	return true;
}


bool cEditManager::Write(const StrPath &fileName)
{
	if (m_nodes.empty())
		return false; // empty data

	cNodeFile nodeFile;
	nodeFile.m_nodes = m_nodes;
	nodeFile.m_links = m_links;
	nodeFile.m_symbTable = m_symbTable;
	for (auto &node : nodeFile.m_nodes)
	{
		node.m_pos = ed::GetNodePosition(node.m_id);
		node.m_size = ed::GetNodeSize(node.m_id);
	}
	return nodeFile.Write(fileName);
}


// render vprog node
bool cEditManager::Render(graphic::cRenderer &renderer)
{
	namespace util = ax::NodeEditor::Utilities;

	ed::SetCurrentEditor(m_editor);
	
	ed::Begin("My Editor");
	{
		const int bgWidth = m_headerBackground->m_imageInfo.Width;
		const int bgHeight = m_headerBackground->m_imageInfo.Height;
		util::BlueprintNodeBuilder builder(m_headerBackground->m_texSRV, bgWidth, bgHeight);

		// node rendering
		for (auto& node : m_nodes)
			node.Render(*this, builder, m_newLinkPin);

		// setup link
		for (auto& link : m_links)
			ed::Link(link.id, link.fromId, link.toId, link.color, 2.0f);

		// new/delete node, link operation
		EditOperation();
	} // ~ed::Begin("My Editor");

	// render popup
	RenderPopup(renderer);

	ed::End();

	return true;
}


// new/delete node, link operation
bool cEditManager::EditOperation()
{
	if (m_isCreateNewNode)
		return true;

	if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
	{
		auto showLabel = [](const char* label, ImColor color)
		{
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
			auto size = ImGui::CalcTextSize(label);

			auto padding = ImGui::GetStyle().FramePadding;
			auto spacing = ImGui::GetStyle().ItemSpacing;

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

			auto rectMin = ImGui::GetCursorScreenPos() - padding;
			auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

			auto drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
			ImGui::TextUnformatted(label);
		};

		ed::PinId fromPinId = 0, toPinId = 0;
		if (ed::QueryNewLink(&fromPinId, &toPinId))
		{
			auto startPin = FindPin(fromPinId);
			auto endPin = FindPin(toPinId);

			m_newLinkPin = startPin ? startPin : endPin;

			if (startPin->kind == ePinKind::Input)
			{
				std::swap(startPin, endPin);
				std::swap(fromPinId, toPinId);
			}

			if (startPin && endPin)
			{
				if (endPin == startPin)
				{
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				else if (endPin->kind == startPin->kind)
				{
					showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
				}
				//else if (endPin->Node == startPin->Node)
				//{
				//    showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
				//    ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
				//}
				else if (endPin->type != startPin->type)
				{
					showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
					ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
				}
				else
				{
					showLabel("+ Create Link", ImColor(32, 45, 32, 180));
					if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
					{
						m_links.emplace_back(vprog::sLink(GetUniqueId(), fromPinId, toPinId));
						m_links.back().color = GetIconColor(startPin->type);
					}
				}
			}
		}

		ed::PinId pinId = 0;
		if (ed::QueryNewNode(&pinId))
		{
			m_newLinkPin = FindPin(pinId);
			if (m_newLinkPin)
				showLabel("+ Create Node", ImColor(32, 45, 32, 180));

			if (ed::AcceptNewItem())
			{
				m_isCreateNewNode = true;
				m_newNodeLinkPin = FindPin(pinId);
				m_newLinkPin = nullptr;
				ed::Suspend();
				ImGui::OpenPopup("Create New Node");
				ed::Resume();
			}
		}
	}
	else
	{
		m_newLinkPin = nullptr;
	}

	ed::EndCreate();


	if (ed::BeginDelete())
	{
		ed::LinkId linkId = 0;
		while (ed::QueryDeletedLink(&linkId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto id = std::find_if(m_links.begin()
					, m_links.end()
					, [linkId](auto& link) { return link.id == linkId; });
				if (id != m_links.end())
					m_links.erase(id);
			}
		}

		ed::NodeId nodeId = 0;
		while (ed::QueryDeletedNode(&nodeId))
		{
			if (ed::AcceptDeletedItem())
			{
				auto id = std::find_if(m_nodes.begin()
					, m_nodes.end(
					), [nodeId](auto& node) { return node.m_id == nodeId; });
				if (id != m_nodes.end())
					m_nodes.erase(id);
			}
		}
	}
	ed::EndDelete();

	return true;
}


bool cEditManager::RenderPopup(graphic::cRenderer &renderer)
{
	auto openPopupPosition = ImGui::GetMousePos();


	ed::Suspend();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("Create New Node"))
	{
		auto newNodePostion = openPopupPosition;
		//ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

		//auto drawList = ImGui::GetWindowDrawList();
		//drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f, 0xFFFF00FF);

		vprog::cNode* node = nullptr;
		if (ImGui::MenuItem("Input Action"))
			node = Generate_ReservedDefinition("InputAction Fire");
		if (ImGui::MenuItem("Output Action"))
			node = Generate_ReservedDefinition("OutputAction");
		if (ImGui::MenuItem("Branch"))
			node = Generate_ReservedDefinition("Branch");
		//if (ImGui::MenuItem("Do N"))
		//	node = Generate_DoNNode();
		if (ImGui::MenuItem("Set Timer"))
			node = Generate_ReservedDefinition("Set Timer");
		if (ImGui::MenuItem("Less"))
			node = Generate_ReservedDefinition("<");
		//if (ImGui::MenuItem("Weird"))
		//	node = Generate_WeirdNode();
		//if (ImGui::MenuItem("Trace by Channel"))
		//	node = Generate_TraceByChannelNode();
		if (ImGui::MenuItem("Print String"))
			node = Generate_ReservedDefinition("Print String");
		//ImGui::Separator();
		//if (ImGui::MenuItem("Comment"))
		//	node = Generate_Comment();
		//ImGui::Separator();
		//if (ImGui::MenuItem("Sequence"))
		//	node = Generate_TreeSequenceNode();
		//if (ImGui::MenuItem("Move To"))
		//	node = Generate_TreeTaskNode();
		//if (ImGui::MenuItem("Random Wait"))
		//	node = Generate_TreeTask2Node();
		//ImGui::Separator();
		//if (ImGui::MenuItem("Message"))
		//	node = Generate_MessageNode();

		if (node)
		{
			m_isCreateNewNode = false;

			ed::SetNodePosition(node->m_id, newNodePostion);

			if (auto startPin = m_newNodeLinkPin)
			{
				auto& pins = startPin->kind == vprog::ePinKind::Input ?
					node->m_outputs : node->m_inputs;

				for (auto& pin : pins)
				{
					if (CanCreateLink(startPin, &pin))
					{
						auto endPin = &pin;
						if (startPin->kind == ePinKind::Input)
							std::swap(startPin, endPin);

						m_links.emplace_back(vprog::sLink(GetUniqueId(), startPin->id, endPin->id));
						m_links.back().color = GetIconColor(startPin->type);

						break;
					}
				}
			}
		}

		ImGui::EndPopup();
	}
	else
	{
		m_isCreateNewNode = false;
	}

	ImGui::PopStyleVar();
	ed::Resume();

	return true;
}


cNode* cEditManager::FindNode(const ed::NodeId id)
{
	for (auto& node : m_nodes)
		if (node.m_id == id)
			return &node;

	return nullptr;
}


sLink* cEditManager::FindLink(const ed::LinkId id)
{
	for (auto& link : m_links)
		if (link.id == id)
			return &link;

	return nullptr;
}


sLink* cEditManager::FindLink(const ed::PinId from, const ed::PinId to)
{
	for (auto& link : m_links)
		if ((from == link.fromId)
			&& (to == link.toId))
			return &link;
	return NULL;
}


sPin* cEditManager::FindPin(const ed::PinId id)
{
	if (!id)
		return nullptr;

	for (auto& node : m_nodes)
	{
		for (auto& pin : node.m_inputs)
			if (pin.id == id)
				return &pin;

		for (auto& pin : node.m_outputs)
			if (pin.id == id)
				return &pin;
	}

	return nullptr;
}

// find contain pin node
cNode* cEditManager::FindContainNode(const ed::PinId id)
{
	if (!id)
		return nullptr;

	for (auto& node : m_nodes)
	{
		for (auto& pin : node.m_inputs)
			if (pin.id == id)
				return &node;

		for (auto& pin : node.m_outputs)
			if (pin.id == id)
				return &node;
	}

	return nullptr;
}


bool cEditManager::IsPinLinked(const ed::PinId id)
{
	if (!id)
		return false;

	for (auto& link : m_links)
		if (link.fromId == id || link.toId == id)
			return true;

	return false;
}


void BuildNode(cNode* node)
{
	for (auto& input : node->m_inputs)
	{
		input.node = node;
		input.kind = ePinKind::Input;
	}

	for (auto& output : node->m_outputs)
	{
		output.node = node;
		output.kind = ePinKind::Output;
	}
}


void cEditManager::BuildNodes()
{
	for (auto& node : m_nodes)
		BuildNode(&node);
}


bool cEditManager::ShowFlow(const ed::LinkId id)
{
	for (auto &link : m_links)
		if (id == link.id)
			ed::Flow(link.id);

	return true;
}


// generate reserved definition node
// from ReadDefinitionFile()
cNode* cEditManager::Generate_ReservedDefinition(const StrId &name
	, const StrId &varName //= ""
)
{
	cNode *defNode = NULL;
	for (auto &def : m_definitions)
	{
		if ((def.m_name == name)
			&& (varName.empty()
				|| (!varName.empty() 
					&& (varName == def.m_varName))))
		{
			defNode = &def;
			break;
		}
	}
	if (!defNode)
		return NULL;

	cNode newNode = *defNode;
	newNode.m_varName = defNode->m_name;

	// generate unique id
	newNode.m_id = GetUniqueId();
	for (auto &pin : newNode.m_inputs)
		pin.id = GetUniqueId();
	for (auto &pin : newNode.m_outputs)
		pin.id = GetUniqueId();

	// add symbol table when variable type
	if (eNodeType::Variable == newNode.m_type)
	{
		for (auto &pin : newNode.m_outputs)
			m_symbTable.AddSymbol(pin, "");
	}

	newNode.m_pos = ImVec2(0, 0);
	m_nodes.push_back(newNode);

	BuildNode(&m_nodes.back());
	return &m_nodes.back();
}


bool cEditManager::AddLink(const ed::PinId from, const ed::PinId to)
{
	sPin *frPin = FindPin(from);
	sPin *toPin = FindPin(to);
	if (!frPin || !toPin)
		return false;

	m_links.push_back(sLink(ed::LinkId(GetUniqueId()), from, to));
	m_links.back().color = GetIconColor(frPin->type);
	return true;
}


// read function definition file
// format: vProg node file
// add to reserved node data
bool cEditManager::ReadDefinitionFile(const StrPath &fileName)
{
	cNodeFile nodeFile;
	if (!nodeFile.Read(fileName))
		return false;
	m_definitions = nodeFile.m_nodes;
	return true;
}


// 중복되지 않은 id를 리턴한다.
// node, pin, link와 비교해서 같은 id가 없을 때만 리턴한다.
int cEditManager::GetUniqueId()
{
	while (1)
	{
		const int id = common::GenerateId();
		const ed::NodeId nid = id;
		const ed::PinId pid = id;
		const ed::LinkId lid = id;
		
		// 중복 검사
		for (auto &node : m_nodes)
		{
			if (node.m_id == nid)
				goto $same;
			for (auto &pin : node.m_inputs)
				if (pin.id == pid)
					goto $same;
			for (auto &pin : node.m_outputs)
				if (pin.id == pid)
					goto $same;
		}
		for (auto &link : m_links)
			if (link.id == lid)
				goto $same;
		return id;

	$same:
		continue; // continue generate unique id
	}	
	return 0;
}


bool cEditManager::IsLoad()
{
	return !m_nodes.empty();
}


void cEditManager::Clear()
{
	ed::SetCurrentEditor(m_editor);
	ed::ClearEditor();
	m_nodes.clear();
	m_links.clear();
	m_symbTable.Clear();
}
