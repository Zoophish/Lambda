#pragma once
#include "GraphNode.h"
#include <image/Texture.h>
#include <shading/SurfaceScatterEvent.h>

namespace ShaderGraph {

	class ScalarInput : public Node {
		public:
			Real value;

			ScalarInput(const Real _value);

			void GetScalar(const SurfaceScatterEvent *_event, void *_out) const;
	};



	class RGBInput : public Node {
		public:
			Colour rgb;

			RGBInput(const Colour &_rgb);
			RGBInput(const Real *_rgb);

			void GetColour(const SurfaceScatterEvent *_event, void *_out) const;
	};



	class ImageTextureInput : public Node {
		public:
			Texture *tex;

			ImageTextureInput(Texture *_tex);

			void GetColour(const SurfaceScatterEvent *_event, void *_out) const;

			void GetScalar(const SurfaceScatterEvent *_event, void *_out) const;
	};
}