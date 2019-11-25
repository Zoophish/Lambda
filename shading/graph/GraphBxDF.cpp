#include "GraphBxDF.h"

namespace ShaderGraph {

	MixBxDFNode::MixBxDFNode(Socket *_bxdfA, Socket *_bxdfB, Socket *_ratio) : BxDFNode(3, 1), MixBSDF(&inputSockets[0], &inputSockets[1], &inputSockets[2]) {
		inputSockets[0] = _bxdfA;
		inputSockets[1] = _bxdfB;
		inputSockets[2] = _ratio;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BxDF");
	}



	LambertianBRDFNode::LambertianBRDFNode(Socket *_albedo) : BxDFNode(1, 1), LambertianBRDF(&inputSockets[0]) {
		inputSockets[0] = _albedo;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BRDF");
	}



	OrenNayarBxDFNode::OrenNayarBxDFNode(Socket *_albedo, Socket *_roughness) : BxDFNode(2, 1), OrenNayarBRDF(&inputSockets[0], &inputSockets[1]) {
		inputSockets[0] = _albedo;
		inputSockets[1] = _roughness;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BRDF");
	}



	MicrofacetBRDFNode::MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel)
		: BxDFNode(2, 1), MicrofacetBRDF(&inputSockets[0], &inputSockets[1], _distribution, _fresnel) {
		inputSockets[0] = _albedo;
		inputSockets[1] = _roughness;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BRDF");
	}



	GhostBTDFNode::GhostBTDFNode(Socket *_alpha) : BxDFNode(1, 1), GhostBTDF(&inputSockets[0]) {
		inputSockets[0] = _alpha;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BTDF");
	}



	FresnelBSDFNode::FresnelBSDFNode(Socket *_albedo, Socket *_ior) : BxDFNode(2, 1), FresnelBSDF(&inputSockets[0], &inputSockets[1]) {
		inputSockets[0] = _albedo;
		inputSockets[1] = _ior;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BSDF");
	}



	SpecularBRDFNode::SpecularBRDFNode(Socket *_albedo, Fresnel *_fresnel) : BxDFNode(1, 1), SpecularBRDF(&inputSockets[0], _fresnel) {
		inputSockets[0] = _albedo;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BRDF");
	}



	SpecularBTDFNode::SpecularBTDFNode(Socket *_albedo, const Real _etaT) : BxDFNode(1, 1), SpecularBTDF(&inputSockets[0], _etaT) {
		inputSockets[0] = _albedo;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_BXDF, this->GetBxDF, "BTDF");
	}

}