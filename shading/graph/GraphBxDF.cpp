#include "GraphBxDF.h"

namespace ShaderGraph {

	MixBxDFNode::MixBxDFNode(Socket *_bxdfA, Socket *_bxdfB, Socket *_ratio)
		: Node(3, 1, "Mix BSDF"), MixBSDF(&inputSockets[0].socket, &inputSockets[1].socket, &inputSockets[2].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_BXDF, _bxdfA);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_BXDF, _bxdfB);
		inputSockets[2] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _ratio);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BxDF");
	}

	void MixBxDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF*>(static_cast<MixBSDF*>(const_cast<MixBxDFNode*>(this)));
	}



	LambertianBRDFNode::LambertianBRDFNode(Socket *_albedo) : Node(1, 1, "Lambertian BRDF"), LambertianBRDF(&inputSockets[0].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void LambertianBRDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<LambertianBRDF *>(const_cast<LambertianBRDFNode *>(this)));
	}



	OrenNayarBxDFNode::OrenNayarBxDFNode(Socket *_albedo, Socket *_roughness)
		: Node(2, 1, "Oren-Nayar BRDF"), OrenNayarBRDF(&inputSockets[0].socket, &inputSockets[1].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void OrenNayarBxDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<OrenNayarBRDF *>(const_cast<OrenNayarBxDFNode *>(this)));
	}


	MicrofacetBRDFNode::MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel)
		: Node(2, 1, "Microfcet BRDF"), MicrofacetBRDF(&inputSockets[0].socket, &inputSockets[1].socket, _distribution, _fresnel) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _roughness);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void MicrofacetBRDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<MicrofacetBRDF *>(const_cast<MicrofacetBRDFNode *>(this)));
	}


	GhostBTDFNode::GhostBTDFNode(Socket *_alpha) : Node(1, 1, "Ghost BTDF"), GhostBTDF(&inputSockets[0].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _alpha);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BTDF");
	}

	void GhostBTDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<GhostBTDF *>(const_cast<GhostBTDFNode *>(this)));
	}



	FresnelBSDFNode::FresnelBSDFNode(Socket *_albedo, Socket *_ior)
		: Node(2, 1, "Fresnel BSDF"), FresnelBSDF(&inputSockets[0].socket, &inputSockets[1].socket) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _ior);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BSDF");
	}

	void FresnelBSDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<FresnelBSDF *>(const_cast<FresnelBSDFNode *>(this)));
	}


	SpecularBRDFNode::SpecularBRDFNode(Socket *_albedo, Fresnel *_fresnel)
		: Node(1, 1, "Specular BRDF"), SpecularBRDF(&inputSockets[0].socket, _fresnel) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BRDF");
	}

	void SpecularBRDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<SpecularBRDF *>(const_cast<SpecularBRDFNode *>(this)));
	}



	SpecularBTDFNode::SpecularBTDFNode(Socket *_albedo, const Real _etaT)
		: Node(1, 1, "Specular BTDF"), SpecularBTDF(&inputSockets[0].socket, _etaT) {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _albedo);
		outputSockets[0] = MAKE_BXDF_SOCKET(SocketType::TYPE_BXDF, BXDF_CALLBACK, "BTDF");
	}

	void SpecularBTDFNode::GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<BxDF **>(_out) = static_cast<BxDF *>(static_cast<SpecularBTDF *>(const_cast<SpecularBTDFNode *>(this)));
	}

}