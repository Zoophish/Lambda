#pragma once
#include <core/Spectrum.h>
class Ray;
struct RayHit;
struct SurfaceScatterEvent;

class Light {
	public:

		//Sampled an incident direction light may arrive on.
		virtual Spectrum Sample_Li(SurfaceScatterEvent &_event, const Vec2 &_u, Real &_pdf) const = 0;
		
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

		//virtual Real Power() const;

		static void Blackbody(const Real *lambda, int n, Real T, Real *Le) {
			const Real c = 299792458;
			const Real h = 6.62606957e-34;
			const Real kb = 1.3806488e-23;
			for (int i = 0; i < n; ++i) {
				const Real l = lambda[i] * 1e-9;
				const Real lambda5 = (l * l) * (l * l) * l;
				Le[i] = (2 * h * c * c) / (lambda5 * (std::exp((h * c) / (l * kb * T)) - 1));
			}
		}
};