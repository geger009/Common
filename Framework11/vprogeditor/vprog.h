//
// 2019-10-26, jjuiddong
// visual programming editor
//		- ui definition
//
#pragma once


namespace framework { 
	namespace vprog {
		DECLARE_ENUM( PinType,
			Flow,
			Bool,
			Int,
			Float,
			String,
			Object,
			Function,
			Delegate
		);

		DECLARE_ENUM( PinKind,
			Output,
			Input
		);

		DECLARE_ENUM( NodeType,
			None, 
			Function,
			Event,
			Control,
			Operator,
			Variable,
			Comment,
			Tree
		);
	}//~vprog
}//~framework


#include "pin.h"
#include "node.h"
#include "link.h"
#include "nodefile.h"
#include "editmanager.h"


namespace framework { namespace vprog {
		ImColor GetIconColor(PinType::Enum type);
 		void DrawPinIcon(const sPin& pin, bool connected, int alpha);
		bool CanCreateLink(sPin* a, sPin* b);
 	}
}
