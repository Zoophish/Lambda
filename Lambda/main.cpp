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
	Scene scene;

	AssetImporter ai;
	ai.Import("../content/sponza.obj");
	TriangleMesh mesh;
	mesh.LoadFromImport(scene.device, ai);
	mesh.smoothNormals = false;
	mesh.hasUVs = false;
	Texture albedo(1,1,Colour(1,1,1));
//	albedo.LoadImageFile("../content/box_tex.png");
	albedo.interpolationMode = InterpolationMode::INTERP_NEAREST;
	OrenNayarBRDF mat(&albedo, 1);
	mesh.bxdf = &mat;
	
	scene.AddObject(mesh);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/cloud_layers_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1;
	ibl.offset = Vec2(PI*-.08, 0);
	scene.AddLight(&ibl);
	scene.envLight = &ibl;

	ai.Import("../content/lucy.obj");
	TriangleMesh lucy;
	lucy.LoadFromImport(scene.device, ai);
	lucy.smoothNormals = false;
	lucy.hasUVs = false;
	const Real r = MicrofacetDistribution::RoughnessToAlpha(.05);
	TrowbridgeReitzDistribution d(r,r);
	FresnelDielectric fres(3);
	Texture red(1, 1, Colour(1, .2, .2));
	MicrofacetBRDF metal(&red, &d, &fres);
	lucy.bxdf = &metal;
	scene.AddObject(lucy);

	scene.Commit();

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	sampleShifter.maskDimensionStart = 3;
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	CircularAperture aperture2(.09);
	ThinLensCamera cam(Vec3(5, 1, 0), 1, 1, 5);
	//SphericalCamera cam(Vec3(0,1,0));
	cam.aperture = &aperture2;
	cam.aperture->size = 0;
	cam.aperture->sampler = &sampler;
	cam.SetFov(1.5);
	cam.SetRotation(-PI*.5, 0);

	//Make the integrator.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);

	Film film(800, 800);

	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &pathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 1;
	renderDirective.tileSizeX = 32;
	renderDirective.tileSizeY = 32;

	ThreadedMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp, 1);
	rdr.RenderOmp();

	Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(0, 0, 0));
	//Convert film to an RGB image;
	film.ToRGBTexture(&tex);
	
	//Save to file.
	tex.SaveToImageFile("out.png");
	return 0;
}