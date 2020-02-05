#pragma once
#include <string>
#include <memory>
#include <utility/Delegate.h>
#include <core/Spectrum.h>
#include "../SurfaceScatterEvent.h"
#include <image/Texture.h>

LAMBDA_BEGIN

namespace ShaderGraph {

	using NodeDelegate = ConstDelegate<void, const SurfaceScatterEvent *, void *>;

	#define MAKE_SOCKET_CALLBACK(_func) NodeDelegate::FromFunction<std::remove_reference<decltype(*this)>::type, (_func)>(this)

	#define MAKE_SOCKET(_type, _callback, _tag) {	(_type), MAKE_SOCKET_CALLBACK(_callback), (_tag)	}

	#define MAKE_INPUT_SOCKET(_type, _socketPtr) {	(_type), (_socketPtr)	};

	enum class SocketType : uint8_t {
		TYPE_SCALAR,
		TYPE_COLOUR,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_SPECTRUM,
		TYPE_BXDF
	};
	
	struct Socket {
		SocketType socketType;
		NodeDelegate callback;
		std::string tag;

		inline Real GetAsScalar(const SurfaceScatterEvent *_event) const {
			if(callback && socketType == SocketType::TYPE_SCALAR) {
				Real r;
				callback(_event, &r);
				return r;
			}
			return 0;
		}

		inline Colour GetAsColour(const SurfaceScatterEvent *_event) const {
			if (callback.operator bool() && socketType == SocketType::TYPE_COLOUR) {
				Colour c;
				callback(_event, &c);
				return c;
			}
			return Colour(1, 0, 1);
		}

		inline Vec2 GetAsVec2(const SurfaceScatterEvent *_event) const {
			if (callback.operator bool() && socketType == SocketType::TYPE_VEC2) {
				Vec2 v;
				callback(_event, &v);
				return v;
			}
			return Vec2(0, 0);
		}

		inline BxDF* GetAsBxDF(const SurfaceScatterEvent *_event) const {
			if (callback && socketType == SocketType::TYPE_BXDF) {
				BxDF *v = nullptr;
				BxDF **vRef = &v;
				callback(_event, vRef);
				return v;
			}
			return nullptr;
		}

		inline Spectrum GetAsSpectrum(const SurfaceScatterEvent *_event, const SpectrumType _type = SpectrumType::Reflectance) {
			const Vec2 uv(maths::Fract(_event->hit->uvCoords.x), maths::Fract(_event->hit->uvCoords.y));
			switch (socketType) {
			case SocketType::TYPE_COLOUR:
				return Spectrum::FromRGB((Real *) &GetAsColour(_event), _type);
			case SocketType::TYPE_SCALAR:
				return Spectrum(GetAsScalar(_event));
			case SocketType::TYPE_SPECTRUM:
			{
				if (callback.operator bool() && socketType == SocketType::TYPE_SPECTRUM) {
					Spectrum out;
					callback(_event, &out);
					return out;
				}
				return Spectrum(0);
			}
			default:
				return Spectrum(0);
			}
		}
	};

	struct SocketRef {
		SocketType socketType;
		Socket *socket;

		void operator=(Socket *_rhs) { 
			if (_rhs->socketType == socketType)
				socket = _rhs;
			else
				throw std::runtime_error("Socket types do not match.");
		}
	};

	class Node {
		public:
			const std::string nodeTag;
			const unsigned numIn, numOut;
			const std::unique_ptr<SocketRef[]> inputSockets;
			const std::unique_ptr<Socket[]> outputSockets;

			Node(const unsigned _numIn, const unsigned _numOut, const std::string &_nodeTag = "node tag") :
				nodeTag(_nodeTag),
				numIn(_numIn), numOut(_numOut),
				inputSockets(new SocketRef[_numIn]),
				outputSockets(new Socket[_numOut]) {}

	};

	static bool Connect(SocketRef &_socketRef, const Socket &_socket) {
		if (_socketRef.socketType == _socket.socketType) {
			_socketRef.socket = (Socket*)&_socket;
			return true;
		}
		return false;
	}

	static void Disconnect(SocketRef &_socketRef) {
		_socketRef.socket = nullptr;
	}
}

LAMBDA_END