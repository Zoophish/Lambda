#include "GraphBxDF.h"

LAMBDA_BEGIN

namespace ShaderGraph {

	MixBxDFNode::MixBxDFNode(Socket *_bxdfA, Socket *_bxdfB, Socket *_ratio)
		: Node(3, 1, "Mix BSDF"), MixBSDF(&inputSockets[0], &inputSockets[1], &inputSockets[2]) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_BXDF, _bxdfA, "BxDF");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_BXDF, _bxdfB, "BxDF");
		inputSockets[2] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _ratio, "Ratio");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BxDF");
	}

	void MixBxDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF*>(static_cast<MixBSDF*>(const_cast<MixBxDFNode*>(this)));
	}



	LambertianBRDFNode::LambertianBRDFNode(Socket *_albedo) : Node(1, 1, "Lambertian BRDF"), LambertianBRDF(&inputSockets[0]) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void LambertianBRDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<LambertianBRDF *>(const_cast<LambertianBRDFNode *>(this)));
	}



	OrenNayarBRDFNode::OrenNayarBRDFNode(Socket *_albedo, Socket *_roughness)
		: Node(2, 1, "Oren-Nayar BRDF"), OrenNayarBRDF(&inputSockets[0], &inputSockets[1]) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness, "Roughness");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void OrenNayarBRDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<OrenNayarBRDF *>(const_cast<OrenNayarBRDFNode *>(this)));
	}



	OrenNayarBTDFNode::OrenNayarBTDFNode(Socket *_albedo, Socket *_roughness)
		: Node(2, 1, "Oren-Nayar BTDF"), OrenNayarBTDF(&inputSockets[0], &inputSockets[1]) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness, "Roughness");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BTDF");
	}

	void OrenNayarBTDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<OrenNayarBTDF *>(const_cast<OrenNayarBTDFNode *>(this)));
	}



	MicrofacetBRDFNode::MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel)
		: Node(2, 1, "Microfcet BRDF"), MicrofacetBRDF(&inputSockets[0], &inputSockets[1], _distribution, _fresnel) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness, "Roughness");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void MicrofacetBRDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<MicrofacetBRDF *>(const_cast<MicrofacetBRDFNode *>(this)));
	}



	GhostBTDFNode::GhostBTDFNode(Socket *_alpha) : Node(1, 1, "Ghost BTDF"), GhostBTDF(&inputSockets[0]) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _alpha, "Alpha");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BTDF");
	}

	void GhostBTDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<GhostBTDF *>(const_cast<GhostBTDFNode *>(this)));
	}



	FresnelBSDFNode::FresnelBSDFNode(Socket *_albedo, Socket *_ior)
		: Node(2, 1, "Fresnel BSDF"), FresnelBSDF(&inputSockets[0], &inputSockets[1]) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _ior, "IOR");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BSDF");
	}

	void FresnelBSDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<FresnelBSDF *>(const_cast<FresnelBSDFNode *>(this)));
	}



	SpecularBRDFNode::SpecularBRDFNode(Socket *_albedo, Fresnel *_fresnel)
		: Node(1, 1, "Specular BRDF"), SpecularBRDF(&inputSockets[0], _fresnel) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void SpecularBRDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<SpecularBRDF *>(const_cast<SpecularBRDFNode *>(this)));
	}



	SpecularBTDFNode::SpecularBTDFNode(Socket *_albedo, const Real _etaT)
		: Node(1, 1, "Specular BTDF"), SpecularBTDF(&inputSockets[0], _etaT) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo, "Albedo");
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BTDF");
	}

	void SpecularBTDFNode::GetBxDF(const ScatterEvent &_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<SpecularBTDF *>(const_cast<SpecularBTDFNode *>(this)));
	}

}

LAMBDA_END