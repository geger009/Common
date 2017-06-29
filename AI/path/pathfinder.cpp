
#include "stdafx.h"
#include "pathfinder.h"



using namespace ai;


cPathFinder::cPathFinder()
{
}

cPathFinder::~cPathFinder()
{
	Clear();
}


bool cPathFinder::Create(const int vertexCount)
{
	m_vertices.reserve(vertexCount);
	return true;
}


bool cPathFinder::Read(const string &fileName)
{
	Clear();

	std::ifstream ifs(fileName);
	RETV(!ifs.is_open(), false);

	int state = 0;
	ai::sVertex vtx;

	char line[256];
	while (ifs.getline(line, sizeof(line)))
	{
		std::stringstream ss(line);
		
		char token[64];
		switch (state)
		{
		case 0:
		{
			ss >> token;
			if (!strcmp(token, "Vertex"))
				state = 1;
		}
		break;

		case 1:
		{
			ss >> token;
			assert(!strcmp(token, "pos"));
			ss >> vtx.pos.x >> vtx.pos.y >> vtx.pos.z;
			state = 2;
		}
		break;

		case 2:
		{
			ss >> token;
			assert(!strcmp(token, "edge"));
			int idx = 0;
			int cnt = 0;
			do {
				ss >> vtx.edge[idx++];
				++cnt;
			} while (!ss.eof() && (cnt < ai::sVertex::MAX_EDGE));

			AddVertex(vtx);
			
			state = 0;
			for (int i=0; i < ai::sVertex::MAX_EDGE; ++i)
				vtx.edge[i] = -1;
		}
		break;
		}
	}

	return true;
}


// Write Format
// Vertex
//	pos 1 1 1
//	edge 0 1 2
bool cPathFinder::Write(const string &fileName)
{
	using namespace std;

	ofstream ofs(fileName);
	RETV(!ofs.is_open(), false);

	for (auto &v : m_vertices)
	{
		ofs << "Vertex" << endl;
		ofs << "\tpos " << v.pos.x << " " << v.pos.y << " " << v.pos.z << endl;
		ofs << "\tedge ";
		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (v.edge[i] < 0)
				break;
			ofs << v.edge[i] << " ";
		}
		ofs << endl;
	}

	return true;
}


bool cPathFinder::AddVertex(const sVertex &vtx)
{
	m_vertices.push_back(vtx);
	return true;
}


bool cPathFinder::RemoveVertex(const int index)
{
	// remove edge and decrease index if greater than remove index
	for (auto &v : m_vertices)
	{
		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (0 > v.edge[i])
				break;

			if (index == v.edge[i]) // rotate left
			{
				for (int k = i; k < ai::sVertex::MAX_EDGE - 1; ++k)
					v.edge[k] = v.edge[k + 1];
				v.edge[ai::sVertex::MAX_EDGE - 1] = -1;

				--i; // for loop bugfix
			}
			else if (index < v.edge[i])
			{
				--v.edge[i]; // decrease index
			}
		}
	}

	// remove index vertex
	common::rotatepopvector(m_vertices, index);
	
	return true;
}


int cPathFinder::GetNearestVertex(const Vector3 &pos) const
{
	RETV(m_vertices.empty(), -1);

	int idx = 0;
	float minLen = FLT_MAX;

	for (u_int i=0; i < m_vertices.size(); ++i)
	{
		auto &vtx = m_vertices[i];
		const float len = vtx.pos.LengthRoughly(pos);
		if (len < minLen)
		{
			idx = (int)i;
			minLen = len;
		}
	}

	return idx;
}


bool cPathFinder::Find(const Vector3 &start, const Vector3 &end,
	OUT vector<Vector3> &out)
{
	const int startIdx = GetNearestVertex(start);
	if (startIdx < 0)
		return false;

	const int endIdx = GetNearestVertex(end);
	if (endIdx < 0)
		return false;

	for (auto &vtx : m_vertices)
	{
		vtx.visit = false;
		vtx.check = false;
		vtx.reg = false;
	}

	vector<int> candidate;
	candidate.reserve(m_vertices.size());
	candidate.push_back(startIdx);
	m_vertices[startIdx].visit = true;
	m_vertices[startIdx].check = true;
	m_vertices[startIdx].startLen = 0;
	m_vertices[startIdx].endLen = Distance(start, end);

	int loopCount1 = 0; // debug, loop count
	int loopCount2 = 0; // debug, insertion count
	bool isFind = false;
	while (!candidate.empty())
	{
		const int curIdx = candidate.front();
		rotatepopvector(candidate, 0);

		sVertex &curVtx = m_vertices[curIdx];
		curVtx.visit = true;

		if (endIdx == curIdx)
		{
			isFind = true;
			break;
		}

		for (int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (curVtx.edge[i] < 0)
				break;

			const int nextIdx = curVtx.edge[i];
			sVertex &nextVtx = m_vertices[nextIdx];

			if (nextVtx.visit || nextVtx.check)
				continue;

			nextVtx.check = true;
			nextVtx.startLen = curVtx.startLen + Distance(curVtx.pos, nextVtx.pos);
			nextVtx.endLen = Distance(end, nextVtx.pos);

			// sorting candidate
			// value = minimum( startLen + endLen )
			bool isInsert = false;
			for (u_int k = 0; k < candidate.size(); ++k)
			{
				++loopCount1;

				sVertex &compVtx = m_vertices[candidate[k]];
				if ((compVtx.endLen + compVtx.startLen) 
						> (nextVtx.endLen + nextVtx.startLen))
				{
					++loopCount2;

					candidate.push_back(nextIdx);
					common::rotateright2(candidate, k);
					isInsert = true;
					break;
				}
			}

			if (!isInsert)
				candidate.push_back(nextIdx);
		}
	}

	if (!isFind)
		return false;

	// tracking end point to start point
	m_vertices[endIdx].reg = true;
	out.push_back(end);
	out.push_back(m_vertices[endIdx].pos);

	int curIdx = endIdx;
	while (curIdx != startIdx)
	{
		sVertex &curVtx = m_vertices[curIdx];

		float minLen = FLT_MAX;
		int minIdx = -1;
		for (u_int i = 0; i < sVertex::MAX_EDGE; ++i)
		{
			if (curVtx.edge[i] < 0)
				break;

			const int nextIdx = curVtx.edge[i];
			sVertex &nextVtx = m_vertices[nextIdx];
			if (!nextVtx.visit || !nextVtx.check || nextVtx.reg)
				continue;

			if (minIdx < 0)
			{
				minIdx = nextIdx;
			}
			else if ((nextVtx.startLen) < (m_vertices[minIdx].startLen))
			{
				minIdx = nextIdx;
			}
		}

		if (minIdx < 0)
			break; // error occur

		m_vertices[minIdx].reg = true;
		out.push_back(m_vertices[minIdx].pos);
		curIdx = minIdx;
	}

	std::reverse(out.begin(), out.end());

	return true;
}


// Manhatan Distance
float cPathFinder::Distance(const Vector3 &p0, const Vector3 &p1) const
{
	return abs(p0.x - p1.x) + abs(p0.y - p1.y) + abs(p0.z - p1.z);
}


void cPathFinder::Clear()
{
	m_vertices.clear();
}