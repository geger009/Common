// ��Ʈ �׼�.
#pragma once

#include "action.h"


namespace ai
{

	template<class T>
	class cRootAction : public cAction<T>
	{
	public:
 		cRootAction(T *agent = NULL)
 			: cAction<T>(agent, "Root", "", eActionType::ROOT)
 		{
 		}
	};

}
