#pragma once
#include <winsock.h>
#include "YS.hpp"

namespace kn {


	typedef __int8 ModelObj_9[832];

	
	struct PartsSelector : YS::OBJ_WINDOW {
		enum class MODE : __int32 {
			MODE_DISPLAY = 0x0,
			MODE_STEALTH = 0x1,
		};

		char* label_[33];
		u_int parts_;
		MODE mode_;
	};



}

