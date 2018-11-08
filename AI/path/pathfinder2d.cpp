
#include "stdafx.h"
#include "pathfinder.h"
#include "pathfinder2d.h"


namespace ai
{
	enum { MAX_VERTEX = 1024};
	float g_edges_len2[MAX_VERTEX][MAX_VERTEX];
	bool  g_edges_visit2[MAX_VERTEX][MAX_VERTEX];
}

using namespace ai;
cPathFinder2D::sVertex cPathFinder2D::m_dummy;



cPathFinder2D::cPathFinder2D()
	: m_map(NULL)
	, m_graph(NULL)
{
}

cPathFinder2D::~cPathFinder2D()
{
	Clear();
}


bool cPathFinder2D::Read(const char *fileName)
{
	auto size = GetRowsCols(fileName);
	if (size.first <= 0)
		return false;

	Clear();

	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return false;

	m_rows = size.first;
	m_cols = size.second;
	m_map = new sVertex[size.first * size.second];
	ZeroMemory(m_map, size.first * size.second);

	int y = 0;
	string line;
	while (getline(ifs, line))
	{
		if (line.empty())
			break;

		int x = 0;
		for (u_int i = 0; i < line.size(); ++i)
		{
			int n = line[i] - '0';
			if (abs(n) > 9)
				return false;

			GetMap(y * m_cols + x).type = (BYTE)n;
			++x;
		}
		++y;
	}

	// find waypoint
	m_waypoints.clear();
	for (int i = 0; i < m_rows; ++i)
		for (int k = 0; k < m_cols; ++k)
			if (3 == GetMap(i*m_cols + k).type)
				m_waypoints.push_back(Vector2i(k,i));

	GenerateGraphNode();

	return true;
}


// m_map ������ �̿��ؼ� Graph�� �����Ѵ�.
bool cPathFinder2D::GenerateGraphNode()
{
	// map ��忡�� �¿�,���Ʒ����� ����� ��尡 3�� �̻��� ���,
	// waypoint�� �ν��ϰ�, �׷����� ��尡 �ȴ�.
	// ���� �׻� ����� ���� ó���Ѵ�.

	m_dummy = sVertex(); // Ȥ�� �� ���׸� �������� �ʱ�ȭ

	if (!m_graph)
		m_graph = new cPathFinder();
	m_graph->Clear();

	map<Vector2i, int> waypoints; // pos, vertex index

	vector<Vector2i> q;
	q.push_back(Vector2i(0, 0));

	while (!q.empty())
	{
		const Vector2i curPos = q.back();
		q.pop_back();

		auto it = waypoints.find(curPos);
		const int curVertexIdx = (waypoints.end() == it) ? -1 : it->second;

		// 4�������� �̵��ϸ鼭 waypoint�� �������� Ȯ���Ѵ�.
		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			Vector2i nextPos = curPos + offsetPos[i];
		
			while (CheckRange(nextPos))
			{
				// check edge?
				// ���� �������� 90�� ���� �������� ��尡 ������ waypoint��.
				const Vector2i orthoEdge = offsetPos[(i + 1) % ARRAYSIZE(offsetPos)];
				if ( (CheckRange(nextPos + orthoEdge) && (0 != GetMap(nextPos + orthoEdge).type))
					|| (CheckRange(nextPos - orthoEdge) && (0 != GetMap(nextPos - orthoEdge).type)))
				{
					// waypoint�� �߰��ߴٸ�, �̹� ��ϵ� waypoint���� Ȯ���Ѵ�.
					if (waypoints.end() == waypoints.find(nextPos))
					{
						q.push_back(nextPos);

						// add vertex
						cPathFinder::sVertex vtx;
						vtx.type = 0;
						vtx.pos = Vector3((float)nextPos.x, 0, (float)nextPos.y);
						m_graph->AddVertex(vtx);

						const int addVertexIdx = (int)m_graph->m_vertices.size() - 1;
						waypoints[nextPos] = addVertexIdx;

						// link vertex
						if (curVertexIdx >= 0)
						{
							m_graph->AddEdge(curVertexIdx, addVertexIdx);
							m_graph->AddEdge(addVertexIdx, curVertexIdx);
						}
					}
					else
					{
						// link vertex
						if (curVertexIdx >= 0)
						{
							const int linkVertexIdx = waypoints.find(nextPos)->second;
							m_graph->AddEdge(curVertexIdx, linkVertexIdx);
							m_graph->AddEdge(linkVertexIdx, curVertexIdx);
						}
					}

					break; // meet waypoint? finish loop
				}

				nextPos += offsetPos[i];
			}
		}
	}

	return true;
}


// pos�� map�� ������ �� �ִ� ���̸� true�� �����Ѵ�.
inline bool cPathFinder2D::CheckRange(const Vector2i &pos)
{
	if ((pos.x < 0)
		|| (pos.x >= m_cols)
		|| (pos.y < 0)
		|| (pos.y >= m_rows))
		return false;
	return true;
}


inline cPathFinder2D::sVertex& cPathFinder2D::GetMap(const int idx)
{
	if ((idx < 0) || (idx >= (m_rows * m_cols)))
		return m_dummy;
	if (!m_map)
		return m_dummy;
	return m_map[idx];
}


inline cPathFinder2D::sVertex& cPathFinder2D::GetMap(const Vector2i &pos)
{
	return GetMap(pos.y*m_cols + pos.x);
}


// a-star path finder
bool cPathFinder2D::Find(const Vector2i &startPos
	, const Vector2i &endPos
	, OUT vector<Vector2i> &out)
{
	ZeroMemory(g_edges_visit2, sizeof(g_edges_visit2));
	ZeroMemory(g_edges_len2, sizeof(g_edges_len2));

	vector<Vector2i> candidate;
	candidate.reserve(m_rows * m_cols);
	candidate.push_back(startPos);

	const int startIdx = startPos.y*m_cols + startPos.x;
	const int endIdx = endPos.y*m_cols + endPos.x;
	GetMap(startIdx).startLen = 0;
	GetMap(startIdx).endLen = Distance_Manhatan(startPos, endPos);

	int loopCount1 = 0; // debug, loop count
	int loopCount2 = 0; // debug, insertion count
	bool isFind = false;
	while (!candidate.empty())
	{
		const Vector2i curPos = candidate.front();
		rotatepopvector(candidate, 0);

		const int curIdx = curPos.y*m_cols + curPos.x;
		sVertex &curVtx = GetMap(curIdx);

		if (endPos == curPos)
		{
			isFind = true;
			break;
		}

		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			const Vector2i nextPos = curPos + offsetPos[i];
			if ((nextPos.x < 0)
				|| (nextPos.x >= m_cols)
				|| (nextPos.y < 0)
				|| (nextPos.y >= m_rows))
				continue;

			const int nextIdx = nextPos.y*m_cols + nextPos.x;
			sVertex &nextVtx = GetMap(nextIdx);
			if ((0 == nextVtx.type) // wall node
				|| (4 == nextVtx.type)) // block waypoint node
				continue; // ignore this node

			if (g_edges_visit2[curIdx][nextIdx])
				continue;

			float alpha = 0.f;
			if (1 == nextVtx.type)
				alpha = 1.1f;

			nextVtx.startLen = curVtx.startLen + Distance_Manhatan(curPos, nextPos) + alpha + 0.00001f;
			nextVtx.endLen = Distance_Manhatan(endPos, nextPos);
			g_edges_visit2[curIdx][nextIdx] = true;
			g_edges_visit2[nextIdx][curIdx] = true;
			g_edges_len2[curIdx][nextIdx] = nextVtx.startLen + nextVtx.endLen;
			g_edges_len2[nextIdx][curIdx] = nextVtx.startLen + nextVtx.endLen;

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				++loopCount1;

				sVertex &compVtx = GetMap(candidate[k].y * m_cols + candidate[k].x);
				if ((compVtx.endLen + compVtx.startLen)
					> (nextVtx.endLen + nextVtx.startLen))
				{
					++loopCount2;

					candidate.push_back(nextPos);
					common::rotateright2(candidate, k);
					isInsert = true;
					break;
				}
			}

			if (!isInsert)
				candidate.push_back(nextPos);
		}
	}

	if (!isFind)
		return false;

	// tracking end point to start point
	vector<Vector2i> verticesIndices;

	out.push_back(endPos);
	verticesIndices.push_back(endPos);

	ZeroMemory(g_edges_visit2, sizeof(g_edges_visit2));

	Vector2i curPos = endPos;
	while ((curPos != startPos) && (verticesIndices.size() < 1000))
	{
		float minEdge = FLT_MAX;
		Vector2i nextPos(-1,-1);

		const int curIdx = curPos.y*m_cols + curPos.x;
		sVertex &vtx = GetMap(curIdx);
		
		const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
		for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
		{
			const Vector2i edgePos = curPos + offsetPos[i];
			if ((edgePos.x < 0)
				|| (edgePos.x >= m_cols)
				|| (edgePos.y < 0)
				|| (edgePos.y >= m_rows))
				continue;

			const int edgeIdx = edgePos.y*m_cols + edgePos.x;
			if (g_edges_visit2[curIdx][edgeIdx])
				continue;

			const float len = g_edges_len2[curIdx][edgeIdx];
			if (0 == len)
				continue;

			if (minEdge > len)
			{
				minEdge = len;
				nextPos = edgePos;
			}
		}

		if (nextPos.x < 0)
		{
			assert(0);
			return false; // error occur
		}

		const int nextIdx = nextPos.y*m_cols + nextPos.x;

		g_edges_visit2[curIdx][nextIdx] = true;
		g_edges_visit2[nextIdx][curIdx] = true;
		out.push_back(nextPos);
		verticesIndices.push_back(nextPos);
		curPos = nextPos;
	}

	assert(verticesIndices.size() < 1000);

	std::reverse(out.begin(), out.end());
	std::reverse(verticesIndices.begin(), verticesIndices.end());

	return true;
}


// ��ã�� ����
// ��� ������ ���� �����Ѵ�.
// Edge�� �ߺ��� ���� �����Ѵ�.
bool cPathFinder2D::FindEnumeration(const Vector2i &startPos
	, const Vector2i &endPos
	, OUT vector<vector<Vector2i>> &out)
{
	RETV(!m_graph, false);

	const Vector3 p0((float)startPos.x, 0, (float)startPos.y);
	const Vector3 p1((float)endPos.x, 0, (float)endPos.y);

	typedef vector<cPathFinder::sEdge> epath; // edge path
	vector<epath> allEpath; // total edge path
	vector<cPathFinder::sEdge> edgeSet;
	set<cPathFinder::sEdge> disableEdges;
	int combination = 0; // disable edge combination
	
	// �پ��� ��θ� �����ϱ�����, Ư�� ������ ���´�.
	// ��ü ����� �պκ� ��, �޺κ� ��� �����ؼ�, ���´�.
	// front
	const int comboSize = 3;
	int combinationIndices1[][comboSize] = {
		{0, -1}
		,{ 0, -1}
		,{ 0, -1}
		,{ 1, -1}
		,{ 1, -1}
		,{ 1, -1}
		,{ 2, -1 }
		,{ 2, -1 }
		,{ 2, -1 }
		,{ 0, 2, -1 }
		,{ 0, 2, -1 }
		,{ 0, 2, -1 }
	};

	// back (path.size() - offset)
	int combinationIndices2[][comboSize] = {
		{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
		,{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
		,{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
		,{ -1 }
		,{ 1, -1 }
		,{ 2, -1 }
	};

	// start, end node �� �߰��Ѵ�.
	AddTemporaryVertexAndEdges(p0);
	AddTemporaryVertexAndEdges(p1);

	while (combination < 12) 
	{
		// find path
		vector<Vector3> vpath;
		vector<int> wayPoints;
		epath edges;
		if (m_graph->Find(p0, p1, vpath, &wayPoints, &edges, &disableEdges))
		{
			// search already stored path list
			{
				auto it = std::find(allEpath.begin(), allEpath.end(), edges);
				if (allEpath.end() != it)
					goto next; // if find, ignore this path
			}

			allEpath.push_back(edges);

			// add all path edges
			for (auto &e : edges)
			{
				if (edgeSet.end() == std::find(edgeSet.begin(), edgeSet.end(), e))
					edgeSet.push_back(e);
			}

			// next edge on/off combination
			if ((int)edgeSet.size() <= combination)
				continue;
		}

	next:
		// composite disable edge 
		disableEdges.clear();

		// front path skip
		for (int i = 0; i < comboSize; ++i)
		{
			const int idx = combinationIndices1[combination][i];
			if (idx < 0)
				break;
			if (idx < (int)edgeSet.size())
				disableEdges.insert(edgeSet[idx]);
		}

		// back path skip
		for (int i = 0; i < comboSize; ++i)
		{
			const int idx = (int)edgeSet.size() - combinationIndices2[combination][i];
			if ((idx < 0) || ((int)edgeSet.size() <= idx))
				break;
			if (idx > comboSize)
				disableEdges.insert(edgeSet[idx]);
		}

		++combination;
	}

	// Store Enumeration Path
	out.reserve(allEpath.size());
	for (u_int i = 0; i < allEpath.size(); ++i)
	{
		out.push_back({});
		out[i].reserve(allEpath.size());
		for (u_int k = 0; k < allEpath[i].size(); ++k)
		{
			const int from = allEpath[i][k].from;
			out[i].push_back(
				Vector2i((int)m_graph->m_vertices[from].pos.x
					, (int)m_graph->m_vertices[from].pos.z));
		}

		const int to = allEpath[i].back().to;
		out[i].push_back(
			Vector2i((int)m_graph->m_vertices[to].pos.x
				, (int)m_graph->m_vertices[to].pos.z));
	}

	RemoveTemporaryVertexAndEdges(p0);
	RemoveTemporaryVertexAndEdges(p1);

	return true;
}


// �ӽ� ��带 �߰��Ѵ�.
// ���� ��ġ�� ��� ���� �ִٸ�, Pos ��带 �߰����� �ʴ´�.
// �̹� �ֱ� ������~
// ��忡 ���� ��ġ�� Pos�� �ִٸ�, �� ��带 �߰��Ѵ�.
bool cPathFinder2D::AddTemporaryVertexAndEdges(const Vector3 &pos)
{
	RETV(!m_graph, false);

	const int nearVtx = m_graph->GetNearestVertex(pos);
	if (nearVtx < 0)
		return true;

	if (pos == m_graph->m_vertices[nearVtx].pos)
		return true;

	// ��带 �߰��ϱ� ����, �߰��� ��� ������ ������ �����ϰ�
	// �翬���Ѵ�.
	std::pair<int,int> edge = m_graph->GetNearestEdge(pos);
	if (edge.first < 0)
		return false; // error occur

	const int from = edge.first;
	const int to = edge.second;
	m_graph->RemoveEdge(from, to);
	m_graph->RemoveEdge(to, from);

	cPathFinder::sVertex vertex;
	vertex.type = 3; // temporary node
	vertex.pos = pos;
	m_graph->AddVertex(vertex);
	const int addVtxIdx = m_graph->m_vertices.size() - 1;

	if (!m_graph->AddEdge(addVtxIdx, to))
	{
		int a = 0;
	}
	if (!m_graph->AddEdge(to, addVtxIdx))
	{
		int a = 0;
	}
	if (!m_graph->AddEdge(addVtxIdx, from))
	{
		int a = 0;
	}
	if (!m_graph->AddEdge(from, addVtxIdx))
	{
		int a = 0;
	}

	return true;
}


// �ӽ� ���� �߰��� ���� �����ϰ�, ���� graph�� �����Ѵ�.
bool cPathFinder2D::RemoveTemporaryVertexAndEdges(const Vector3 &pos)
{
	RETV(!m_graph, false);

	const int temporaryVtxIdx = m_graph->GetNearestVertex(pos);
	if (temporaryVtxIdx < 0)
		return true;

	// temporary node?
	cPathFinder::sVertex &tvertex = m_graph->m_vertices[temporaryVtxIdx];
	if (tvertex.type != 3)
		return true;

	// ��带 �����ϱ� ����, ��� �ֺ� ��带 �����Ѵ�.
	int state = 0;
	int from, to;
	for (int i = 0; i < cPathFinder::sVertex::MAX_EDGE; ++i)
	{
		if (tvertex.edge[i] < 0)
			break;
		if (0 == state)
		{
			from = tvertex.edge[i];
			state = 1;
		}
		else if (1 == state)
		{
			to = tvertex.edge[i];
			state = 2;
		}
		else
		{
			assert(0);
		}
	}

	if (state != 2)
		return false;

	// edge ����, ���� �߿�
	m_graph->AddEdge(from, to);
	m_graph->AddEdge(to, from);
	m_graph->RemoveVertex(temporaryVtxIdx);

	return true;
}


// Manhatan Distance
float cPathFinder2D::Distance_Manhatan(const Vector2i &p0, const Vector2i &p1) const
{
	return (float)(abs(p0.x - p1.x) + abs(p0.y - p1.y));
}


// return map file rows, cols size
// 1 character size
std::pair<int, int> cPathFinder2D::GetRowsCols(const char *fileName)
{
	using namespace std;
	ifstream ifs(fileName);
	if (!ifs.is_open())
		return { 0, 0 };

	int rows = 0;
	int cols = 0;

	string line;
	while (getline(ifs, line))
	{
		if (line.empty())
			break;
		cols = max((int)line.size(), cols);
		++rows;
	}

	return { rows, cols };
}


void cPathFinder2D::Clear()
{
	SAFE_DELETEA(m_map);
	SAFE_DELETE(m_graph);
}
