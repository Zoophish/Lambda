#pragma once
#include <lambda/Lambda.h>

#include <core/Scene.h>
#include <core/TriangleMesh.h>
#include <core/Instance.h>



class ResourceMap {
	private:
		struct pair_hash {
			template<class A, class B>
			inline size_t operator() (const std::pair<A, B> &_pair) const {
				return std::hash<A>()(_pair.first) ^ std::hash<B>()(_pair.second);
			}
		};

	public:
		std::unordered_map<std::pair<std::string, Lambda::LAMBDAType>, void*, pair_hash> pool;

		ResourceMap() {}

		~ResourceMap() {
			for (auto &it : pool) delete it.second;
		}

		inline void Append(const std::string &_tag, const Lambda::LAMBDAType _type, void *_item) {
			pool.insert({ {_tag, _type}, _item });
		}

		inline bool Remove(const std::string &_tag, const Lambda::LAMBDAType _type) {
			return pool.erase({ _tag, _type }) == 1;
		}

		inline size_t Size() const {
			return pool.size();
		}

		inline void *Find(const std::string &_tag, const Lambda::LAMBDAType _type) const {
			const std::unordered_map<std::pair<std::string, Lambda::LAMBDAType>, void *>::const_iterator it = pool.find({_tag, _type });
			if (it != pool.end()) return it->second;
			return nullptr;
		}
};

LAMBDA_API_NAMESPACE_BEGIN

struct LAMBDADevice {
	ResourceMap resourceMap;
};

struct LAMBDAScene {
	lambda::Scene scene;
};

struct LAMBDATriangleMesh {
	lambda::TriangleMesh mesh;
};

struct LAMBDAProxy {
	lambda::InstanceProxy proxy;
};

struct LAMBDAInstance {
	lambda::Instance instance;
};

struct LAMBDAMaterial {
	lambda::Material material;
};



LAMBDADevice *lambdaCreateDevice() {
	LAMBDADevice *device = new LAMBDADevice();
	return device;
}

void lambdaReleaseDevice(LAMBDADevice *_device) {
	delete _device;
}

LAMBDAScene *lambdaCreateScene(LAMBDADevice *_device, const char *_name) {
	LAMBDAScene *scene = new LAMBDAScene();
	_device->resourceMap.Append(_name, LAMBDA_SCENE, scene);
	return scene;
}

void lambdaReleaseScene(LAMBDAScene *_scene) {
	delete _scene;
}

void lambdaAttachObject(LAMBDAScene *_scene, LAMBDAInstance *_instance) {
	_scene->scene.AddObject(&_instance->instance);
}

void lambdaDetachObject(LAMBDAScene *_scene, LAMBDAInstance *_instance) {
	_scene->scene.RemoveObject(&_instance->instance);
}

LAMBDATriangleMesh *lambdaCreateTriangleMesh(LAMBDADevice *_device, const char *_name) {
	LAMBDATriangleMesh *mesh = new LAMBDATriangleMesh();
	_device->resourceMap.Append(_name, LAMBDA_TRIANGLE_MESH, mesh);
	return mesh;
}

void lambdaReleaseTriangleMesh(LAMBDATriangleMesh *_mesh) {
	delete _mesh;
}

void lambdaTriangleMeshAllocData(LAMBDATriangleMesh *_mesh, size_t _numVertices, size_t numTriangles) {
	_mesh->mesh.AllocData(_numVertices, numTriangles);
}

void lambdaTriangleMeshSetBuffer(LAMBDATriangleMesh *_mesh, LAMBDABuffer _bufferType, void *_ptr, size_t _len) {
	switch(_bufferType) {
	case LAMBDA_BUFFER_VERTEX:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDATriangleMesh vertex buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertices = (Vec3 *)_ptr;
		break;

	case LAMBDA_BUFFER_TRIANGLE:

		if (_mesh->mesh.numTriangles != _len) throw "LAMBDATriangleMesh triangle buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.triangles = (lambda::Triangle *)_ptr;
		break;

	case LAMBDA_BUFFER_VERTEX_NORMAL:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDATriangleMesh vertex normal buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertexNormals = (Vec3 *)_ptr;
		break;

	case LAMBDA_BUFFER_VERTEX_TANGENT:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDATriangleMesh vertex tangent buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertexTangents = (Vec3 *)_ptr;
		break;

	case LAMBDA_BUFFER_TEXTURE_COORDINATE:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDATriangleMesh texture coordinate buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertexTangents = (Vec3 *)_ptr;
		break;

	}
}

LAMBDAProxy *lambdaCreateProxy(LAMBDATriangleMesh *_mesh) {
	LAMBDAProxy *proxy = new LAMBDAProxy();
	return proxy;
}

LAMBDAInstance *lambdaCreateInstance(LAMBDAProxy *_proxy) {
	LAMBDAInstance *instance = new LAMBDAInstance();
	instance->instance = lambda::Instance(&_proxy->proxy);
	return instance;
}

void lambdaMakeAffineTransform(float _xfm[12], float _position[3], float _scale[3], float _eulerAngles[3]) {
	Affine3 xfm;
	xfm[9] = _position[0];
	xfm[10] = _position[1];
	xfm[11] = _position[2];
	xfm[0] = _scale[0];
	xfm[4] = _scale[1];
	xfm[8] = _scale[2];
	Affine3 rot = Affine3::GetRotationX(_eulerAngles[0]) * Affine3::GetRotationX(_eulerAngles[1]) * Affine3::GetRotationX(_eulerAngles[2]);
	xfm = xfm * rot;
	_xfm = &xfm[0];
}

void lambdaSetTransform(LAMBDAInstance *_instance, float *_xfm) {
	_instance->instance.xfm = Affine3(_xfm);
}

void lambdaSetPosition(LAMBDAInstance *_instance, float _position[3]) {
	_instance->instance.SetPosition(Vec3(_position[0], _position[1], _position[2]));
}

void lambdaSetScale(LAMBDAInstance *_instance, float _scale[3]) {
	_instance->instance.SetScale(Vec3(_scale[0], _scale[1], _scale[2]));
}

void lambdaSetEulerAngles(LAMBDAInstance *_instance, float _eulerAngles[3]) {
	_instance->instance.SetEulerAngles(Vec3(_eulerAngles[0], _eulerAngles[1], _eulerAngles[2]));
}

LAMBDAMaterial *lambdaCreateMaterial() {
	LAMBDAMaterial *material = new LAMBDAMaterial();
	return material;
}

LAMBDA_API_NAMESPACE_END