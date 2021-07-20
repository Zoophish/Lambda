#pragma once
#include <core/Spectrum.h>
#include <sampling/Sampler.h>


LAMBDA_BEGIN

class Ray;
struct RayHit;
struct ScatterEvent;
class Scene;
struct PartialLightSample;

class Light {
	public:

		/*
			Samples an incident direction from light to shading point.
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
			Samples a point on the light (ideally proportionally to influence on shading point).
		*/
		virtual Spectrum SamplePoint(Sampler &_sampler, ScatterEvent &_event, PartialLightSample *_ls) const = 0;

		/*
			Visibility term between shading point and point on this light. Also completes the pdf.
		*/
		virtual Spectrum Visibility(const Vec3 &_shadingPoint, ScatterEvent &_event, Sampler &_sampler, PartialLightSample *_ls) const = 0;

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
			Power per unit area.
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
			- Returns true if there is mutual visibility between _p1 and _p2
			- _scene.hasVolumes must be true to account for beam transmittance, _Tr.
			- Conveniently stores the new wi direction in _event to prevent duplicate computation
		*/
		static bool MutualVisibility(const Vec3 &_p1, const Vec3 &_p2, ScatterEvent &_event, const Scene &_scene, Sampler &_sampler, Spectrum *_Tr = nullptr);

		/*
			Accounts for correct transmittance between points that aren't intersectable.
		*/
		static bool PointMutualVisibility(const Vec3 &_p1, const Vec3 &_p2, ScatterEvent &_event, const Scene &_scene, Sampler &_sampler, Spectrum *_Tr = nullptr);

		/*
			Returns true if ray, _r, escapes the scene.
		*/
		static bool RayEscapes(const Ray &_r, const ScatterEvent &_event, Sampler &_sampler, Spectrum *_Tr = nullptr);
};

/*
	For sampling a point on a light, then completing the full contribution and pdf terms to a shading point at some later stage.
*/
struct PartialLightSample {
	Vec3 point, normal;
	Spectrum Le;
	Light *light;
	Real pdf;
};

LAMBDA_END