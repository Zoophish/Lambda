#include <assets/AssetImporter.h>
#include <shading/surface/OrenNayar.h>
#include <shading/surface/Lambertian.h>
#include <shading/surface/Specular.h>
#include <shading/surface/Microfacet.h>
#include <shading/media/HomogenousMedium.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/PathIntegrator.h>
#include <integrators/UtilityIntegrators.h>
#include <sampling/HaltonSampler.h>
#include <camera/PinholeCamera.h>
#include <camera/SphericalCamera.h>
#include <camera/ThinLensCamera.h>
#include <core/TriangleMesh.h>
#include <core/Instance.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <lighting/MeshPortal.h>
#include <render/MosaicRenderer.h>
#include <lighting/Blackbody.h>
#include <camera/Aperture.h>
#include <random>

int main() {
	ResourceManager resources;
	Scene scene;

	Texture white(1, 1, Colour(1, 1, 1));
	white.interpolationMode = InterpolationMode::INTERP_NEAREST;
	OrenNayarBRDF mat(&white, 2);

	AssetImporter ai;
	ai.Import(&resources, "E:\\Assets\\san-miguel.obj");
	for (auto &it : resources.objectPool.pool) {
		it.second->bxdf = &mat;
		scene.AddObject(*it.second);
	}

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/quarry_01_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = .5;
	ibl.offset = Vec2(0, 0);
	scene.AddLight(&ibl);
	scene.envLight = &ibl;

	scene.Commit();

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	sampleShifter.maskDimensionStart = 3;
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	CircularAperture aperture2(.09);
	ThinLensCamera cam(Vec3(0, 2, 10), 16, 9, 5);
	//SphericalCamera cam(Vec3(0,1,0));
	cam.aperture = &aperture2;
	cam.aperture->size = 0;
	cam.aperture->sampler = &sampler;
	cam.SetFov(1.5);
	cam.SetRotation(-PI*.5, -.05);

	//Make the integrator.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);

	Film film(1280, 720);

	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &pathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 1;
	renderDirective.tileSizeX = 64;
	renderDirective.tileSizeY = 64;

	ThreadedMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp, 4);
	rdr.RenderOmp();

	Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(0, 0, 0));
	//Convert film to an RGB image;
	film.ToRGBTexture(&tex);
	
	//Save to file.
	tex.SaveToImageFile("out.png");
	return 0;
}