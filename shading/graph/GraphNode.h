#pragma once
#include <string>
#include <memory>
class SurfaceScatterEvent;

namespace ShaderGraph {

	#define MAKE_SOCKET_CALLBACK(_func) (void(*)(const SurfaceScatterEvent *, void*))(_func)

	#define MAKE_SOCKET(_type, _callback, _tag) {	(_type), MAKE_SOCKET_CALLBACK(_callback), (_tag)	};

	enum class SocketType {
		TYPE_SCALAR,
		TYPE_COLOUR,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_BXDF
	};

	struct Socket {
		SocketType socketType;
		void(*callback)(const SurfaceScatterEvent *, void *);
		std::string tag;

		Real GetAsScalar(const SurfaceScatterEvent *_event) const {
			if (socketType == SocketType::TYPE_SCALAR) {
				Real r;
				callback(_event, &r);
				return r;
			}
			return 0;
		}

		Colour GetAsColour(const SurfaceScatterEvent *_event) const {
			if (socketType == SocketType::TYPE_COLOUR) {
				Colour c;
				callback(_event, &c);
				return c;
			}
			return Colour(1, 0, 1);
		}
	};

	class Node {
		public:
			std::string nodeTag;
			unsigned numIn, numOut;
			std::unique_ptr<Socket[]> inputSockets, outputSockets;

			Node(const unsigned _numIn, const unsigned _numOut) {
				numIn = _numIn;
				numOut = _numOut;
				inputSockets.reset(new Socket[numIn]);
				outputSockets.reset(new Socket[numOut]);
			}
	};

}