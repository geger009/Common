
#include "stdafx.h"
#include "node.h"

using namespace graphic;


cNode::cNode( const int id, const StrId &name) :
	m_id(id)
,	m_name(name)
,	m_parent(NULL)
,	m_shader(NULL)
,	m_isRender(true)
,	 m_scale(Vector3(1,1,1))
{
}

cNode::~cNode()
{
	Clear();
}


// 노드를 자식으로 추가한다.
bool cNode::InsertChild(cNode *node)
{
	RETV(!node, false);
	RETV(FindNode(node->GetId()), false); // Already Exist
	
	node->m_parent = this;
	m_children.push_back(node);
	return true;
}


// 자식 노드 중에 id 와 같은 노드가 있다면 리턴한다.
const cNode* cNode::FindNode(const int id) const
{
	if (m_id == id)
		return this;

	for each (auto node in m_children)
	{
		if (const cNode *ret = node->FindNode(id))
			return ret;
	}

	return NULL;
}


// 자식 노드 중에 name과 같은 노드가 있다면 리턴한다.
const cNode* cNode::FindNode(const StrId &name) const
{
	if (m_name == name)
		return this;

	for each (auto node in m_children)
	{
		if (const cNode *ret = node->FindNode(name))
			return ret;
	}

	return NULL;
}

// id 노드를 제거한다. 메모리까지 제거된다.
bool cNode::RemoveNode(const int id)
{
	for each (auto node in m_children)
	{
		if (node->GetId() == id)
		{
			delete node;
			common::removevector(m_children, node);
			return true;
		}
	}

	for each (auto node in m_children)
	{
		if (node->RemoveNode(id))
			return true;
	}

	return false;
}


// 자식노드의 Render 를 호출한다.
void cNode::Render(cRenderer &renderer, const Matrix44 &parentTm)
{
	RET(!m_isRender);
	
	for each (auto node in m_children)
	{
		node->Render(renderer, parentTm);
	}
}


// 자식노드의 RenderShadow를 호출한다.
void cNode::RenderShadow(cRenderer &renderer, const Matrix44 &viewProj,
	const Vector3 &lightPos, const Vector3 &lightDir, const Matrix44 &parentTm)
{
	for each (auto node in m_children)
	{
		node->RenderShadow(renderer, viewProj, lightPos, lightDir, parentTm);
	}
}



//// 셰이더 출력, 셰이더는 렌더링하는 객체에서 선택한다.
//void cNode::RenderShader( const Matrix44 &parentTm )
//{
//	for each (auto node, m_children)
//	{
//		node->RenderShader(parentTm);
//	}
//}
//
//
//// 셰이더를 이용해서 출력한다.
//void cNode::RenderShader( cShader &shader, const Matrix44 &parentTm )
//{
//	for each (auto node, m_children)
//	{
//		node->RenderShader(shader, parentTm);
//	}
//}
//
//
//// 셰이더를 이용해서 출력한다.
//void cNode::RenderShadow( cShader &shader, const Matrix44 &parentTm )
//{
//	for each (auto node, m_children)
//	{
//		node->RenderShadow(shader, parentTm);
//	}
//}


// 모든 노드를 제거한다.
void cNode::Clear()
{
	for each (auto node in m_children)
	{
		node->Clear();
		delete node;
	}

	m_children.clear();
}
