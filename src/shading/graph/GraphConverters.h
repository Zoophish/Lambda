#pragma once
#include "ShaderGraph.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Converter {

	class SeparateXYZ : public Node {
		public:
			SeparateXYZ(Socket *_vector = nullptr);

			void GetX(const ScatterEvent &_event, void *_out) const;

			void GetY(const ScatterEvent &_event, void *_out) const;

			void GetZ(const ScatterEvent &_event, void *_out) const;
	};



	class SeparateRGBA : public Node {
		public:
			SeparateRGBA(Socket *_colour = nullptr);

			void GetR(const ScatterEvent &_event, void *_out) const;

			void GetG(const ScatterEvent &_event, void *_out) const;

			void GetB(const ScatterEvent &_event, void *_out) const;

			void GetA(const ScatterEvent &_event, void *_out) const;
	};



	class MergeXYZ : public Node {
		public:
			MergeXYZ(Socket *_x = nullptr, Socket *_y = nullptr, Socket *_z = nullptr);

			void GetVec3(const ScatterEvent &_event, void *_out) const;
	};



	class MergeRGBA : public Node {
		public:
			MergeRGBA(Socket *_r = nullptr, Socket *_g = nullptr, Socket *_b = nullptr, Socket *_a = nullptr);

			void GetColour(const ScatterEvent &_event, void *_out) const;
	};



	class ScalarToColour : public Node {
		public:
			ScalarToColour(Socket *_scalar = nullptr);

			void GetScalar(const ScatterEvent &_event, void *_out) const;
	};

}

SG_END
LAMBDA_END