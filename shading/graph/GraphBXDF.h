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
	#define MAKE_BXDF_SOCKET(_type, _callback, _tag) {	(_type), NodeDelegate::FromFunction<_DECLTYPE, _callback>(this), (_tag)	}
	
	#define DECL_CLBCK_FUNC void GetBxDF(const ScatterEvent *_event, void *_out) const;

	class MixBxDFNode : public Node, public MixBSDF {
		public:
			MixBxDFNode(Socket *_bxdfA, Socket *_bxdfB, Socket *_ratio);

			DECL_CLBCK_FUNC
	};

	class LambertianBRDFNode : public Node, public LambertianBRDF {
		public:
			LambertianBRDFNode(Socket *_albedo);

			DECL_CLBCK_FUNC
	};

	class OrenNayarBxDFNode : public Node, public OrenNayarBRDF {
		public:
			OrenNayarBxDFNode(Socket *_albedo, Socket *_roughness);

			DECL_CLBCK_FUNC
	};

	class MicrofacetBRDFNode : public Node, public MicrofacetBRDF {
		public:
			MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel);

			DECL_CLBCK_FUNC
	};

	class GhostBTDFNode : public Node, public GhostBTDF {
		public:
			GhostBTDFNode(Socket *_alpha);

			DECL_CLBCK_FUNC
	};

	class FresnelBSDFNode : public Node, public FresnelBSDF {
		public:
			FresnelBSDFNode(Socket *_albedo, Socket *_ior);

			DECL_CLBCK_FUNC
	};

	class SpecularBRDFNode : public Node, public SpecularBRDF {
		public:
			SpecularBRDFNode(Socket *_albedo, Fresnel *_fresnel);

			DECL_CLBCK_FUNC
	};

	class SpecularBTDFNode : public Node, public SpecularBTDF {
		public:
			SpecularBTDFNode(Socket *_albedo, const Real _etaT);

			DECL_CLBCK_FUNC
	};
	
}

LAMBDA_END