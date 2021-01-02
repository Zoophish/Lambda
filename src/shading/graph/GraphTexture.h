#include <random>
#include "ShaderGraph.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Textures {

	class Noise {
		public:
			Real scale;

			virtual Real Get2D(Vec2 _texCoords) const = 0;

			virtual Real Get3D(Vec3 _point) const = 0;
	};

	class PerlinNoise : public Node, public Noise {
		public:
			PerlinNoise(const Real _scale = 1);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			Real Get2D(Vec2 _texCoords) const override;

			Real Get3D(Vec3 _point) const override;
	};



	class Checker : public Node, public Noise {
		public:
			//Real scale;
		
			Checker(const Real _scale = 1);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			Real Get2D(Vec2 _texCoords) const override;

			Real Get3D(Vec3 _point) const override;
	};



	class ValueNoise : public Node, public Noise {
		public:
			//Real scale;

			ValueNoise(const Real _scale = 1);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			Real Get2D(Vec2 _texCoords) const override;

			Real Get3D(Vec3 _point) const override;
	};



	class OctaveNoise : public Node, public Noise {
		public:
			//Real lacuranity;
			unsigned octaves;
			Noise *noise = nullptr;

			OctaveNoise(const Real _scale = 1, const unsigned _octaves = 3);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			Real Get2D(Vec2 _texCoords) const override;

			Real Get3D(Vec3 _point) const override;
	};



	class Voronoi : public Node, public Noise {
		public:
			Voronoi(const Real _scale = 1);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

			Real Get2D(Vec2 _texCoords) const override;

			Real Get3D(Vec3 _point) const override;
	};



	class BackgroundNode : public Node {
		public:
			BackgroundNode(Socket *_image = nullptr, Socket *_offset = nullptr);

			void GetColour(const ScatterEvent &_event, void *_out) const;
	};

}

SG_END
LAMBDA_END