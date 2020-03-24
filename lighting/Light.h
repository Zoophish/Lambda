#pragma once
#include <core/Spectrum.h>
#include <sampling/Sampler.h>

LAMBDA_BEGIN

class Ray;
struct RayHit;
struct ScatterEvent;

class Light {
	public:
		//TO SELF: Should be specific to EnvironmentLight but is a temporary circular dependancy issue fix.
		Real radius;

		/*
			Samples an incident direction light may arrive on.
		*/
		virtual Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const = 0;
		
		/*
			Pdf of given _event. _sampler is included incase volumes are present.
		*/
		virtual Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const = 0;

		/*
			Returns radiance incoming along a ray that has escaped scene.
		*/
		virtual Spectrum Le(const Ray &_r) const {
			return Spectrum(0);
		}

		/*
			Returns emitted radiance at point on surface in outgoing direction wo.
			Only valid for ray-tracable area lights.
		*/
		virtual Spectrum L(const ScatterEvent &_event) const {
			return Spectrum(0);
		}

		/*
			Importance heuristics.
		*/
		virtual Real Irradiance() const = 0;
		virtual Real Area() const = 0;
		
		Real Power() const {
			return Irradiance() * Area();
		}
};

LAMBDA_END