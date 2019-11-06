
#include "stdafx.h"
#include "graphscript.h"

using namespace common;


cGraphScript::cGraphScript()
{
}

cGraphScript::~cGraphScript()
{
	Clear();
}


// �׷��� ��ũ��Ʈ ������ �о, �׷��� ������ �����Ѵ�.
//
// Graph Script Sample
//	plot = %f;
//	name = Yaw
//	plot = %*f; %f;
//	name = Pitch
//	plot = %*f; %*f; %f;
//	name = Roll
bool cGraphScript::ReadFile(const StrPath &fileName)
{
	string allStr;
	cFile graphScriptFile;
	if (!graphScriptFile.ReadFile2String(fileName, allStr))
		return false;
	return ReadString(allStr);

}


// ��ũ��Ʈ ���ڿ��� �Ľ��ؼ�, �׷��� ������ �����Ѵ�.
bool cGraphScript::ReadString(const string &strScript)
{
	Clear();

	vector<string> strs;
	common::tokenizer(strScript, "\n", "", strs);

	for (uint i = 0; i < strs.size(); ++i)
	{
		vector<string> cmds;
		common::tokenizer(strs[i], "=", "", cmds);
		if (cmds.size() < 2)
			continue;

		common::trim(cmds[0]);

		if (cmds[0] == "plotstr")
		{
			sGraphFormat graph;
			graph.type = 0; // string type
			graph.name.Format("graph%d", i + 1);
			graph.fmt = cmds[1];
			m_graphs.push_back(graph);
		}
		else if (cmds[0] == "plot")
		{
			sGraphFormat graph;
			graph.type = 1; // binary type
			graph.name.Format("graph%d", i + 1);
			graph.idx = (uint)atoi(cmds[1].c_str());
			m_graphs.push_back(graph);
		}
		else if (cmds[0] == "name")
		{
			if (!m_graphs.empty())
				m_graphs.back().name = common::trim(cmds[1]);
		}
	}

	return true;
}


void cGraphScript::Clear()
{
	m_graphs.clear();
}
