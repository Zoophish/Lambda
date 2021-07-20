#pragma once
#include "BxDF.h"
#include "../Fresnel.h"

LAMBDA_BEGIN

class FresnelBSDF : public BxDF {
	public:
		ShaderGraph::SocketRef *albedoSocket, *iorSocket;

		FresnelBSDF(ShaderGraph::SocketRef *_albedoSocket, ShaderGraph::SocketRef *_iorSocket);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;
};



class SpecularBRDF : public BxDF {
	public:
		Fresnel *fresnel;
		ShaderGraph::SocketRef *albedoSocket;
	
		SpecularBRDF(ShaderGraph::SocketRef *_albedoSocket, Fresnel *_fresnel);
	
		Spectrum f(const ScatterEvent &_event) const override;
		
		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;
};



class SpecularBTDF : public BxDF {
	public:
		FresnelDielectric fresnel;
		ShaderGraph::SocketRef *albedoSocket;

		SpecularBTDF(ShaderGraph::SocketRef *_albedoSocket, const Real _etaT);

		Spectrum f(const ScatterEvent &_event) const override;

		Spectrum Sample_f(ScatterEvent &_event, Sampler &_sampler, Real &_pdf) const override;

		Real Pdf(const Vec3 &_wo, const Vec3 &_wi, const ScatterEvent &_event) const override;
};

LAMBDA_END