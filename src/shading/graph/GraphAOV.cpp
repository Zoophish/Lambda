#include "GraphAOV.h"

LAMBDA_BEGIN

SG_BEGIN

AOVOutput::AOVOutput(const std::string &_name) : Node(1, 0, "AOV Output") {
	name = _name;
	inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_NULL, nullptr, "Input Variable");
}

SG_END

LAMBDA_END