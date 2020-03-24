#pragma once
#include "BxDF.h"
#include "../Fresnel.h"

LAMBDA_BEGIN

class FresnelBSDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket, **iorSocket;

		FresnelBSDF(ShaderGraph::Socket **_albedoSocket, ShaderGraph::Socket **_iorSocket);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;
};



class SpecularBRDF : public BxDF {
	public:
		Fresnel *fresnel;
		ShaderGraph::Socket **albedoSocket;
	
		SpecularBRDF(ShaderGraph::Socket **_albedoSocket, Fresnel *_fresnel);
	
		Spectrum f(const ScatterEvent &_event) const override;
		
		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};



class SpecularBTDF : public BxDF {
	public:
		FresnelDielectric fresnel;
		ShaderGraph::Socket **albedoSocket;

		SpecularBTDF(ShaderGraph::Socket **_albedoSocket, const Real _etaT);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;
};

LAMBDA_END