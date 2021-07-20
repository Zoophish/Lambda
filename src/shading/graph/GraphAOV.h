#pragma once
#include "ShaderGraph.h"

LAMBDA_BEGIN

SG_BEGIN

class AOVOutput : public Node {
	public:
		std::string name;
		
		AOVOutput(const std::string &_name);
};

SG_END

LAMBDA_END