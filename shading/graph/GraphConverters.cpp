#include "GraphConverters.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Converter {

	SeparateXYZ::SeparateXYZ(Socket *_vector) : Node(1, 3, "Separate XYZ") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _vector, "Vector");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateXYZ::GetX, "x");
		outputSockets[1] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateXYZ::GetY, "y");
		outputSockets[2] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateXYZ::GetZ, "z");
	}

	void SeparateXYZ::GetX(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0]);
		*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Vec3>(_event).x;
	}

	void SeparateXYZ::GetY(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[1]);
		*reinterpret_cast<Real *>(_out) = inputSockets[1].socket->GetAs<Vec3>(_event).y;
	}

	void SeparateXYZ::GetZ(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[2]);
		*reinterpret_cast<Real *>(_out) = inputSockets[2].socket->GetAs<Vec3>(_event).z;
	}



	SeparateRGBA::SeparateRGBA(Socket *_colour) : Node(1, 4, "Separate RGBA") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_COLOUR, _colour, "Colour");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateRGBA::GetR, "r");
		outputSockets[1] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateRGBA::GetG, "g");
		outputSockets[2] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateRGBA::GetB, "b");
		outputSockets[3] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &SeparateRGBA::GetA, "a");
	}

	void SeparateRGBA::GetR(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0]);
		*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Colour>(_event).r;
	}

	void SeparateRGBA::GetG(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0]);
		*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Colour>(_event).g;
	}

	void SeparateRGBA::GetB(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0]);
		*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Colour>(_event).b;
	}

	void SeparateRGBA::GetA(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0]);
		*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Colour>(_event).a;
	}



	MergeXYZ::MergeXYZ(Socket *_x, Socket *_y, Socket *_z) : Node(3, 1, "Merge XYZ") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _x, "x");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _y, "y");
		inputSockets[2] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _z, "z");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_VEC3, &MergeXYZ::GetVec3, "Vector");
	}

	void MergeXYZ::GetVec3(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0] && inputSockets[1] && inputSockets[2]);
		const Real &x = inputSockets[0].socket->GetAs<Real>(_event);
		const Real &y = inputSockets[1].socket->GetAs<Real>(_event);
		const Real &z = inputSockets[2].socket->GetAs<Real>(_event);
		*reinterpret_cast<Vec3 *>(_out) = Vec3(x, y, z);
	}



	MergeRGBA::MergeRGBA(Socket *_r, Socket *_g, Socket *_b, Socket *_a) : Node(4, 1, "Merge RGBA") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _r, "r");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _g, "g");
		inputSockets[2] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _b, "b");
		inputSockets[3] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _a, "a");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, &MergeRGBA::GetColour, "Colour");
	}

	void MergeRGBA::GetColour(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0] && inputSockets[1] && inputSockets[2] && inputSockets[3]);
		const Real &r = inputSockets[0].socket->GetAs<Real>(_event);
		const Real &g = inputSockets[1].socket->GetAs<Real>(_event);
		const Real &b = inputSockets[2].socket->GetAs<Real>(_event);
		const Real &a = inputSockets[3].socket->GetAs<Real>(_event);
		*reinterpret_cast<Colour *>(_out) = Colour(r, g, b, a);
	}

}

SG_END
LAMBDA_END