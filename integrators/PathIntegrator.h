#pragma once
#include "Integrator.h"

class PathIntegrator : public Integrator {
	public:
		unsigned maxBounces, minBounces;

		PathIntegrator(Sampler *_sampler, const unsigned _maxBounces = 128, const unsigned _minBounces = 3) {
			maxBounces = _maxBounces;
			minBounces = _minBounces;
			sampler = _sampler;
		}

		Integrator *clone() const override { return new PathIntegrator(*this); }

		Spectrum Li(const Ray &_r, const Scene &_scene) const override {
			Spectrum L(0), beta(1);
			Ray r = _r;
			RayHit hit;
			SurfaceScatterEvent event;
			event.hit = &hit;
			event.scene = &_scene;
			event.wo = -r.d;
			bool scatterIntersect = false;
			for (unsigned bounces = 0; bounces < maxBounces; ++bounces) {
				if (bounces == 0 ? _scene.Intersect(r, hit) : scatterIntersect) {
					event.pdf = 1;

					if (bounces == 0 && hit.object->light) {
						L += hit.object->light->L(event);
					}

					if (hit.object->bxdf) {
						event.Localise();
						event.wo = -r.d;
						Real lightDistPdf = 1;
						const Light *l = _scene.lights[_scene.lightDistribution.SampleDiscrete(sampler->Get1D(), &lightDistPdf)];
						Spectrum Ld(0);
						Real scatteringPDF, lightPDF;
						Spectrum f, Li = l->Sample_Li(event, sampler, lightPDF);
						if (lightPDF > 0 && !Li.IsBlack()) {	//Add light sample contribution
							f = hit.object->bxdf->f(event) * std::abs(event.wiL.y);
							scatteringPDF = hit.object->bxdf->Pdf(event.woL, event.wiL);
							if (!f.IsBlack() && scatteringPDF > 0) {
								const Real weight = PowerHeuristic(1, lightPDF, 1, scatteringPDF);
								Ld += Li * f * weight / lightPDF;
							}
						}
						f = hit.object->bxdf->Sample_f(event, sampler->Get2D(), scatteringPDF);
						f *= std::abs(event.wiL.y);	//This must follow previous line to allow computation of wiL.
						lightPDF = l->PDF_Li(event);	//Evaluated before hit is altered to next path vertex
						r.o = hit.point;
						r.d = event.wi;
						scatterIntersect = _scene.Intersect(r, hit);	//Go to next path vertex
						if (scatteringPDF > 0 && !f.IsBlack()) {	//Add bsdf-scatter light contribution
							if (lightPDF != 0) {
								Li = Spectrum(0);
								const Real weight = PowerHeuristic(1, scatteringPDF, 1, lightPDF);
								if (scatterIntersect) {
									if (hit.object->light == l)
										Li = hit.object->light->L(event);
								}
								else {
									Li = l->Le(r);
								}
								if (!Li.IsBlack()) Ld += Li * f * weight / scatteringPDF;
							}
						}
						else break;	//Don't continue path bsdf is zero or if zero scattering pdf

						L += beta * (Ld / lightDistPdf);
						beta *= f / event.pdf;
					}
					else {
						r.o = event.hit->point + r.d * .0005;
						continue;
					}

					if (bounces > minBounces) {
						const Real q = std::max((Real).05, 1 - beta.y());
						if (sampler->Get1D() < q) break;
						beta /= (Real)1 - q;
					}
				}
				else {
					L += beta * _scene.envLight->Le(r);
					break;
				}
			}
			return L;
		}
};