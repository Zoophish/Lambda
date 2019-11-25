#pragma once
#include "GraphNode.h"

namespace ShaderGraph {

	class Maths : public Node {
		public:
			enum class OperatorType {
				ADD,
				SUBTRACT,
				MULTIPLY,
				DIVIDE
			};

			OperatorType operatorType;

			//Maths(Socket *_a, Socket *_b, const OperatorType _operatorType) : Node(2, 1) {
			//	operatorType = _operatorType;
			//	inputSockets[0] = _a;
			//	inputSockets[1] = _b;
			//	outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, this->Get, "Output");
			//}
			//
			//void Get(const SurfaceScatterEvent *_event, void *_out) const {
			//	switch (operatorType) {
			//	case OperatorType::ADD:
			//
			//	}
			//}
	};

}