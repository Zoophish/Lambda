#pragma once
#include "BxDF.h"
#include "../Fresnel.h"

class FresnelBSDF : public BxDF {
	public:
		ShaderGraph::Socket **albedoSocket, **iorSocket;

		FresnelBSDF(ShaderGraph::Socket **_albedoSocket, ShaderGraph::Socket **_iorSocket);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const override;
};



class SpecularBRDF : public BxDF {
	public:
		Fresnel *fresnel;
		ShaderGraph::Socket **albedoSocket;
	
		SpecularBRDF(ShaderGraph::Socket **_albedoSocket, Fresnel *_fresnel);
	
		Spectrum f(const SurfaceScatterEvent &_event) const override;
		
		Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};



class SpecularBTDF : public BxDF {
	public:
		FresnelDielectric fresnel;
		ShaderGraph::Socket **albedoSocket;

		SpecularBTDF(ShaderGraph::Socket **_albedoSocket, const Real _etaT);

		Spectrum f(const SurfaceScatterEvent &_event) const override;

		Spectrum Sample_f(SurfaceScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const SurfaceScatterEvent &_event) const override;
};