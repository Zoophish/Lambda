#include "GraphMaths.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Maths {

	ScalarMaths::ScalarMaths(ScalarOperatorType _operatorType, Socket *_valueA, Socket *_valueB) : Node(2, 1, "Maths") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _valueA, "Scalar");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _valueB, "Scalar");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &ScalarMaths::GetScalar, "Scalar");
		operatorType = _operatorType;
	}

	void ScalarMaths::GetScalar(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0] && inputSockets[1]);
		switch (operatorType) {
		case ScalarOperatorType::ADD:
			*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Real>(_event) + inputSockets[1].socket->GetAs<Real>(_event);
			break;
		case ScalarOperatorType::SUBTRACT:
			*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Real>(_event) - inputSockets[1].socket->GetAs<Real>(_event);
			break;
		case ScalarOperatorType::MULTIPLY:
			*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Real>(_event) * inputSockets[1].socket->GetAs<Real>(_event);
			break;
		case ScalarOperatorType::DIVIDE:
			*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Real>(_event) / inputSockets[1].socket->GetAs<Real>(_event);
			break;
		}
	}



	VectorMaths::VectorMaths(VectorOperatorType _operatorType, Socket *_valueA, Socket *_valueB) : Node(2, 1, "Vector Maths") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _valueA, "Vector");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _valueB, "Vector");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_VEC3, &VectorMaths::GetVec3, "Vector");
		operatorType = _operatorType;
	}

	void VectorMaths::GetVec3(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0] && inputSockets[1]);
		switch (operatorType) {
		case VectorOperatorType::ADD:
			*reinterpret_cast<Vec3 *>(_out) = inputSockets[0].socket->GetAs<Vec3>(_event) + inputSockets[1].socket->GetAs<Vec3>(_event);
			break;
		case VectorOperatorType::SUBTRACT:
			*reinterpret_cast<Vec3 *>(_out) = inputSockets[0].socket->GetAs<Vec3>(_event) - inputSockets[1].socket->GetAs<Vec3>(_event);
			break;
		case VectorOperatorType::MULTIPLY:
			*reinterpret_cast<Vec3 *>(_out) = inputSockets[0].socket->GetAs<Vec3>(_event) * inputSockets[1].socket->GetAs<Vec3>(_event);
			break;
		case VectorOperatorType::DIVIDE:
			*reinterpret_cast<Vec3 *>(_out) = inputSockets[0].socket->GetAs<Vec3>(_event) / inputSockets[1].socket->GetAs<Vec3>(_event);
			break;
		}
	}



	DotProductNode::DotProductNode(Socket *_valueA, Socket *_valueB) : Node(2, 1, "Dot") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _valueA, "Vector");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _valueB, "Vector");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &DotProductNode::GetScalar, "Dot");
	}

	void DotProductNode::GetScalar(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0] && inputSockets[1]);
		*reinterpret_cast<Real *>(_out) = maths::Dot(inputSockets[0].socket->GetAs<Vec3>(_event), inputSockets[1].socket->GetAs<Vec3>(_event));
	}



	CrossProductNode::CrossProductNode(Socket *_valueA, Socket *_valueB) : Node(2, 1, "Cross") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _valueA, "Vector");
		inputSockets[1] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _valueB, "Vector");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_VEC3, &CrossProductNode::GetVec3, "Vector");
	}

	void CrossProductNode::GetVec3(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0] && inputSockets[1]);
		*reinterpret_cast<Vec3 *>(_out) = maths::Cross(inputSockets[0].socket->GetAs<Vec3>(_event), inputSockets[1].socket->GetAs<Vec3>(_event));
	}


	VectorLength::VectorLength(Socket *_socket) : Node(1, 1, "Length") {
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_VEC3, _socket, "Vector");
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &VectorLength::GetScalar, "Length");
	}

	void VectorLength::GetScalar(const ScatterEvent &_event, void *_out) const {
		ASSERT_INPUTS(inputSockets[0]);
		*reinterpret_cast<Real *>(_out) = inputSockets[0].socket->GetAs<Vec3>(_event).Magnitude();
	}

}

SG_END
LAMBDA_END