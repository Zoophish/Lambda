#pragma once
#include <string>
#include <memory>
#include <cassert>
#include <stack>
#include <utility/Delegate.h>
#include <core/Spectrum.h>
#include "../ScatterEvent.h"
#include <image/Texture.h>


/*
	Check for null sockets or empty callbacks.
*/
#ifdef _DEBUG
	#define ASSERT_INPUTS(_condition) assert((_condition) && "Attempted use of null inputs.")
	#define ASSERT_CALLBACK(_callback) assert((_callback) && "Bad node - no callback")
#else
	#define ASSERT_INPUTS(_condition)
	#define ASSERT_CALLBACK(_callback)
#endif



#define SG_BEGIN namespace ShaderGraph {
#define SG_END }



LAMBDA_BEGIN

SG_BEGIN



using NodeDelegate = ConstDelegate<void, const ScatterEvent &, void *>;

#define MAKE_SOCKET_CALLBACK(_func) NodeDelegate::FromFunction<std::remove_reference<decltype(*this)>::type, (_func)>(this)

#define MAKE_SOCKET(_type, _callback, _tag) {	(_type), MAKE_SOCKET_CALLBACK(_callback), nullptr, (_tag)	}

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

class Node;

struct Socket {
	SocketType socketType = SocketType::TYPE_NULL;
	NodeDelegate callback;
	Node *node = nullptr;
	std::string tag;

	/*
		Methods are kept inline for higher chance of expansion.
	*/

	template<class T> inline T GetAs(const ScatterEvent &_event) {
		static_assert(false, "GetAs(): Invalid template - not specialised.");
	}

	/*
		A to B conversions
	*/

	template<> inline Real GetAs<Real>(const ScatterEvent &_event) {
		ASSERT_CALLBACK(callback);
		switch (socketType) {
		case SocketType::TYPE_SCALAR:
		{
			Real r;
			callback(_event, &r);
			return r;
		}
		case SocketType::TYPE_COLOUR:
		{
			Colour c;
			callback(_event, &c);
			return c.r * (Real).3 + c.g * (Real).59 + c.b * (Real).11;
		}
		case SocketType::TYPE_VEC2:
		{
			Vec2 v;
			callback(_event, &v);
			return (v.x + v.y) * (Real).5;
		}
		case SocketType::TYPE_VEC3:
		{
			Vec3 v;
			callback(_event, &v);
			return (v.x + v.y + v.z) * (Real).333333333333;
		}
		case SocketType::TYPE_SPECTRUM:
		{
			Spectrum s;
			callback(_event, &s);
			return s.y();
		}
		default:
			return 0;
		}
		return 0;
	}

	template<> inline Colour GetAs<Colour>(const ScatterEvent &_event) {
		ASSERT_CALLBACK(callback);
		switch(socketType) {
		case SocketType::TYPE_COLOUR:
		{
			Colour c;
			callback(_event, &c);
			return c;
		}
		case SocketType::TYPE_SCALAR:
		{
			Real r;
			callback(_event, &r);
			return Colour(r);
		}
		case SocketType::TYPE_SPECTRUM:
		{
			Spectrum s;
			callback(_event, &s);
			Colour c;
			s.ToRGB(&c.r);
			return c;
		}
		case SocketType::TYPE_VEC3:
		{
			Vec3 v;
			callback(_event, &v);
			return Colour(v.x, v.y, v.z);
		}
		case SocketType::TYPE_VEC2:
		{
			Vec2 v;
			callback(_event, &v);
			return Colour((v.x + v.y) * (Real).5);
		}
		default:
			return Colour(1, 0, 1);
			}
	}

	template<> inline Vec2 GetAs<Vec2>(const ScatterEvent &_event) {
		ASSERT_CALLBACK(callback);
		switch (socketType) {
		case SocketType::TYPE_VEC2:
		{
			Vec2 v;
			callback(_event, &v);
			return v;
		}
		case SocketType::TYPE_VEC3:
		{
			Vec3 v;
			callback(_event, &v);
			return Vec2(v.x, v.y);
		}
		case SocketType::TYPE_SCALAR:
		{
			Real r;
			callback(_event, &r);
			return Vec2(r, r);
		}
		default:
			return Vec2(0, 0);
		}
	}

	template<> inline Vec3 GetAs<Vec3>(const ScatterEvent &_event) {
		ASSERT_CALLBACK(callback);
		switch (socketType) {
		case SocketType::TYPE_VEC3:
		{
			Vec3 v;
			callback(_event, &v);
			return v;
		}
		case SocketType::TYPE_VEC2:
		{
			Vec2 v;
			callback(_event, &v);
			return Vec3(v.x, v.y, 0);
		}
		case SocketType::TYPE_SCALAR:
		{
			Real r;
			callback(_event, &r);
			return Vec3(r, r, r);
		}
		default:
			return Vec3(0, 0, 0);
		}
	}

	template<> inline BxDF *GetAs<BxDF *>(const ScatterEvent &_event) {
		ASSERT_CALLBACK(callback);
		if (socketType == SocketType::TYPE_BXDF) {
			BxDF *v = nullptr;
			BxDF **vRef = &v;
			callback(_event, vRef);
			return v;
		}
		else return nullptr;
	}

	inline Spectrum GetAsSpectrum(const ScatterEvent &_event, const SpectrumType _type = SpectrumType::Reflectance) {
		ASSERT_CALLBACK(callback);
		switch (socketType) {
		case SocketType::TYPE_COLOUR:
			return Spectrum::FromRGB((Real *) &GetAs<Colour>(_event), _type);
		case SocketType::TYPE_SCALAR:
			return Spectrum(GetAs<Real>(_event));
		case SocketType::TYPE_SPECTRUM:
			{
				Spectrum s;
				callback(_event, &s);
				return s;
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

	template<class T> inline T GetAs(const ScatterEvent &_event) {
		return socket->GetAs<T>(_event);
	}

	inline Spectrum GetAsSpectrum(const ScatterEvent &_event) {
		return socket->GetAsSpectrum(_event);
	}

	void operator=(Socket *_rhs);
};


bool Connect(SocketRef &_socketRef, const Socket &_socket);

bool Connect(const Socket &_socket, SocketRef &_socketRef);

void Disconnect(SocketRef &_socketRef);

/*
	Forward declaration for begin() & end().
*/
class GraphIterator;

class Node {
	public:
		const std::string nodeTag;
		const unsigned numIn, numOut;
		const std::unique_ptr<SocketRef[]> inputSockets;
		const std::unique_ptr<Socket[]> outputSockets;

		Node(const unsigned _numIn, const unsigned _numOut, const std::string &_nodeTag = "node tag");

		Node(const Node &_node);

		SocketRef *GetInputSocket(const unsigned _index);

		SocketRef *GetInputSocket(const char *_tag);

		Socket *GetOutputSocket(const unsigned _index);

		Socket *GetOutputSocket(const char *_tag);

		GraphIterator begin();

		GraphIterator end();
};

/*
	Visits every node connected to the root (front) node at least once.
*/
class GraphIterator : public std::iterator<std::forward_iterator_tag, Node> {
	public:
		GraphIterator(Node *_root);

		GraphIterator(const GraphIterator &_it);

		GraphIterator &operator++();

		GraphIterator operator++(int);

		bool operator==(const GraphIterator &_rhs) const;

		bool operator!=(const GraphIterator &_rhs) const;

		Node &operator*();

	private:
		Node *p;
		std::stack<Node *> stack;

		void MakeStack(std::stack<Node *> &_stack, Node *_node);
};

SG_END

LAMBDA_END