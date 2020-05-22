#pragma once
#include "ShaderGraph.h"

LAMBDA_BEGIN
SG_BEGIN

namespace Maths {

	enum class ScalarOperatorType {
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
		SQRT,
		POWER,
	};

	class ScalarMaths : public Node {
		public:
			ScalarOperatorType operatorType;
	
			ScalarMaths(ScalarOperatorType _operatorType = ScalarOperatorType::ADD, Socket *_valueA = nullptr, Socket *_valueB = nullptr);
	
			void GetScalar(const ScatterEvent &_event, void *_out) const;
	};



	enum class VectorOperatorType {
		ADD,
		SUBTRACT,
		MULTIPLY,
		DIVIDE,
	};

	class VectorMaths : public Node {
		public:
			VectorOperatorType operatorType;

			VectorMaths(VectorOperatorType _operatorType = VectorOperatorType::ADD, Socket *_valueA = nullptr, Socket *_valueB = nullptr);

			void GetVec3(const ScatterEvent &_event, void *_out) const;
	};



	class DotProductNode : public Node {
		public:
			DotProductNode(Socket *_valueA = nullptr, Socket *_valueB = nullptr);

			void GetScalar(const ScatterEvent &_event, void *_out) const;
	};



	class CrossProductNode : public Node {
		public:
			CrossProductNode(Socket *_valueA = nullptr, Socket *_valueB = nullptr);

			void GetVec3(const ScatterEvent &_event, void *_out) const;
	};



	class VectorLength : public Node {
		public:
			VectorLength(Socket *_socket = nullptr);

			void GetScalar(const ScatterEvent &_event, void *_out) const;

	};

}

SG_END
LAMBDA_END