#pragma once
#include <utility/Memory.h>
#include "ShaderGraph.h"

namespace ShaderGraph {

class ShaderGraphArena {
	public:
		MemoryArena arena;
};

}