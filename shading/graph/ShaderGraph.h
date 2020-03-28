#pragma once
#include <string>
#include <memory>
#include <cassert>
#include <utility/Delegate.h>
#include <core/Spectrum.h>
#include "../ScatterEvent.h"
#include <image/Texture.h>



/*
	VERY IMPORTANT macro!
	If enabled, attempted use off null input sockets will throw a runtime error.
	Turn this off for release.
*/
#define SG_USE_ASSERTS

#ifdef SG_USE_ASSERTS
	#define ASSERT_INPUTS(_condition) assert((_condition) && "Attempted use of null inputs.")
#else
	#define ASSERT_INPUTS(_condition)
#endif



#define SG_BEGIN namespace ShaderGraph {
#define SG_END }



LAMBDA_BEGIN

SG_BEGIN



using NodeDelegate = ConstDelegate<void, const ScatterEvent &, void *>;

#define MAKE_SOCKET_CALLBACK(_func) NodeDelegate::FromFunction<std::remove_reference<decltype(*this)>::type, (_func)>(this)

#define MAKE_SOCKET(_type, _callback, _tag) {	(_type), MAKE_SOCKET_CALLBACK(_callback), (_tag)	}

#define MAKE_INPUT_SOCKET(_type, _socketPtr, _tag) {	(_type), (_socketPtr), (_tag)	};



enum class SocketType : uint8_t {
	TYPE_NULL,
	TYPE_SCALAR,
	TYPE_COLOUR,
	TYPE_VEC2,
	TYPE_VEC3,
	TYPE_SPECTRUM,
	TYPE_BXDF
};

template<class T> constexpr SocketType getSocketType = SocketType::TYPE_NULL;
template<> constexpr SocketType getSocketType<Real> = SocketType::TYPE_SCALAR;
template<> constexpr SocketType getSocketType<Vec2> = SocketType::TYPE_VEC2;
template<> constexpr SocketType getSocketType<Vec3> = SocketType::TYPE_VEC3;
template<> constexpr SocketType getSocketType<Colour> = SocketType::TYPE_COLOUR;
template<> constexpr SocketType getSocketType<Spectrum> = SocketType::TYPE_SPECTRUM;

template<class T> constexpr char *getSocketTag = "Null";
template<> constexpr char const *getSocketTag<Real> = "Scalar";
template<> constexpr char const *getSocketTag<Vec2> = "Vector2";
template<> constexpr char const *getSocketTag<Vec3> = "Vector3";
template<> constexpr char const *getSocketTag<Colour> = "RGB";
template<> constexpr char const *getSocketTag<Spectrum> = "Spectrum";



struct Socket {
	SocketType socketType = SocketType::TYPE_NULL;
	NodeDelegate callback;
	std::string tag;

	/*
		Methods kept inline so higher chance of expansion to reduce
		function calls (very frequently called when rendering).
	*/

	template<class T> inline T GetAs(const ScatterEvent &_event) {
		static_assert(false, "GetAs(): Invalid type - type needs specialised GetAs() method.");
	}

	template<> inline Real GetAs<Real>(const ScatterEvent &_event) {
		if (callback && socketType == SocketType::TYPE_SCALAR) {
			Real r;
			callback(_event, &r);
			return r;
		}
		return 0;
	}

	template<> inline Colour GetAs<Colour>(const ScatterEvent &_event) {
		if (callback.operator bool() && socketType == SocketType::TYPE_COLOUR) {
			Colour c;
			callback(_event, &c);
			return c;
		}
		return Colour(1, 0, 1);
	}

	template<> inline Vec2 GetAs<Vec2>(const ScatterEvent &_event) {
		if (callback.operator bool() && socketType == SocketType::TYPE_VEC2) {
			Vec2 v;
			callback(_event, &v);
			return v;
		}
		return Vec2(0, 0);
	}

	template<> inline Vec3 GetAs<Vec3>(const ScatterEvent &_event) {
		if (callback.operator bool() && socketType == SocketType::TYPE_VEC3) {
			Vec3 v;
			callback(_event, &v);
			return v;
		}
		return Vec3(0, 0);
	}

	template<> inline BxDF *GetAs<BxDF *>(const ScatterEvent &_event) {
		if (callback && socketType == SocketType::TYPE_BXDF) {
			BxDF *v = nullptr;
			BxDF **vRef = &v;
			callback(_event, vRef);
			return v;
		}
		return nullptr;
	}

	inline Spectrum GetAsSpectrum(const ScatterEvent &_event, const SpectrumType _type = SpectrumType::Reflectance) {
		const Vec2 uv = maths::Fract(_event.hit->uvCoords);
		switch (socketType) {
		case SocketType::TYPE_COLOUR:
			return Spectrum::FromRGB((Real *) &GetAs<Colour>(_event), _type);
		case SocketType::TYPE_SCALAR:
			return Spectrum(GetAs<Real>(_event));
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
	std::string tag;

	inline operator bool() const {
		return static_cast<bool>(socket);
	}

	void operator=(Socket *_rhs);
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

bool Connect(SocketRef &_socketRef, const Socket &_socket);

void Disconnect(SocketRef &_socketRef);

SG_END

LAMBDA_END