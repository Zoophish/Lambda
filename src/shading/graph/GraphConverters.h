#pragma once
#include "ShaderGraph.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Converter {

	class SeparateXYZ : public Node {
		public:
			SeparateXYZ(Socket *_vector);

			void GetX(const ScatterEvent &_event, void *_out) const;

			void GetY(const ScatterEvent &_event, void *_out) const;

			void GetZ(const ScatterEvent &_event, void *_out) const;
	};



	class SeparateRGBA : public Node {
		public:
			SeparateRGBA(Socket *_colour);

			void GetR(const ScatterEvent &_event, void *_out) const;

			void GetG(const ScatterEvent &_event, void *_out) const;

			void GetB(const ScatterEvent &_event, void *_out) const;

			void GetA(const ScatterEvent &_event, void *_out) const;
	};



	class MergeXYZ : public Node {
		public:
			MergeXYZ(Socket *_x, Socket *_y, Socket *_z);

			void GetVec3(const ScatterEvent &_event, void *_out) const;
	};



	class MergeRGBA : public Node {
		public:
			MergeRGBA(Socket *_r, Socket *_g, Socket *_b, Socket *_a);

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