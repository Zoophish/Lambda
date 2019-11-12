#pragma once
#include <core/Spectrum.h>

class Fresnel {
public:
	virtual Spectrum Evaluate(const Real _cosThetaI, const Real _etaT) const = 0;

	static Real FrDielectric(Real _cosThetaI, Real _etaI, Real _etaT);

	static Spectrum FrConductor(Real _cosThetaI, const Spectrum &_etaI, const Spectrum &_etaT, const Spectrum &_k);
};



class FresnelDielectric : public Fresnel {
public:
	Real etaT;

	FresnelDielectric();

	FresnelDielectric(const Real _etaT);

	Spectrum Evaluate(const Real _cosThetaI, const Real _etaI) const override;
};



class FresnelConductor : public Fresnel {
public:
	Spectrum etaI, k;

	FresnelConductor(const Spectrum &_etaI, const Spectrum &_k);

	Spectrum Evaluate(const Real _cosThetaI, const Real _etaT) const override;
};