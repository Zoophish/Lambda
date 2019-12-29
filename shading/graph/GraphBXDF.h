#pragma once
#include "ShaderGraph.h"
#include "../surface/OrenNayar.h"
#include "../surface/Microfacet.h"
#include "../surface/Ghost.h"
#include "../surface/Specular.h"

namespace ShaderGraph {

	#define MAKE_BXDF_SOCKET(_type, _callback, _tag) {	(_type), NodeDelegate::FromFunction<BxDFNode, (_callback)>(this), (_tag)	}

	class BxDFNode : public Node {
		public:
			BxDFNode(const unsigned _numIn, const unsigned _numOut, const std::string &_nodeTag) : Node(_numIn, _numOut, _nodeTag) {}

		protected:
			void GetBxDF(const SurfaceScatterEvent *_event, void *_out) const {
				_out = (void *)this;
			}
	};

	class MixBxDFNode : public BxDFNode, public MixBSDF {
		public:
			MixBxDFNode(Socket *_bxdfA, Socket *_bxdfB, Socket *_ratio);
	};

	class LambertianBRDFNode : public BxDFNode, public LambertianBRDF {
		public:
			LambertianBRDFNode(Socket *_albedo);
	};

	class OrenNayarBxDFNode : public BxDFNode, public OrenNayarBRDF {
		public:
			OrenNayarBxDFNode(Socket *_albedo, Socket *_roughness);

	};

	class MicrofacetBRDFNode : public BxDFNode, public MicrofacetBRDF {
		public:
			MicrofacetBRDFNode(Socket *_albedo, Socket *_roughness, MicrofacetDistribution *_distribution, Fresnel *_fresnel);
	};

	class GhostBTDFNode : public BxDFNode, public GhostBTDF {
		public:
			GhostBTDFNode(Socket *_alpha);
	};

	class FresnelBSDFNode : public BxDFNode, public FresnelBSDF {
		public:
			FresnelBSDFNode(Socket *_albedo, Socket *_ior);
	};

	class SpecularBRDFNode : public BxDFNode, public SpecularBRDF {
		public:
			SpecularBRDFNode(Socket *_albedo, Fresnel *_fresnel);
	};

	class SpecularBTDFNode : public BxDFNode, public SpecularBTDF {
		public:
			SpecularBTDFNode(Socket *_albedo, const Real _etaT);
	};
	
}