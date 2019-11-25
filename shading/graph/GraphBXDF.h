#pragma once
#include "GraphNode.h"
#include "../surface/OrenNayar.h"
#include "../surface/Microfacet.h"

namespace ShaderGraph {

	class OrenNayarBxDFNode : public Node, public OrenNayarBRDF {
		public:
			OrenNayarBxDFNode(Socket *_albedo, Socket *_roughness) : Node(2, 1), OrenNayarBRDF(&inputSockets[0], &inputSockets[1]) {
				inputSockets[0] = _albedo;
				inputSockets[1] = _roughness;
				//outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this, "BRDF"); //void(*)(..) needs casting to a BxDF type WHENEVER using TYPE_BXDF?
			}
	};

	class MicrofacetBRDFNode : public Node, public MicrofacetBRDF {
		public:
			MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel)
			: Node(2, 1), MicrofacetBRDF(&inputSockets[0], &inputSockets[1], _distribution, _fresnel) {
				inputSockets[0] = _albedo;
				inputSockets[1] = _roughness;
			}
	};
	
}