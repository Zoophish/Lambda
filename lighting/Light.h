#pragma once
#include <core/Spectrum.h>
#include <sampling/Sampler.h>
class Ray;
struct RayHit;
struct SurfaceScatterEvent;

class Light {
	public:
		//TO SELF: Should be specific to EnvironmentLight but is a temporary circular dependancy issue fix.
		Real radius;

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
		
		Real Power() const { return Irradiance() * Area(); }
};