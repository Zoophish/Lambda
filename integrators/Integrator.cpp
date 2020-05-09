#include "Integrator.h"

LAMBDA_BEGIN

Spectrum Integrator::SampleOneLight(ScatterEvent &_event, const Scene &_scene) const {
	if (_event.hit->object->material->bxdf) {
		Real lightPdf = 1;
		Light *l = _scene.lightSampler->Sample(_event, *sampler, &lightPdf);
		return EstimateDirect(_event, _scene, *l) / lightPdf;
	}
	return Spectrum(0);
}

Spectrum Integrator::EstimateDirect(ScatterEvent &_event, const Scene &_scene, const Light &_light) const {
	Spectrum Ld(0);
	Real scatteringPDF, lightPDF;
	Spectrum f, Li = _light.Sample_Li(_event, sampler, lightPDF);
	if (lightPDF > 0 && !Li.IsBlack()) {
		Spectrum f;
		if (_event.hit->object->material->bxdf) {
			f = _event.hit->object->material->bxdf->f(_event) * std::abs(_event.wiL.y);
			scatteringPDF = _event.hit->object->material->bxdf->Pdf(_event.woL, _event.wiL, _event);
		}
		if (!f.IsBlack() && scatteringPDF > 0) {
			const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
			Ld += Li * f * weight / lightPDF;
		}
	}
	f = _event.hit->object->material->bxdf->Sample_f(_event, *sampler, scatteringPDF);
	f *= std::abs(_event.wiL.y);
	if (scatteringPDF > 0 && !f.IsBlack()) {
		Real weight = 1;
		Spectrum Li(0);
		lightPDF = _light.PDF_Li(_event, *sampler);
		if (lightPDF == 0) {
			return Ld;
		}
		weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
		ScatterEvent bsdfIntersect;
		bsdfIntersect.scene = &_scene;
		RayHit lightHit;
		bsdfIntersect.hit = &lightHit;
		bsdfIntersect.wo = -_event.wi;
		Ray r(_event.hit->point + _event.hit->normalG * .00001, _event.wi);
		if (_scene.Intersect(r, lightHit)) {
			if (bsdfIntersect.hit->object->material->light == &_light)
				Li = bsdfIntersect.hit->object->material->light->L(bsdfIntersect);
		}
		else {
			Li = _light.Le(r);
		}
		if (!Li.IsBlack()) Ld += f * Li * weight / scatteringPDF;
	}
	return Ld;
}

LAMBDA_END