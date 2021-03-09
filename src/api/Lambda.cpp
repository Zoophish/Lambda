#pragma once
#include <lambda/Lambda.h>

#include <core/Scene.h>
#include <core/TriangleMesh.h>
#include <core/Instance.h>
#include <render/ProgressiveRender.h>
#include <sampling/HaltonSampler.h>
#include <integrators/PathIntegrator.h>
#include <integrators/VolumetricPathIntegrator.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/UtilityIntegrators.h>
#include <shading/graph/GraphBxDF.h>
#include <shading/graph/GraphInputs.h>
#include <lighting/ManyLightSampler.h>

namespace sg = lambda::ShaderGraph;


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

struct LAMBDA_Device {
	ResourceMap resourceMap;
};

struct LAMBDA_Scene {
	lambda::Scene scene;
};

struct LAMBDA_TriangleMesh {
	lambda::TriangleMesh mesh;
};

struct LAMBDA_Proxy {
	lambda::InstanceProxy proxy;
};

struct LAMBDA_Instance {
	lambda::Instance instance;
};

struct LAMBDA_Film {
	lambda::Film film;
};

struct LAMBDA_Camera {
	std::unique_ptr<lambda::Camera> camera;
	std::unique_ptr<lambda::Aperture> aperture;
	LAMBDA_CameraType type;
};

struct LAMBDA_RenderDirective {
	std::unique_ptr<lambda::RenderDirective> directive;
	std::unique_ptr<lambda::Integrator> integrator;
	std::unique_ptr<lambda::Sampler> sampler;
	std::unique_ptr<lambda::SampleShifter> sampleShifter;
	std::unique_ptr<lambda::LightSampler> lightSampler;
};

struct LAMBDA_ProgressiveRenderer {
	std::unique_ptr<lambda::ProgressiveRender> renderer;
};

void LoadStartupResources(LAMBDA_Device *_device) {
	// startup resources
	ResourceMap &resources = _device->resourceMap;

	// sample shift mask
	lambda::Texture *blueNoiseTexture = new lambda::Texture;
	blueNoiseTexture->LoadImageFile("HDR_RGBA_7.png");
	_device->resourceMap.Append(
		"blue_noise_mask",
		LAMBDA_TEXTURE,
		blueNoiseTexture
	);

	// default spectrum
	sg::SpectralInput *white = new sg::SpectralInput(lambda::Spectrum(1));
	resources.Append(
		"default_spectrum",
		LAMBDA_NODE,
		white
	);

	// default bxdf
	sg::LambertianBRDFNode *bxdf = new sg::LambertianBRDFNode(white->GetOutputSocket(sg::SocketType::TYPE_SPECTRUM));
	resources.Append(
		"default_bxdf",
		LAMBDA_NODE,
		bxdf
	);

	// default material
	lambda::Material *material = new lambda::Material;
	material->bxdf = bxdf;
	resources.Append(
		"default_material",
		LAMBDA_MATERIAL,
		material
	);
}

LAMBDA_Device *lambdaCreateDevice() {
	LAMBDA_Device *device = new LAMBDA_Device();
	LoadStartupResources(device);
	return device;
}

void lambdaReleaseDevice(LAMBDA_Device *_device) {
	delete _device;
}

LAMBDA_Scene *lambdaCreateScene(LAMBDA_Device *_device, const char *_name) {
	LAMBDA_Scene *scene = new LAMBDA_Scene();
	_device->resourceMap.Append(_name, LAMBDA_SCENE, scene);
	return scene;
}

void lambdaReleaseScene(LAMBDA_Scene *_scene) {
	delete _scene;
}

void lambdaAttachObject(LAMBDA_Scene *_scene, LAMBDA_Instance *_instance) {
	_scene->scene.AddObject(&_instance->instance);
}

void lambdaDetachObject(LAMBDA_Scene *_scene, LAMBDA_Instance *_instance) {
	_scene->scene.RemoveObject(&_instance->instance);
}

static void AssignObjectDefaults(LAMBDA_Device *_device, lambda::Object *_obj) {
	_obj->material = (lambda::Material*)_device->resourceMap.Find("default_material", LAMBDA_MATERIAL);
}

LAMBDA_TriangleMesh *lambdaCreateTriangleMesh(LAMBDA_Device *_device, const char *_name) {
	LAMBDA_TriangleMesh *mesh = new LAMBDA_TriangleMesh();
	_device->resourceMap.Append(_name, LAMBDA_TRIANGLE_MESH, mesh);
	AssignObjectDefaults(_device, &mesh->mesh);
	return mesh;
}

void lambdaReleaseTriangleMesh(LAMBDA_TriangleMesh *_mesh) {
	delete _mesh;
}

void lambdaTriangleMeshAllocData(LAMBDA_TriangleMesh *_mesh, size_t _numVertices, size_t numTriangles) {
	_mesh->mesh.AllocData(_numVertices, numTriangles);
}

void lambdaTriangleMeshSetBuffer(LAMBDA_TriangleMesh *_mesh, LAMBDA_Buffer _bufferType, void *_ptr, size_t _len) {
	switch(_bufferType) {
	case LAMBDA_BUFFER_VERTEX:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDA_TriangleMesh vertex buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertices = (Vec3 *)_ptr;
		break;

	case LAMBDA_BUFFER_TRIANGLE:

		if (_mesh->mesh.numTriangles != _len) throw "LAMBDA_TriangleMesh triangle buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.triangles = (lambda::Triangle *)_ptr;
		break;

	case LAMBDA_BUFFER_VERTEX_NORMAL:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDA_TriangleMesh vertex normal buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertexNormals = (Vec3 *)_ptr;
		break;

	case LAMBDA_BUFFER_VERTEX_TANGENT:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDA_TriangleMesh vertex tangent buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertexTangents = (Vec3 *)_ptr;
		break;

	case LAMBDA_BUFFER_TEXTURE_COORDINATE:

		if (_mesh->mesh.numVertices != _len) throw "LAMBDA_TriangleMesh texture coordinate buffer length _len does not match size of the allocated buffer.";
		else _mesh->mesh.vertexTangents = (Vec3 *)_ptr;
		break;

	}
}

LAMBDA_Proxy *lambdaCreateProxy(LAMBDA_TriangleMesh *_mesh) {
	LAMBDA_Proxy *proxy = new LAMBDA_Proxy();
	return proxy;
}

LAMBDA_Instance *lambdaCreateInstance(LAMBDA_Device *_device, LAMBDA_Proxy *_proxy) {
	LAMBDA_Instance *instance = new LAMBDA_Instance();
	instance->instance = lambda::Instance(&_proxy->proxy);
	AssignObjectDefaults(_device, &instance->instance);
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

void lambdaSetTransform(LAMBDA_Instance *_instance, float *_xfm) {
	_instance->instance.xfm = Affine3(_xfm);
}

void lambdaSetPosition(LAMBDA_Instance *_instance, float _position[3]) {
	_instance->instance.SetPosition(Vec3(_position[0], _position[1], _position[2]));
}

void lambdaSetScale(LAMBDA_Instance *_instance, float _scale[3]) {
	_instance->instance.SetScale(Vec3(_scale[0], _scale[1], _scale[2]));
}

void lambdaSetEulerAngles(LAMBDA_Instance *_instance, float _eulerAngles[3]) {
	_instance->instance.SetEulerAngles(Vec3(_eulerAngles[0], _eulerAngles[1], _eulerAngles[2]));
}

LAMBDA_Film *lambdaCreateFilm(int _width, int _height) {
	LAMBDA_Film *film = new LAMBDA_Film;
	film->film = lambda::Film(_width, _height);
	return film;
}

LAMBDA_Camera *lambdaCreateCamera(LAMBDA_CameraType _cameraType, float _pos[3], float _phi, float _theta) {
	LAMBDA_Camera *camera = new LAMBDA_Camera();
	camera->type = _cameraType;
	camera->aperture.reset(new lambda::CircularAperture(0));
	switch (_cameraType) {
	case LAMBDA_CAMERA_THIN_LENS:
		camera->camera.reset(new lambda::ThinLensCamera(Vec3(_pos), 1, 1, 1, camera->aperture.get()));
		return camera;
	case LAMBDA_CAMERA_SPHERICAL:
		camera->camera.reset(new lambda::SphericalCamera(Vec3(_pos)));
		return camera;
	}
}

void lambdaSetCameraFocalLength(LAMBDA_Camera *_camera, float _focalLength) {
	if (lambda::ThinLensCamera *camera = dynamic_cast<lambda::ThinLensCamera *>(_camera->camera.get())) {
		camera->focalLength = _focalLength;
	}
}

void lambdaSetCameraApertureSize(LAMBDA_Camera *_camera, float _size) {
	_camera->aperture->size = _size;
}

LAMBDA_RenderProperties *lambdaCreateRenderProperties() {
	LAMBDA_RenderProperties *props = new LAMBDA_RenderProperties;
	props->integrator = LAMBDA_INTEGRATOR_VOLPATH;
	props->lightStrategy = LAMBDA_LIGHT_STRATEGY_TREE;
	props->numThreads = 0;
	props->spp = 1;
	props->tileSizeX = 16;
	props->tileSizeY = 16;
	return props;
}

static void SetIntegrator(LAMBDA_RenderDirective *_directive, LAMBDA_Integrator _integrator) {
	auto setIntegrator = [&](lambda::Integrator *_intgtr) {
		_directive->integrator.reset(_intgtr);
	};
	switch (_integrator) {
	case LAMBDA_INTEGRATOR_PATH:
		setIntegrator(new lambda::PathIntegrator(_directive->sampler.get()));
		break;
	case LAMBDA_INTEGRATOR_VOLPATH:
		setIntegrator(new lambda::VolumetricPathIntegrator(_directive->sampler.get()));
		break;
	case LAMBDA_INTEGRATOR_DIRECT:
		setIntegrator(new lambda::DirectLightingIntegrator(_directive->sampler.get()));
		break;
	case LAMBDA_INTEGRATOR_ALBEDO:
		setIntegrator(new lambda::AlbedoPass(_directive->sampler.get()));
		break;
	case LAMBDA_INTEGRATOR_NORMAL:
		setIntegrator(new lambda::NormalPass(_directive->sampler.get()));
		break;
	case LAMBDA_INTEGRATOR_DEPTH:
		setIntegrator(new lambda::DepthPass(_directive->sampler.get(), 100));
		break;
	default:
		setIntegrator(new lambda::PathIntegrator(_directive->sampler.get()));
		break;
	}
}

static void SetLightSampler(LAMBDA_RenderDirective *_directive, LAMBDA_LightStrategy _lightStrategy) {
	switch (_lightStrategy) {
	case LAMBDA_LIGHT_STRATEGY_POWER:
		_directive->lightSampler.reset(new lambda::PowerLightSampler(*_directive->directive->scene));
		break;
	case LAMBDA_LIGHT_STRATEGY_TREE:
		_directive->lightSampler.reset(new lambda::ManyLightSampler(*_directive->directive->scene));
		break;
	default:
		_directive->lightSampler.reset(new lambda::ManyLightSampler(*_directive->directive->scene));
	}
}

LAMBDA_RenderDirective *lambdaCreateRenderDirective(LAMBDA_Device *_device, LAMBDA_Film *_film, LAMBDA_Camera *_camera, LAMBDA_RenderProperties *_properties) {
	LAMBDA_RenderDirective *directive = new LAMBDA_RenderDirective;
	directive->directive.reset(new lambda::RenderDirective());
	directive->sampler.reset(new lambda::HaltonSampler());

	lambda::Texture *blue_noise_tex = (lambda::Texture *)_device->resourceMap.Find("blue_noise_mask", LAMBDAType::LAMBDA_TEXTURE);
	directive->sampleShifter.reset(new lambda::SampleShifter(blue_noise_tex));
	directive->sampleShifter->maskDimensionStart = 3;	// better output starting here
	directive->directive->camera = _camera->camera.get();
	directive->directive->film = &_film->film;
	directive->directive->sampleShifter = directive->sampleShifter.get();
	// render properties
	directive->directive->spp = _properties->spp;
	directive->directive->tileSizeX = _properties->tileSizeX;
	directive->directive->tileSizeY = _properties->tileSizeY;
	//directive->directive->numThreads = _properties->numThreads;
	SetIntegrator(directive, _properties->integrator);

	return directive;
}

void lambdaSetCamera(LAMBDA_RenderDirective *_directive, LAMBDA_Camera *_camera) {
	_directive->directive->camera = _camera->camera.get();
}

void lambdaSetScene(LAMBDA_RenderDirective *_directive, LAMBDA_Scene *_scene) {
	_directive->directive->scene = &_scene->scene;
}

LAMBDA_ProgressiveRenderer *lambdaCreateProgressiveRenderer(LAMBDA_RenderDirective *_directive) {
	LAMBDA_ProgressiveRenderer *renderer = new LAMBDA_ProgressiveRenderer;
	renderer->renderer.reset(new lambda::ProgressiveRender(*_directive->directive.get()));
	return renderer;
}

void lambdaSetProgressiveRendererCallback(LAMBDA_ProgressiveRenderer *_renderer, void(*_callback)()) {
	_renderer->renderer->updateCallback = _callback;
}

void *lambdaGetProgressiveRendererData(LAMBDA_ProgressiveRenderer *_renderer, int *_width, int *_height) {
	*_width = _renderer->renderer->outputTexture.GetWidth();
	*_height = _renderer->renderer->outputTexture.GetHeight();
	return _renderer->renderer->outputTexture.GetData();
}

LAMBDA_API_NAMESPACE_END