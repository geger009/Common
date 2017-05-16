
#include "../stdafx.h"
#include "dockwindow.h"
#include "renderwindow.h"
#include "dockmanager.h"
#include "../external/imgui/imgui_internal.h"


using namespace graphic;
using namespace framework;

int cDockWindow::s_id = 1;


eDockType::Enum eDockType::GetOpposite(const Enum &type)
{
	switch (type)
	{
	case TAB: return TAB;
	case LEFT: return RIGHT;
	case TOP:  return BOTTOM;
	case RIGHT: return LEFT;
	case BOTTOM: return TOP;
	case LEFT_MOST: return RIGHT_MOST;
	case TOP_MOST: return BOTTOM_MOST;
	case RIGHT_MOST: return LEFT_MOST;
	case BOTTOM_MOST: return TOP_MOST;
	default: assert(0);
	}
	return TAB;
}

bool eDockType::IsOpposite(const Enum &type1, const Enum &type2)
{
	return (GetOpposite(type1) == type2);
}



cDockWindow::cDockWindow(const string &name //=""
)
	: m_state(eDockState::DOCK)
	, m_name(name)
	, m_isBind(false)
	, m_dockType(eDockType::TOP)
	, m_owner(NULL)
	, m_lower(NULL)
	, m_upper(NULL)
	, m_parent(NULL)
	, m_rect(0,0,0,0)
	, m_selectTab(0)
	, m_dragSlot(eDockType::NONE)
{
	//m_name = format("dock%d", s_id++);
}

cDockWindow::~cDockWindow()
{
	// delete all docking window
}


bool cDockWindow::Create(const eDockState::Enum state, const eDockType::Enum type,
	cRenderWindow *owner, cDockWindow *parent
	, const float windowSize//=0.5f
)
{
	m_state = state;
	m_dockType = type;
	m_owner = owner;
	m_parent = parent;

	if (owner)
	{
		if (!owner->m_dock)
		{
			owner->m_dock = this;

			const sf::Vector2u size = m_owner->getSize();
			m_rect = sRectf(0, TITLEBAR_HEIGHT2, (float)size.x, (float)size.y);
		}
	}

	if (parent)
	{
		if (state == eDockState::DOCK)
			parent->Dock(type, this, windowSize);
	}

	return true;
}


bool cDockWindow::Dock(const eDockType::Enum type
	, cDockWindow *child
	, const float windowSize //= 0.5f
)
{
	if (this == child)
		return false;

	if (eDockType::TAB == type)
	{
		child->m_owner = m_owner;
		child->m_parent = this;
		child->m_dockType = m_dockType;
		child->m_rect = m_rect;
		m_tabs.push_back(child);

		ResizeEnd(eDockResize::DOCK_WINDOW, m_rect);
		return true;
	}

	cDockWindow *dock = cDockManager::Get()->NewDockWindow();
	dock->m_state = eDockState::VIRTUAL;
	dock->m_dockType = m_dockType;
	dock->m_rect = m_rect;
	dock->m_owner = m_owner;

	m_dockType = eDockType::GetOpposite(type);
	switch (type)
	{
	case eDockType::LEFT:
	case eDockType::TOP:
		dock->m_lower = child;
		dock->m_upper = this;
		break;

	case eDockType::RIGHT:
	case eDockType::BOTTOM:
		dock->m_lower = this;
		dock->m_upper = child;
		break;

	default: assert(0);
	}

	SetParentDockPtr(dock);

	m_parent = dock;
	child->m_owner = m_owner;
	child->m_parent = dock;
	child->m_dockType = type;
	CalcWindowSize(child, windowSize);

	return true;
}


bool cDockWindow::Undock(const bool newWindow) // = true
{
	if (m_parent)
	{
		m_parent->Undock(this);
	}
	else
	{
		if (cDockWindow *showWnd = UndockTab())
		{
			SetParentDockPtr(showWnd);
		}
		else
		{
			SetParentDockPtr(NULL);
			cDockManager::Get()->DeleteRenderWindow(m_owner);
		}
	}

	if (newWindow)
	{
		const int width = 800;
		const int height = 600;
		cImGui *oldGui = &m_owner->m_gui;
		cRenderWindow *window = cDockManager::Get()->NewRenderWindow(m_name, width, height, m_owner->m_sharedRenderer);
		window->m_dock = this;
		m_parent = NULL;
		m_owner = window;
		CalcResizeWindow(eDockResize::RENDER_WINDOW, sRectf(0, 0, (float)width, (float)height));
		oldGui->SetContext();
		window->requestFocus();
		window->SetDragState();
		window->RequestResetDeviceNextFrame();
	}
	else
	{
		m_parent = NULL;
		m_owner = NULL;
	}

	return true;
}


// Undock in Child Window dock ptr
bool cDockWindow::Undock(cDockWindow *udock)
{
	RETV(this == udock, false);

	cDockWindow *rmWnd = NULL;
	cDockWindow *showWnd = NULL;

	if (m_lower == udock)
	{
		rmWnd = m_lower;
		showWnd = m_upper;
	}
	else if (m_upper == udock)
	{
		rmWnd = m_upper;
		showWnd = m_lower;
	}
	else
	{
		if (RemoveTab(udock))
			return true;

		assert(0);
		return false;
	}

	if (rmWnd->m_tabs.empty())
	{
		if (m_lower == udock)
		{
			rmWnd = m_lower;
			showWnd = m_upper;
		}
		else if (m_upper == udock)
		{
			rmWnd = m_upper;
			showWnd = m_lower;
		}
		else
		{
			assert(0);
		}

		udock->Merge(showWnd);
		showWnd->m_dockType = m_dockType;
		SetParentDockPtr(showWnd);
		showWnd->OnResizeEnd(eDockResize::DOCK_WINDOW, showWnd->m_rect);
		cDockManager::Get()->DeleteDockWindow(this);
	}
	else
	{
		showWnd = rmWnd->UndockTab();

		if (m_lower == udock)
		{
			m_lower = showWnd;
		}
		else if (m_upper == udock)
		{
			m_upper = showWnd;
		}
	}

	//// todo : remove rmWnd class
	//rmWnd->m_lower = NULL;
	//rmWnd->m_upper = NULL;
	//rmWnd->m_tabs.clear();

	return true;
}


// remove this DockWindow from Tab
// return Show DockWindow
cDockWindow* cDockWindow::UndockTab()
{
	RETV(m_tabs.empty(), NULL);

	cDockWindow *showWnd = m_tabs[0];
	common::popvector(m_tabs, 0);

	showWnd->m_parent = m_parent;
	showWnd->m_dockType = m_dockType;
	showWnd->m_lower = m_lower;
	showWnd->m_upper = m_upper;
	showWnd->m_tabs = m_tabs;
	for (auto p : showWnd->m_tabs)
		p->m_parent = showWnd;	
	m_tabs.clear();

	return showWnd;
}


// Merge Current Space and Dock Space
// and then, Remove Current Dock Window
bool cDockWindow::Merge(cDockWindow *dock)
{
	sRectf rect;
	if (((m_dockType == eDockType::LEFT) && (dock->m_dockType == eDockType::RIGHT))
		|| ((m_dockType == eDockType::RIGHT) && (dock->m_dockType == eDockType::LEFT)))
	{
		rect = sRectf(min(m_rect.left, dock->m_rect.left)
			, m_rect.top
			, max(m_rect.right, dock->m_rect.right)
			, m_rect.bottom);
	}
	else if (((m_dockType == eDockType::TOP) && (dock->m_dockType == eDockType::BOTTOM))
		|| ((m_dockType == eDockType::BOTTOM) && (dock->m_dockType == eDockType::TOP)))
	{
		rect = sRectf(m_rect.left
			, min(m_rect.top, dock->m_rect.top)
			, m_rect.right
			, max(m_rect.bottom, dock->m_rect.bottom));
	}
	else
	{
		assert(0);
		return false;
	}

	dock->CalcResizeWindow(eDockResize::DOCK_WINDOW, rect);
	return true;
}


// Replace Parent DockWindow Pointer
// return previous pointer
void cDockWindow::SetParentDockPtr(cDockWindow *dock)
{
	if (m_parent)
	{
		if (m_parent->m_lower == this)
			m_parent->m_lower = dock;
		else if (m_parent->m_upper == this)
			m_parent->m_upper = dock;
		else
			assert(0);

		if (dock)
			dock->m_parent = m_parent;
	}
	else if (m_owner)
	{
		if (m_owner->m_dock == this)
			m_owner->m_dock = dock;
		else
			assert(0);

		if (dock)
			dock->m_parent = NULL;
	}
	else
	{
		assert(0);
	}
}


using namespace ImGui;
ImRect getSlotRect(ImRect parent_rect, eDockType::Enum dock_slot)
{
	ImVec2 size = parent_rect.Max - parent_rect.Min;
	ImVec2 center = parent_rect.Min + size * 0.5f;
	switch (dock_slot)
	{
	default: return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
	case eDockType::TOP: return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
	case eDockType::RIGHT: return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
	case eDockType::BOTTOM: return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
	case eDockType::LEFT: return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
	}
}


eDockType::Enum cDockWindow::render_dock_slot_preview(const ImVec2& mouse_pos, const ImVec2& cPos, const ImVec2& cSize)
{
	eDockType::Enum dock_slot = eDockType::NONE;

	ImRect rect{ cPos, cPos + cSize };

	auto checkSlot = [&mouse_pos](ImRect rect, eDockType::Enum slot) -> bool
	{
		auto slotRect = getSlotRect(rect, slot);
		ImGui::GetWindowDrawList()->AddRectFilled(
			slotRect.Min,
			slotRect.Max,
			ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.8f)));//tab
		return slotRect.Contains(mouse_pos);
	};

	if (checkSlot(rect, eDockType::TAB))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockType::TAB;
	}

	if (checkSlot(rect, eDockType::LEFT))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockType::LEFT;
	}

	if (checkSlot(rect, eDockType::RIGHT))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x + (cSize.x / 2.0f), cPos.y), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockType::RIGHT;
	}

	if (checkSlot(rect, eDockType::TOP))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(cPos, ImVec2(cPos.x + cSize.x, cPos.y + (cSize.y / 2.0f)), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockType::TOP;
	}

	if (checkSlot(rect, eDockType::BOTTOM))
	{
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cPos.x, cPos.y + (cSize.y / 2.0f)), ImVec2(cPos.x + cSize.x, cPos.y + cSize.y), ImGui::ColorConvertFloat4ToU32(ImVec4(0.4f, 0.4f, 0.4f, 0.5f)));//tab
		dock_slot = eDockType::BOTTOM;
	}

	return dock_slot;
}


void cDockWindow::RenderDock(const Vector2 &pos //=ImVec2(0,0)
	) 
{
	if (m_lower)
	{
		m_lower->RenderDock();
	}
	else
	{
		Vector2 npos = pos + Vector2(m_rect.left, m_rect.top);
		ImGui::SetCursorPos(ImVec2(pos.x + m_rect.left, pos.y + m_rect.top));
		RenderTab();
		npos.y += 32;
		ImGui::SetCursorPos(ImVec2(npos.x, npos.y));

		ImGui::BeginChild(m_name.c_str(),
			ImVec2((float)m_rect.Width()-3, (float)m_rect.Height() - 35), false,
			ImGuiWindowFlags_AlwaysUseWindowPadding);
		if (m_selectTab == 0)
			OnRender();
		else if ((int)m_tabs.size() > m_selectTab-1)
			m_tabs[m_selectTab-1]->OnRender();

		if (m_isBind)
		{
			const ImVec2 size(m_rect.Width(), m_rect.Height());
			const ImVec2 screen_cursor_pos = ImVec2(npos.x, npos.y-32);
			const auto mouse_pos = sf::Mouse::getPosition(*m_owner);
			const ImVec2 cursor_pos = { float(mouse_pos.x), float(mouse_pos.y) };
			if ((mouse_pos.x > screen_cursor_pos.x && mouse_pos.x < (screen_cursor_pos.x + size.x)) &&
				(mouse_pos.y > screen_cursor_pos.y && mouse_pos.y < (screen_cursor_pos.y + size.y)))
			{
				// todo: cliprect bugfix
				ImGui::SetScrollY(ImGui::GetScrollY() + ImGui::GetWindowSize().y);

				ImGui::BeginChild("##dockSlotPreview");
				ImGui::PushClipRect(ImVec2(), ImGui::GetIO().DisplaySize, false);
				m_dragSlot = render_dock_slot_preview(cursor_pos, screen_cursor_pos, ImVec2(m_rect.Width(), m_rect.Height()));
				ImGui::PopClipRect();
				ImGui::EndChild();

				if (m_dragSlot != eDockType::NONE)
				{
					cDockManager::Get()->SetDragTarget(this);
				}
			}
		}

		ImGui::EndChild();
	}

	if (m_upper)
	{
		m_upper->RenderDock( pos );
	}
}


void cDockWindow::RenderTab()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14.0f, 3.0f));
	ImVec4 childBg = ImGui::GetStyle().Colors[ImGuiCol_ChildWindowBg];

	for (u_int i = 0; i < m_tabs.size()+1; ++i)
	{
		if (i == m_selectTab)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, childBg);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, childBg);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, childBg);
		}
		else
		{
			ImVec4 button_color = ImVec4(childBg.x - 0.04f, childBg.y - 0.04f, childBg.z - 0.04f, childBg.w);
			ImVec4 buttonColorActive = ImVec4(childBg.x + 0.10f, childBg.y + 0.10f, childBg.z + 0.10f, childBg.w);
			ImVec4 buttonColorHovered = ImVec4(childBg.x + 0.15f, childBg.y + 0.15f, childBg.z + 0.15f, childBg.w);
			ImGui::PushStyleColor(ImGuiCol_Button, button_color);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColorActive);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColorHovered);
		}

		cDockWindow *dock = (i == 0) ? this : m_tabs[i-1];
		if (ImGui::Button(dock->m_name.c_str(), ImVec2(0, 32)))
		{
			m_selectTab = i;
		}

		if (ImGui::IsItemActive() && 
			!cDockManager::Get()->IsDragState() && !cDockManager::Get()->IsMoveState())
		{
			const float delta = m_owner->m_clickPos.y - ImGui::GetIO().MousePos.y;
			if (!m_parent && !m_upper && !m_lower && m_tabs.empty())
			{ // SingleTab DockWindow
				if (delta < -2 || delta > 2)
				{
					const bool IsMainWindow = cDockManager::Get()->m_mainWindow == m_owner;
					if (!IsMainWindow)
						m_owner->SetDragState();
				}
			}
			else if (delta < -4 || delta > 27)
			{ // MultiTab DockWindow
				if (i == 0)
				{
					Undock();
				}
				else
				{
					m_tabs[i - 1]->Undock();
				}
			}
		}

		ImGui::SameLine();
		ImGui::PopStyleColor(3);
	}

	ImGui::PopStyleVar();
}


void cDockWindow::PreRender()
{
	if (m_lower)
	{
		m_lower->PreRender();
	}
	else
	{
		if (m_selectTab == 0)
			OnPreRender();
		else if ((int)m_tabs.size() > m_selectTab - 1)
			m_tabs[m_selectTab - 1]->PreRender();
	}

	if (m_upper)
	{
		m_upper->PreRender();
	}
}


void cDockWindow::Update(const float deltaSeconds)
{
	OnUpdate(deltaSeconds);

	if (m_lower)
		m_lower->Update(deltaSeconds);
	for (auto &p : m_tabs)
		p->Update(deltaSeconds);
	if (m_upper)
		m_upper->Update(deltaSeconds);
}


void cDockWindow::CalcWindowSize(cDockWindow *dock
	, const float windowSize //= 0.5f
)
{
	RET(!dock);

	const sf::Vector2f size(m_rect.Width(), m_rect.Height());
	const float x = m_rect.left;
	const float y = m_rect.top;
	sRectf rect1, rect2;

	switch (dock->m_dockType)
	{
	case 	eDockType::TAB:
		rect1 = sRectf(x, y, x + size.x, y + size.y);
		rect2 = sRectf(x, y, x + size.x, y + size.y);
		break;

	case eDockType::LEFT:
		rect1 = sRectf(x + size.x *windowSize, y, x+size.x, y+size.y);
		rect2 = sRectf(x, y, x+size.x*windowSize, y+size.y);
		break;

	case eDockType::RIGHT:
		rect1 = sRectf(x, y, x + size.x *(1-windowSize), y + size.y);
		rect2 = sRectf(x + size.x *(1-windowSize), y, x + size.x, y + size.y);
		break;

	case eDockType::TOP:
		rect1 = sRectf(x, y + size.y *windowSize, x + size.x, y + size.y);
		rect2 = sRectf(x, y, x + size.x, y + size.y *windowSize);
		break;

	case eDockType::BOTTOM:
		rect1 = sRectf(x, y, x + size.x, y + size.y *(1-windowSize));
		rect2 = sRectf(x, y + size.y *(1-windowSize), x + size.x, y + size.y);
		break;

	case eDockType::LEFT_MOST:
	case eDockType::TOP_MOST:
	case eDockType::RIGHT_MOST:
	case eDockType::BOTTOM_MOST:
		break;
	}

	CalcResizeWindow(eDockResize::DOCK_WINDOW, rect1);
	dock->CalcResizeWindow(eDockResize::DOCK_WINDOW, rect2);

	OnResizeEnd(eDockResize::DOCK_WINDOW, rect1);
	dock->OnResizeEnd(eDockResize::DOCK_WINDOW, rect1);
}


// Update Window size, from already setting width/height rate
void cDockWindow::CalcResizeWindow(const eDockResize::Enum type, const sRectf &rect)
{
	if (m_rect == rect)
		return;

	const float w = m_rect.Width();
	const float h = m_rect.Height();
	const float nw = rect.Width();
	const float nh = rect.Height();
	Vector4 v0(rect.left, rect.top, rect.left, rect.top);
	Vector4 v1(m_rect.left, m_rect.top, m_rect.left, m_rect.top);

	if (m_lower)
	{
		cDockWindow *p = m_lower;
		Vector4 v2(p->m_rect.left, p->m_rect.top, p->m_rect.right, p->m_rect.bottom);
		Vector4 v3 = v2 - v1;
		v3.x = (v3.x / w) * nw; // left
		v3.y = (v3.y / h) * nh; // top
		v3.z = (v3.z / w) * nw; // right
		v3.w = (v3.w / h) * nh; // bottom
		Vector4 v4 = v3 + v0;
		sRectf nr(v4.x, v4.y, v4.z, v4.w);
		m_lower->CalcResizeWindow(type, nr);
	}

	if (m_upper)
	{
		cDockWindow *p = m_upper;
		Vector4 v2(p->m_rect.left, p->m_rect.top, p->m_rect.right, p->m_rect.bottom);
		Vector4 v3 = v2 - v1;
		v3.x = (v3.x / w) * nw; // left
		v3.y = (v3.y / h) * nh; // top
		v3.z = (v3.z / w) * nw; // right
		v3.w = (v3.w / h) * nh; // bottom
		Vector4 v4 = v3 + v0;
		sRectf nr(v4.x, v4.y, v4.z, v4.w);
		m_upper->CalcResizeWindow(type, nr);
	}

	m_rect = rect;

	for (auto &p : m_tabs)
		p->CalcResizeWindow(type, rect);
		//p->m_rect = rect;

	OnResize(type, rect);
}


void cDockWindow::CalcResizeWindow(const eDockResize::Enum type, const int deltaSize)
{
	RET(m_state != eDockState::VIRTUAL);
	RET(!m_lower || !m_upper);

	const float minCx = 50;
	const float minCy = 100;

	sRectf rect1 = m_lower->m_rect;
	sRectf rect2 = m_upper->m_rect;
	const eDockType::Enum dockType = m_lower->m_dockType;
	switch (dockType)
	{
	case eDockType::LEFT:
		rect1.right += (float)deltaSize;
		rect2.left += (float)deltaSize;

		rect1.right = max(m_rect.left + minCx, rect1.right);
		rect1.right = min(m_rect.right - minCx, rect1.right);
		rect2.left = max(m_rect.left + minCx, rect2.left);
		rect2.left = min(m_rect.right - minCx, rect2.left);
		break;

	case eDockType::TOP:
		rect1.bottom += (float)deltaSize;
		rect2.top += (float)deltaSize;

		rect1.bottom = max(m_rect.top + minCy, rect1.bottom);
		rect1.bottom = min(m_rect.bottom - minCy, rect1.bottom);
		rect2.top = max(m_rect.top + minCy, rect2.top);
		rect2.top = min(m_rect.bottom - minCy, rect2.top);
		break;

	case eDockType::RIGHT:
	case eDockType::BOTTOM:
		assert(0); // never reach this code
		break;

	case eDockType::LEFT_MOST:
	case eDockType::RIGHT_MOST:
	case eDockType::TOP_MOST:
	case eDockType::BOTTOM_MOST:
	default: assert(0);
	}
	
	m_lower->CalcResizeWindow(type, rect1);
	m_upper->CalcResizeWindow(type, rect2);
}


bool cDockWindow::RemoveTab(cDockWindow *tab)
{
	RETV(!tab, false);
	RETV(m_tabs.empty(), false);

	// todo : remove dock window
	if (!common::removevector(m_tabs, tab))
		return false;

	m_selectTab = 0;
	return true;
}


void cDockWindow::ResizeEnd(const eDockResize::Enum type, const sRectf &rect)
{
	OnResizeEnd(type, rect);

	if (m_lower)
		m_lower->ResizeEnd(type, rect);
	for (auto &p : m_tabs)
		p->ResizeEnd(type, rect);
	if (m_upper)
		m_upper->ResizeEnd(type, rect);
}


bool cDockWindow::IsInSizerSpace(const Vector2 &pos)
{
	RETV(m_state != eDockState::VIRTUAL, false);

	const float SPACE = 10;

	sRectf rect;
	const eDockType::Enum dockType = m_lower->m_dockType;
	switch (dockType)
	{
	case eDockType::LEFT:
		rect = sRectf(m_lower->m_rect.right - SPACE
			, m_rect.top
			, m_lower->m_rect.right + SPACE
			, m_rect.bottom
		);
		break;

	case eDockType::TOP:
		rect = sRectf(m_rect.left
			, m_lower->m_rect.bottom - SPACE
			, m_rect.right
			, m_lower->m_rect.bottom + SPACE
		);
		break;

	default: assert(0);
	}

	return rect.IsIn(pos.x, pos.y);
}


eDockSizingType::Enum cDockWindow::GetDockSizingType()
{
	const eDockType::Enum dockType = m_lower->m_dockType;
	switch (dockType)
	{
	case eDockType::LEFT: return eDockSizingType::HORIZONTAL;
	case eDockType::TOP: return eDockSizingType::VERTICAL;
	default: break;
	}
	return eDockSizingType::NONE;
}


void cDockWindow::SetBindState(const bool enable) // enable = true;
{
	m_isBind = enable;
	if (m_lower)
		m_lower->SetBindState(enable);
	if (m_upper)
		m_upper->SetBindState(enable);
}


void cDockWindow::LostDevice()
{
	OnLostDevice();

	if (m_lower)
		m_lower->LostDevice();
	for (auto &p : m_tabs)
		p->LostDevice();
	if (m_upper)
		m_upper->LostDevice();
}


void cDockWindow::ResetDevice(graphic::cRenderer *shared)//= NULL
{
	OnResetDevice(shared);

	if (m_lower)
		m_lower->ResetDevice(shared);
	for (auto &p : m_tabs)
		p->ResetDevice(shared);
	if (m_upper)
		m_upper->ResetDevice(shared);
}


void cDockWindow::ClearConnection()
{
	m_lower = NULL;
	m_upper = NULL;
	m_parent = NULL;
	m_owner = NULL;
	m_tabs.clear();
}


void cDockWindow::DefaultEventProc(const sf::Event &evt)
{
	if (!CheckEventTarget(evt))
		return;

	if (m_lower)
	{
		m_lower->DefaultEventProc(evt);
	}
	else
	{
		if (m_selectTab == 0)
			OnEventProc(evt);
		else if ((int)m_tabs.size() > m_selectTab - 1)
			m_tabs[m_selectTab - 1]->DefaultEventProc(evt);
	}

	if (m_upper)
	{
		m_upper->DefaultEventProc(evt);
	}
}


bool cDockWindow::CheckEventTarget(const sf::Event &evt)
{
	switch (evt.type)
	{
	case sf::Event::MouseButtonPressed:
	{
		POINT pos = { evt.mouseButton.x, evt.mouseButton.y };
		if (!m_rect.IsIn((float)pos.x, (float)pos.y))
			return false;
	}
	break;

	case sf::Event::MouseWheelMoved:
	{
		POINT pos = { evt.mouseWheel.x, evt.mouseWheel.y };
		if (!m_rect.IsIn((float)pos.x, (float)pos.y))
			return false;
	}
	break;

	case sf::Event::MouseMoved:
	case sf::Event::MouseButtonReleased:
		break;
	}

	return true;
}


// Delete All Connection Docking Window
void cDockWindow::Clear()
{
	for (auto &p : m_tabs)
	{
		p->Clear();
		delete p;
	}
	m_tabs.clear();

	if (m_lower)
	{
		m_lower->Clear();
		SAFE_DELETE(m_lower);
	}

	if (m_upper)
	{
		m_upper->Clear();
		SAFE_DELETE(m_upper);
	}
}
