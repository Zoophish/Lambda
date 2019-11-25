#pragma once
#include <string>
#include <memory>
#include <functional>
#include <image/Texture.h>
#include <core/Spectrum.h>
#include "../SurfaceScatterEvent.h"

namespace ShaderGraph {

	#define MAKE_SOCKET_CALLBACK(_func) [this](const SurfaceScatterEvent *_event, void *_out) { _func(_event, _out); }

	#define MAKE_SOCKET(_type, _callback, _tag) {	(_type), MAKE_SOCKET_CALLBACK(_callback), (_tag)	};

	#define MAKE_EMPTY_SOCKET(_type, _tag) {	(_type), nullptr, (_tag)	};

	enum class SocketType {
		TYPE_SCALAR,
		TYPE_COLOUR,
		TYPE_VEC2,
		TYPE_VEC3,
		TYPE_BXDF
	};
	
	struct Socket {
		SocketType socketType;
		std::function<void(const SurfaceScatterEvent *, void *)> callback;
		std::string tag;

		inline Real GetAsScalar(const SurfaceScatterEvent *_event) const {
			if(callback.operator bool() && socketType == SocketType::TYPE_SCALAR) {
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

		inline Spectrum GetAsSpectrum(const SurfaceScatterEvent *_event, const SpectrumType _type = SpectrumType::Reflectance) {
			const Vec2 uv(maths::Fract(_event->hit->uvCoords.x), maths::Fract(_event->hit->uvCoords.y));
			switch (socketType) {
			case ShaderGraph::SocketType::TYPE_COLOUR:
			{
				const Colour c = GetAsColour(_event);
				return Spectrum::FromRGB((Real *)&c, _type);//Remove pointer cast if doesn't work.
			}
			case ShaderGraph::SocketType::TYPE_SCALAR:
				return Spectrum(GetAsScalar(_event));
			default:
				return Spectrum(0);
			}
		}
	};

	class Node {
		public:
			std::string nodeTag;
			unsigned numIn, numOut;
			std::unique_ptr<Socket*[]> inputSockets;
			std::unique_ptr<Socket[]> outputSockets;

			Node(const unsigned _numIn, const unsigned _numOut) {
				numIn = _numIn;
				numOut = _numOut;
				inputSockets.reset(new Socket*[numIn]);
				outputSockets.reset(new Socket[numOut]);
			}
	};

}