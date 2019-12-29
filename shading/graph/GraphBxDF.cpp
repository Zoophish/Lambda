#include "GraphBxDF.h"

namespace ShaderGraph {

	MixBxDFNode::MixBxDFNode(Socket *_bxdfA, Socket *_bxdfB, Socket *_ratio)
		: BxDFNode(3, 1, "Mix BSDF"), MixBSDF(&inputSockets[0].socket, &inputSockets[1].socket, &inputSockets[2].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_BXDF, _bxdfA);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_BXDF, _bxdfB);
		inputSockets[2] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _ratio);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &MixBxDFNode::GetBxDF, "BxDF");
	}



	LambertianBRDFNode::LambertianBRDFNode(Socket *_albedo) : BxDFNode(1, 1, "Lambertian BRDF"), LambertianBRDF(&inputSockets[0].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &LambertianBRDFNode::GetBxDF, "BRDF");
	}



	OrenNayarBxDFNode::OrenNayarBxDFNode(Socket *_albedo, Socket *_roughness)
		: BxDFNode(2, 1, "Oren-Nayar BRDF"), OrenNayarBRDF(&inputSockets[0].socket, &inputSockets[1].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &OrenNayarBxDFNode::GetBxDF, "BRDF");
	}



	MicrofacetBRDFNode::MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel)
		: BxDFNode(2, 1, "Microfcet BRDF"), MicrofacetBRDF(&inputSockets[0].socket, &inputSockets[1].socket, _distribution, _fresnel) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &MicrofacetBRDFNode::GetBxDF, "BRDF");
	}



	GhostBTDFNode::GhostBTDFNode(Socket *_alpha) : BxDFNode(1, 1, "Ghost BTDF"), GhostBTDF(&inputSockets[0].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _alpha);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &GhostBTDFNode::GetBxDF, "BTDF");
	}



	FresnelBSDFNode::FresnelBSDFNode(Socket *_albedo, Socket *_ior)
		: BxDFNode(2, 1, "Fresnel BSDF"), FresnelBSDF(&inputSockets[0].socket, &inputSockets[1].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _ior);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &FresnelBSDFNode::GetBxDF, "BSDF");
	}



	SpecularBRDFNode::SpecularBRDFNode(Socket *_albedo, Fresnel *_fresnel)
		: BxDFNode(1, 1, "Specular BRDF"), SpecularBRDF(&inputSockets[0].socket, _fresnel) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &SpecularBRDFNode::GetBxDF, "BRDF");
	}



	SpecularBTDFNode::SpecularBTDFNode(Socket *_albedo, const Real _etaT)
		: BxDFNode(1, 1, "Specular BTDF"), SpecularBTDF(&inputSockets[0].socket, _etaT) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, &SpecularBTDFNode::GetBxDF, "BTDF");
	}

}