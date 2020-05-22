#pragma once
#include <sampling/Piecewise.h>
#include <shading/ScatterEvent.h>
#include <lighting/Light.h>

LAMBDA_BEGIN

class Scene;

class LightSampler {
	public:
		LightSampler(const Scene *_scene = nullptr);

		/*
			Samples light from distribution.
		*/
		virtual Light *Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const = 0;

		/*
			Returns probability that _light was chosen from distribution, which could be dependent on _event.
		*/
		virtual Real Pdf(const ScatterEvent &_event, const Light *_light) const = 0;

		/*
			Commits with the scene - builds light distribution for scene.
		*/
		virtual void Commit() = 0;

	protected:
		const Scene *scene;
};



class PowerLightSampler : public LightSampler {
	public:
		PowerLightSampler();
		
		PowerLightSampler(const Scene &_scene);

		/*
			Samples light from distribution.
		*/
		Light *Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const override;

		/*
			Returns probability that _light was chosen from distribution.
		*/
		Real Pdf(const ScatterEvent &_event, const Light *_light) const override;

		/*
			Commits with the scene. Builds light distribution for scene.
		*/
		void Commit() override;

	private:
		Distribution::Piecewise1D lightDistribution;
		Real invTotalPower;
};

LAMBDA_END