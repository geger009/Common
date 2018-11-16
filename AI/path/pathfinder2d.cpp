
#include "stdafx.h"
#include "pathfinder.h"
#include "pathfinder2d.h"


using namespace ai;
cPathFinder2D::sVertex cPathFinder2D::m_dummy;



cPathFinder2D::cPathFinder2D()
	: m_map(NULL)
	, m_fastmap(NULL)
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

	if (!m_fastmap)
		m_fastmap = new cPathFinder();
	m_fastmap->Clear();

	set<Vector2i> visitSet;
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

			if (visitSet.end() != visitSet.find(nextPos))
				continue; // already visit
			visitSet.insert(nextPos);

			while (CheckRange(nextPos))
			{
				// check edge?
				// ���� �������� 90�� ���� �������� ��尡 ������ waypoint��.
				// offsetPos[]�� �ð�������� ����Ǳ� ������ +1�� �׻� 90�� ���� �����̴�.
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
						m_fastmap->AddVertex(vtx);

						const int addVertexIdx = (int)m_fastmap->m_vertices.size() - 1;
						waypoints[nextPos] = addVertexIdx;

						// link vertex
						if (curVertexIdx >= 0)
						{
							m_fastmap->AddEdge(curVertexIdx, addVertexIdx);
							m_fastmap->AddEdge(addVertexIdx, curVertexIdx);

							// move backward to update edgekey
							GetMap(curPos).edgeKey = -1; // waypoint hasn't edgeKey
							Vector2i back = nextPos - offsetPos[i];
							while (curPos != back)
							{
								GetMap(back).edgeKey = MakeUniqueEdgeKey(curVertexIdx, addVertexIdx);
								back -= offsetPos[i];
							}
						}
					}
					else
					{
						// link vertex
						if (curVertexIdx >= 0)
						{
							const int linkVertexIdx = waypoints.find(nextPos)->second;
							m_fastmap->AddEdge(curVertexIdx, linkVertexIdx);
							m_fastmap->AddEdge(linkVertexIdx, curVertexIdx);

							// move backward to update edgekey
							GetMap(curPos).edgeKey = -1; // waypoint hasn't edgeKey
							Vector2i back = nextPos - offsetPos[i];
							while (curPos != back)
							{
								GetMap(back).edgeKey = MakeUniqueEdgeKey(curVertexIdx, linkVertexIdx);
								back -= offsetPos[i];
							}
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
	set<int> visitSet;
	map<int, float> lenSet; // key = edgeKey, data = length

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

			const int edgeKey1 = MakeEdgeKey(curIdx, nextIdx);
			const int edgeKey2 = MakeEdgeKey(nextIdx, curIdx);
			if (visitSet.end() != visitSet.find(edgeKey1))
				continue; // is visit?

			float alpha = 0.f;
			if (1 == nextVtx.type)
				alpha = 1.1f;

			nextVtx.startLen = curVtx.startLen + Distance_Manhatan(curPos, nextPos) + alpha + 0.00001f;
			nextVtx.endLen = Distance_Manhatan(endPos, nextPos);
			visitSet.insert(edgeKey1);
			visitSet.insert(edgeKey2);
			lenSet[edgeKey1] = nextVtx.startLen + nextVtx.endLen;
			lenSet[edgeKey2] = nextVtx.startLen + nextVtx.endLen;

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

	visitSet.clear();

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
			const int edgeKey = MakeEdgeKey(curIdx, edgeIdx);
			if (visitSet.end() != visitSet.find(edgeKey))
				continue; // is visit?

			auto it = lenSet.find(edgeKey);
			if (lenSet.end() == it)
				continue;
			const float len = it->second;
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

		visitSet.insert(MakeEdgeKey(curIdx, nextIdx));
		visitSet.insert(MakeEdgeKey(nextIdx, curIdx));
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
	, OUT vector<ppath> &outPos
	, OUT vector<ppath> &outVertexPos
	, OUT vector<epath> &outEdges
)
{
	RETV(!m_fastmap, false);

	const Vector3 p0((float)startPos.x, 0, (float)startPos.y);
	const Vector3 p1((float)endPos.x, 0, (float)endPos.y);

	vector<epath> allEpath; // total edge path
	epath edgeSet;
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
		if (m_fastmap->Find(p0, p1, vpath, &wayPoints, &edges, &disableEdges))
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
	outPos.reserve(allEpath.size());
	outVertexPos.reserve(allEpath.size());
	outEdges.reserve(allEpath.size());
	for (u_int i = 0; i < allEpath.size(); ++i)
	{
		if (allEpath[i].empty())
			continue;

		outEdges.push_back(allEpath[i]);

		outPos.push_back({});
		GetEdgePath2PosPath(allEpath[i], outPos[i]);

		outVertexPos.push_back({});
		outVertexPos[i].reserve(allEpath[i].size());
		for (u_int k = 0; k < allEpath[i].size(); ++k)
		{
			const int from = allEpath[i][k].from;
			outVertexPos[i].push_back(
				Vector2i((int)m_fastmap->m_vertices[from].pos.x
					, (int)m_fastmap->m_vertices[from].pos.z));
		}

		const int to = allEpath[i].back().to;
		outVertexPos[i].push_back(
			Vector2i((int)m_fastmap->m_vertices[to].pos.x
				, (int)m_fastmap->m_vertices[to].pos.z));
	}

	RemoveTemporaryVertexAndEdges(p0);
	RemoveTemporaryVertexAndEdges(p1);

	return true;
}


// m_fastmap�� edge path ������ position path �� �����ؼ� �����Ѵ�.
// ù��° ���ؽ��� ������ ���ؽ��� �������� �߰��� ���ؽ��̱� ������, sVertex::edgeKey�� 
// �������� �ʴ´�. �׷��� replaceFromIdx �� �����ؼ� �˻��Ѵ�.
// ������ ���ؽ��� ���������, edgeKey�� �����ϱ� ������, ���� �� �����ϴ�.
bool cPathFinder2D::GetEdgePath2PosPath(const epath &edgePath, OUT ppath &out)
{
	RETV(!m_fastmap, false);
	RETV(edgePath.empty(), false);

	// front vertex
	// �Ųٷ� �˻��� ��, �����´�.
	// ���� ���ؽ��� ����������� �������� �𸣱� ������.
	bool isFrontReverse = false;

	if (edgePath.size() == 1)
	{
		const cPathFinder::sEdge &front = edgePath.front();
		const auto &frontV = m_fastmap->m_vertices[front.from];
		const auto &backV = m_fastmap->m_vertices[front.to];

		// �߰��� ���ؽ����� �ƴ����� ���� collect �Ǵ� ����� �ٸ���.
		const bool b1 = (frontV.replaceFromIdx >= 0);
		const bool b2 = (backV.replaceFromIdx >= 0);

		// 1. �� ���� �ȿ� start, end�� ���� ���
		if (b1 && b2)
		{
			CollectEdgeAddedVertices(frontV.replaceFromIdx, frontV.replaceToIdx
				, MakeUniqueEdgeKey(frontV.replaceFromIdx, frontV.replaceToIdx)
				, front.from, front.to
				, out);
		}
		// 2. from�� �߰��� ���ؽ� �� ���
		else if (b1 && !b2)
		{
			// �Ųٷ� �˻��� ��, �����´�.
			isFrontReverse = true;
		}
		// 3. to�� �߰��� ���ؽ� �� ���
		else if (!b1 && b2)
		{
			const int to = (front.from == backV.replaceFromIdx) ? backV.replaceToIdx : backV.replaceFromIdx;
			CollectEdgeVertices(front.from, front.to, MakeUniqueEdgeKey(front.from, to), out);

			const auto &toV = m_fastmap->m_vertices[front.to];
			out.push_back(Vector2i((int)toV.pos.x, (int)toV.pos.z));
		}
		// 4. �Ѵ� �߰��� ���ؽ��� �ƴ� ���
		else if (!b1 && !b2)
		{
			CollectEdgeVertices(front.from, front.to, MakeUniqueEdgeKey(front.to, front.from), out);

			const auto &toV = m_fastmap->m_vertices[front.to];
			out.push_back(Vector2i((int)toV.pos.x, (int)toV.pos.z));
		}
		else
		{
			assert(0);
		}
	}

	if (isFrontReverse || (edgePath.size() > 1))
	{
		const cPathFinder::sEdge &front = edgePath.front();
		const auto &frontV = m_fastmap->m_vertices[front.from];

		int from = front.from;
		if (frontV.replaceFromIdx >= 0) // temporary added vertex?
			from = (frontV.replaceToIdx == front.to) ? frontV.replaceFromIdx : frontV.replaceToIdx;

		CollectEdgeVertices(front.to, front.from, MakeUniqueEdgeKey(front.to, from), out);

		out.push_back(Vector2i((int)frontV.pos.x, (int)frontV.pos.z));
		std::reverse(out.begin(), out.end());
		
		if (!isFrontReverse) // ���� ����Ʈ�� 2�� �̻��� ���� �ش�ȴ�.
			out.pop_back(); // ������ ���� �ߺ��̱� ������ ����
	}

	for (u_int i = 1; i < edgePath.size() - 1; ++i)
	{
		const cPathFinder::sEdge &edge = edgePath[i];
		CollectEdgeVertices(edge.from, edge.to, MakeUniqueEdgeKey(edge.to, edge.from), out);
	}

	// back vertex
	if (edgePath.size() > 1)
	{
		// ������ ���ؽ��� ����������� �������� �𸣱� ������ 
		// replaceFromIdx, replaceToIdx�� �����Ѵ�.
		const cPathFinder::sEdge &back = edgePath.back();
		const auto &backV = m_fastmap->m_vertices[back.to];

		int to = back.to;
		if (backV.replaceFromIdx >= 0) // temporary added vertex?
			to = (backV.replaceFromIdx == back.from) ? backV.replaceToIdx : backV.replaceFromIdx;

		CollectEdgeVertices(back.from, back.to, MakeUniqueEdgeKey(back.from, to), out);
		out.push_back(Vector2i((int)backV.pos.x, (int)backV.pos.z));
	}

	return true;
}


// fastmap�� ���� ������ ����, from���� to�� ���ϴ� ���ؽ� ��ġ ������
// out�� �����ؼ� �����Ѵ�.
bool cPathFinder2D::CollectEdgeVertices(const int from, const int to
	, const int uniqueEdgeKey, OUT ppath &out)
{
	auto &fromV = m_fastmap->m_vertices[from];
	const auto &destV = m_fastmap->m_vertices[to];
	const Vector2i destPos = Vector2i((int)destV.pos.x, (int)destV.pos.z);

	const Vector2i curPos = Vector2i((int)fromV.pos.x, (int)fromV.pos.z);
	const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
	for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
	{
		const Vector2i nextPos = curPos + offsetPos[i];
		if (!CheckRange(nextPos))
			continue;

		if (uniqueEdgeKey == GetMap(nextPos).edgeKey)
		{
			// to waypoint�� ���ϴ� ���ؽ� �߰�
			// to ���ؽ��� ������ ������ ��ġ ����
			Vector2i p = curPos;
			while (CheckRange(p) && (destPos != p))
			{
				out.push_back(p);
				p += offsetPos[i];
			}
			
			return true;
		}
	}

	return false;
}


// fastmap�� ���� ������ ����, from���� to�� ���ϴ� ���ؽ� ��ġ ������
// out�� �����ؼ� �����Ѵ�.
// �� ��, �߰��� ���ؽ��� edgekey�� �����Ǿ� ���� �����Ƿ�, from���� to�� ���ϴ� ���� �߿� 
// �߰��� ���ؽ��� ��Ÿ���� out�� �����ϴ� ������� ó���Ѵ�. 
// �� ������ �߰��� ���ؽ��� 2���� ���� �� ó���ϴ� �Լ���.
bool cPathFinder2D::CollectEdgeAddedVertices(const int from, const int to, const int uniqueEdgeKey
	, const int addedVertexFrom, const int addedVertexTo
	, OUT ppath &out)
{
	const auto &fromV = m_fastmap->m_vertices[from];
	const auto &destV = m_fastmap->m_vertices[to];
	const Vector2i destPos = Vector2i((int)destV.pos.x, (int)destV.pos.z);

	const auto &addedFromV = m_fastmap->m_vertices[addedVertexFrom];
	const auto &addedToV = m_fastmap->m_vertices[addedVertexTo];
	const Vector2i addedFromPos = Vector2i((int)addedFromV.pos.x, (int)addedFromV.pos.z);
	const Vector2i addedToPos = Vector2i((int)addedToV.pos.x, (int)addedToV.pos.z);

	const Vector2i curPos = Vector2i((int)fromV.pos.x, (int)fromV.pos.z);
	const Vector2i offsetPos[] = { Vector2i(-1,0), Vector2i(0,-1), Vector2i(1,0), Vector2i(0,1) };
	for (int i = 0; i < ARRAYSIZE(offsetPos); ++i)
	{
		const Vector2i nextPos = curPos + offsetPos[i];
		if (!CheckRange(nextPos))
			continue;

		if (uniqueEdgeKey == GetMap(nextPos).edgeKey)
		{
			// to waypoint�� ���ϴ� ���ؽ� �߰�
			// to ���ؽ��� ������ ������ ��ġ ����
			int state = 1;
			Vector2i p = curPos;
			while (CheckRange(p) && (state != 4))
			{
				switch (state)
				{
				case 1:
					if (p == addedFromPos)
					{
						out.push_back(p);
						state = 2;
					}
					else if (p == addedToPos)
					{
						out.push_back(p);
						state = 3;
					}
					break;

				case 2:
					out.push_back(p);
					if (addedToPos == p)
						state = 4; // finisth
					break;

				case 3:
					out.push_back(p);
					if (addedFromPos == p)
						state = 4; // finisth
					break;
				}

				p += offsetPos[i];
			}

			return true;
		}
	}

	return false;
}


// �ӽ� ��带 �߰��Ѵ�.
// ���� ��ġ�� ���ؽ� ���� �ִٸ�, Pos ���ؽ��� �߰����� �ʴ´�.
// �̹� �ֱ� ������~
// ��忡 ���� ��ġ�� Pos�� �ִٸ�, �� ���ؽ��� �߰��Ѵ�.
bool cPathFinder2D::AddTemporaryVertexAndEdges(const Vector3 &pos)
{
	RETV(!m_fastmap, false);

	const int nearVtx = m_fastmap->GetNearestVertex(pos);
	if (nearVtx < 0)
		return true;

	if (pos == m_fastmap->m_vertices[nearVtx].pos)
		return true;

	// ��带 �߰��ϱ� ����, �߰��� ��� ������ ������ �����ϰ�
	// �翬���Ѵ�.
	std::pair<int,int> edge = m_fastmap->GetNearestEdge(pos);
	if (edge.first < 0)
		return false; // error occur

	const int from = edge.first;
	const int to = edge.second;
	m_fastmap->RemoveEdge(from, to);
	m_fastmap->RemoveEdge(to, from);

	cPathFinder::sVertex vertex;
	vertex.type = 3; // temporary node
	vertex.replaceFromIdx = from;
	vertex.replaceToIdx = to;
	vertex.pos = pos;
	m_fastmap->AddVertex(vertex);
	const int addVtxIdx = m_fastmap->m_vertices.size() - 1;

	if (!m_fastmap->AddEdge(addVtxIdx, to))
	{
		assert(0);
	}
	if (!m_fastmap->AddEdge(to, addVtxIdx))
	{
		assert(0);
	}
	if (!m_fastmap->AddEdge(addVtxIdx, from))
	{
		assert(0);
	}
	if (!m_fastmap->AddEdge(from, addVtxIdx))
	{
		assert(0);
	}

	return true;
}


// �ӽ� ���� �߰��� ���� �����ϰ�, ���� graph�� �����Ѵ�.
bool cPathFinder2D::RemoveTemporaryVertexAndEdges(const Vector3 &pos)
{
	RETV(!m_fastmap, false);

	const int temporaryVtxIdx = m_fastmap->GetNearestVertex(pos);
	if (temporaryVtxIdx < 0)
		return true;

	// temporary node?
	cPathFinder::sVertex &tvertex = m_fastmap->m_vertices[temporaryVtxIdx];
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
	m_fastmap->AddEdge(from, to);
	m_fastmap->AddEdge(to, from);
	m_fastmap->RemoveVertex(temporaryVtxIdx);

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


inline int cPathFinder2D::MakeEdgeKey(const int from, const int to) const
{
	return from * 1000 + to;
}


// �ڱ��ڽ����� ���ϴ� ���� ������ �� ����.
inline int cPathFinder2D::MakeUniqueEdgeKey(const int from, const int to) const
{
	return max(from, to)*1000 + min(from, to);
}


// edgeKey �и�
inline std::pair<int, int> cPathFinder2D::SeparateEdgeKey(const int edgeKey) const
{
	return {edgeKey/1000, edgeKey%1000};
}


void cPathFinder2D::Clear()
{
	SAFE_DELETEA(m_map);
	SAFE_DELETE(m_fastmap);
}
