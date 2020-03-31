#include <random>
#include "ShaderGraph.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Textures {

	class PerlinNoise : public Node {
		public:
			Real scale;

			PerlinNoise(const Real _scale);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			Real Get2D(Vec2 _texCoords) const;

			Real Get3D(Vec3 _point) const;
	};



	class Checker : public Node {
		public:
			Real scale;
		
			Checker(const Real _scale);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			inline Real Get2D(Vec2 _texCoords) const;

			inline Real Get3D(Vec3 _point) const;
	};

}

SG_END
LAMBDA_END