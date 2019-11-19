#pragma once
#include "GraphNode.h"
#include <image/Texture.h>
#include <shading/SurfaceScatterEvent.h>

namespace ShaderGraph {

	class RGBInput : public Node {
		public:
			Colour rgb;

			RGBInput(const Colour &_rgb);

			void GetColour(const SurfaceScatterEvent *_event, void *_out) const;
	};

	class TextureInput : public Node {
		public:
			Texture *tex;

			TextureInput(Texture *_tex);

			void GetColour(const SurfaceScatterEvent *_event, void *_out) const;

			void GetScalar(const SurfaceScatterEvent *_event, void *_out) const;
	};
}