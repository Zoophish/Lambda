#pragma once
#include "GraphNode.h"
#include "../surface/OrenNayar.h"

namespace ShaderGraph {

	class OrenNayarBxDFNode : public Node, BxDF {
		public:
			OrenNayarBxDFNode() : Node(2, 1), BxDF(BxDF_DIFFUSE) {
				inputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, nullptr, "Albedo");
				inputSockets[1] = MAKE_SOCKET(SocketType::TYPE_SCALAR, nullptr, "Roughness");
				outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this, "BRDF"); //void(*)(..) needs casting to a BxDF type WHENEVER using TYPE_BXDF?
			}
	};

}