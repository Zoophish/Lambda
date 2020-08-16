#pragma once
#include <core/Spectrum.h>
#include <sampling/Sampler.h>

LAMBDA_BEGIN

class Ray;
struct RayHit;
struct ScatterEvent;
class Scene;

class Light {
	public:

		/*
			Samples an incident direction light may arrive on.
		*/
		virtual Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const = 0;
		
		/*
			Pdf of given _event. _sampler is included incase volumes are present.
		*/
		virtual Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const = 0;

		/*
			Pdf ray intersection under _event, given that the intersection hit THIS light (except for infinite lights).
		*/
		virtual Real PDF_Li(const ScatterEvent &_event) const = 0;

		/*
			Returns radiance incoming along a ray that has escaped scene.
		*/
		virtual Spectrum Le(const Ray &_r) const {
			return Spectrum(0);
		}

		/*
			Assuming there is mutual visibilty between the previous shading point
			and new one that is stored in event, returns incoming radiance.
		*/
		virtual Spectrum L(const ScatterEvent &_event) const {
			return Spectrum(0);
		}

		/*
			Power per square unit.
		*/
		virtual Real Irradiance() const = 0;

		/*
			Surface area of light.
		*/
		virtual Real Area() const = 0;
		
		/*
			Total power of light.
		*/
		Real Power() const {
			return Irradiance() * Area();
		}

		/*
			Get bounds of light in world.
		*/
		virtual Bounds GetBounds() const = 0;

		/*
			Get main direction of light.
		*/
		virtual Vec3 GetDirection() const = 0;

		/*
			Special scene intersection functions for direct lighting that account for volumetric
			beam transmittance.
			_scene.hasVolumes must be true to account for beam transmittance, _Tr.

			Returns true if there is mutual visibility between _p1 and _p2
		*/
		static bool MutualVisibility(const Vec3 &_p1, const Vec3 &_p2, ScatterEvent &_event, const Scene &_scene, Sampler &_sampler, Spectrum *_Tr = nullptr);

		/*
			Returns true if ray, _r, escapes the scene.
		*/
		static bool RayEscapes(const Ray &_r, const ScatterEvent &_event, Sampler &_sampler, Spectrum *_Tr = nullptr);
};

LAMBDA_END