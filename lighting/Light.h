#pragma once
#include <core/Spectrum.h>
#include <sampling/Sampler.h>
class Ray;
struct RayHit;
struct SurfaceScatterEvent;

class Light {
	public:
		//Sampled an incident direction light may arrive on.
		virtual Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const = 0;
		
		virtual Real PDF_Li(const SurfaceScatterEvent &_event) const = 0;

		//Returns radiance incoming along a ray that has escaped scene.
		virtual Spectrum Le(const Ray &_r) const {
			return Spectrum(0);
		}

		//Returns emitted radiance at point on surface in outgoing direction wo.
		//Only valid for ray-tracable area lights.
		virtual Spectrum L(const SurfaceScatterEvent &_event) const {
			return Spectrum(0);
		}

		//Importance heuristic functions.
		virtual Real Irradiance() const = 0;
		virtual Real Area() const = 0;
		
		Real Power() const {
			return Irradiance() * Area();
		}

		//Function of relative intensity of emission spectra of ideal blackbody emitter at given temperature T in  Kelvin. (Stefan-Boltzmann)
		static void BlackbodySpectra(const Real *_lambda, int _n, Real _T, Real *_Le) {
			const Real c = 299792458;
			const Real h = 6.62606957e-34;
			const Real kb = 1.3806488e-23;
			for (unsigned i = 0; i < _n; ++i) {
				const Real l = _lambda[i] * 1e-9;
				const Real lambda5 = (l * l) * (l * l) * l;
				_Le[i] = (2 * h * c * c) / (lambda5 * (std::exp((h * c) / (l * kb * _T)) - 1));
			}
		}
};