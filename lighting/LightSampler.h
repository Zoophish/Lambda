#pragma once
#include <sampling/Piecewise.h>
#include <shading/ScatterEvent.h>
#include <lighting/Light.h>

LAMBDA_BEGIN

class Scene;

class LightSampler {
	public:
		LightSampler(const Scene *_scene = nullptr);

		virtual Light *Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const = 0;

		virtual void Commit() = 0;

	protected:
		const Scene *scene;
};



class PowerLightSampler : public LightSampler {
	public:
		PowerLightSampler();
		
		/*
			Construct sampler using _scene's lights.
		*/
		PowerLightSampler(const Scene &_scene);

		/*
			Samples light from distribution.
		*/
		Light *Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const override;

		/*
			Called when scene is comitted
		*/
		void Commit() override;

	private:
		Distribution::Piecewise1D lightDistribution;
};

LAMBDA_END