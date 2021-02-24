#pragma once
#include "ShaderGraph.h"
#include "../surface/OrenNayar.h"
#include "../surface/Microfacet.h"
#include "../surface/Ghost.h"
#include "../surface/Specular.h"

LAMBDA_BEGIN

namespace ShaderGraph {

	#define _DECLTYPE std::remove_reference<decltype(*this)>::type
	#define BXDF_CALLBACK &std::add_const<_DECLTYPE>::type::GetBxDF
	#define MAKE_BXDF_SOCKET(_type, _callback, _tag) {	(_tag), NodeDelegate::FromFunction<_DECLTYPE, _callback>(this), nullptr, (_type)	}
	
	/*
		All BxDF nodes return the same thing, so we can use this macro as a
		shortcut to add the GetBxDF method that all the BxDF nodes need.
		(Inheritance is too slow)
	*/
	#define DECL_CLBCK_FUNC void GetBxDF(const ScatterEvent &_event, void *_out) const

	class MixBxDFNode : public Node, public MixBSDF {
		public:
			MixBxDFNode(Socket *_bxdfA = nullptr, Socket *_bxdfB = nullptr, Socket *_ratio = nullptr);

			DECL_CLBCK_FUNC;
	};

	class LambertianBRDFNode : public Node, public LambertianBRDF {
		public:
			LambertianBRDFNode(Socket *_albedo = nullptr);

			DECL_CLBCK_FUNC;
	};

	class OrenNayarBRDFNode : public Node, public OrenNayarBRDF {
		public:
			OrenNayarBRDFNode(Socket *_albedo = nullptr, Socket *_roughness = nullptr);

			DECL_CLBCK_FUNC;
	};

	class OrenNayarBTDFNode : public Node, public OrenNayarBTDF {
		public:
			OrenNayarBTDFNode(Socket *_albedo = nullptr, Socket *_roughness = nullptr);

			DECL_CLBCK_FUNC;
	};

	class MicrofacetBRDFNode : public Node, public MicrofacetBRDF {
		public:
			MicrofacetBRDFNode(Socket *_albedo = nullptr, Socket *_roughness = nullptr, MicrofacetDistribution *_distribution = nullptr, Fresnel *_fresnel = nullptr);

			DECL_CLBCK_FUNC;
	};

	class GhostBTDFNode : public Node, public GhostBTDF {
		public:
			GhostBTDFNode(Socket *_alpha = nullptr);

			DECL_CLBCK_FUNC;
	};

	class FresnelBSDFNode : public Node, public FresnelBSDF {
		public:
			FresnelBSDFNode(Socket *_albedo = nullptr, Socket *_ior = nullptr);

			DECL_CLBCK_FUNC;
	};

	class SpecularBRDFNode : public Node, public SpecularBRDF {
		public:
			SpecularBRDFNode(Socket *_albedo = nullptr, Fresnel *_fresnel = nullptr);

			DECL_CLBCK_FUNC;
	};

	class SpecularBTDFNode : public Node, public SpecularBTDF {
		public:
			SpecularBTDFNode(Socket *_albedo = nullptr, const Real _etaT = 1);

			DECL_CLBCK_FUNC;
	};
	
}

LAMBDA_END