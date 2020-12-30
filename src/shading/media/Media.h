#pragma once
#include <core/Spectrum.h>
#include "PhaseFunction.h"
#include "../ScatterEvent.h"

LAMBDA_BEGIN

class Medium {
	public:
		PhaseFunction *phase;

		/*
			Beam transmittance _tFar along _ray.
		*/
		virtual Spectrum Tr(const Ray &_ray, const Real _tFar, Sampler &_sampler) const = 0;

		/*
			Sample the ray segment proportionally to the beam transmittance along the ray.
			Favours points closer towards the ray origin (due to higher beam transmittance).
			May produce a surface interaction rather than a medium interaction.
		*/
		virtual Spectrum SampleDistance(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event, Real *_t = nullptr, Real *_pdf = nullptr) const = 0;
		
		/*
			
		*/
		virtual Real PDFDistance(const Real _t) const = 0;

		/*
			Sample the ray proportionally to _light's contribution at that point.
			May produce a surface interaction rather than a medium interaction.
		*/
		virtual Spectrum SampleEquiangular(const Ray &_ray, Sampler &_sampler, ScatterEvent &_event, const Vec3 &_lightPoint, Real *_t = nullptr, Real *_pdf = nullptr) const = 0;

		/*
			
		*/
		virtual Real PDFEquiangular(const Ray &_ray, const Vec3 &_lightPoint, const Real _tFar, const Real _t) const = 0;
};

class MediaBoundary {
	public:
		Medium *interior, *exterior;	//exterior does not adhere to closed mesh rule... should always be nullptr

		MediaBoundary(Medium *_interior = nullptr, Medium *_exterior = nullptr) {
			interior = _interior;
			exterior = _exterior;
		}

		/*
			Returns the medium that direction _w is 'going into' at the boundary.
		*/
		inline Medium *GetMedium(const Vec3 &_w, const Vec3 &_n) const {
			return maths::Dot(_w, _n) < 0 ? interior : exterior;
		}
};

LAMBDA_END