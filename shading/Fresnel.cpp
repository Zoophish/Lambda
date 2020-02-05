#include "Fresnel.h"

LAMBDA_BEGIN

Real Fresnel::FrDielectric(Real _cosThetaI, Real _etaI, Real _etaT) {
	_cosThetaI = maths::Clamp(_cosThetaI, (Real)-1, (Real)1);
	if (_cosThetaI < 0.) {
		std::swap(_etaI, _etaT);
		_cosThetaI = std::abs(_cosThetaI);
	}
	const Real sinThetaI = std::sqrt(std::max((Real)0, 1 - _cosThetaI * _cosThetaI));
	const Real sinThetaT = _etaI / _etaT * sinThetaI;
	if (sinThetaT >= 1) {
		return 1;
	}
	const Real cosThetaT = std::sqrt(std::max((Real)0, 1 - sinThetaT * sinThetaT));
	const Real rPara = ((_etaT * _cosThetaI) - (_etaI * cosThetaT)) / ((_etaT * _cosThetaI) + (_etaI * cosThetaT));
	const Real rPerp = ((_etaI * _cosThetaI) - (_etaT * cosThetaT)) / ((_etaI * _cosThetaI) + (_etaT * cosThetaT));
	return (rPara * rPara + rPerp * rPerp) * .5;
}

//Note to self: Could do with optimising.
Spectrum Fresnel::FrConductor(Real _cosThetaI, const Spectrum &_etaI, const Spectrum &_etaT, const Spectrum &_k) {
	_cosThetaI = maths::Clamp(_cosThetaI, (Real)-1, (Real)1);
	const Spectrum eta = _etaT / _etaI;
	const Spectrum etak = _k / _etaI;

	const Real cosThetaI2 = _cosThetaI * _cosThetaI;
	const Real sinThetaI2 = (Real)1 - cosThetaI2;
	const Spectrum eta2 = eta * eta;
	const Spectrum etak2 = etak * etak;

	const Spectrum t0 = eta2 - etak2 - sinThetaI2;
	const Spectrum a2plusb2 = Spectrum::Sqrt(t0 * t0 + eta2 * (Real)4 * etak2);
	const Spectrum t1 = a2plusb2 + cosThetaI2;
	const Spectrum a = Spectrum::Sqrt((a2plusb2 + t0) * (Real).5);
	const Spectrum t2 = a * _cosThetaI * (Real)2;
	const Spectrum rS = (t1 - t2) / (t1 + t2);

	const Spectrum t3 = a2plusb2 * cosThetaI2 + sinThetaI2 * sinThetaI2;
	const Spectrum t4 = t2 * sinThetaI2;
	const Spectrum rP = rS * (t3 - t4) / (t3 + t4);

	return (rP + rS) * (Real).5;
}



FresnelDielectric::FresnelDielectric() {}

FresnelDielectric::FresnelDielectric(const Real _etaT) {
	etaT = _etaT;
}

Spectrum FresnelDielectric::Evaluate(const Real _cosThetaI, const Real _etaI) const {
	return Fresnel::FrDielectric(_cosThetaI, etaT, _etaI);
}



FresnelConductor::FresnelConductor(const Spectrum &_etaI, const Spectrum &_k) {
	etaI = _etaI;
	k = _k;
}

Spectrum FresnelConductor::Evaluate(const Real _cosThetaI, const Real _etaT) const {
	return FrConductor(_cosThetaI, etaI, _etaT, k);
}

LAMBDA_END