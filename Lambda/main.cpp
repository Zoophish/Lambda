#include <assets/AssetImporter.h>
#include <shading/OrenNayar.h>
#include <shading/Lambertian.h>
#include <shading/Specular.h>
#include <shading/Microfacet.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/PathIntegrator.h>
#include <integrators/UtilityIntegrators.h>
#include <sampling/HaltonSampler.h>
#include <camera/PinholeCamera.h>
#include <camera/EnvironmentCamera.h>
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
	ai.Import("../content/box.obj");
	TriangleMesh mesh;
	mesh.LoadFromImport(scene.device, ai);
	mesh.smoothNormals = false;
	mesh.hasUVs = true;
	Texture albedo(1,1,Colour(1,1,1));
	albedo.LoadImageFile("../content/box_tex.png");
	albedo.interpolationMode = InterpolationMode::INTERP_NEAREST;
	OrenNayarBRDF mat(&albedo, 1);
	mesh.bxdf = &mat;

	ai.Import("../content/lucy.obj");
	TriangleMesh lucy;
	lucy.LoadFromImport(scene.device, ai);
	lucy.smoothNormals = false;
	Texture white(1, 1, Colour(1, .9, .9));
	const Real r = MicrofacetDistribution::RoughnessToAlpha(.005);
	BeckmannDistribution dist(r, r);
	FresnelDielectric fres(1.8);
	MicrofacetBRDF mat2(&white, &dist, &fres);
	//mat2.etaT = 1.8;
	//FresnelBSDF glass(&white, 1.333);
	//OrenNayarBRDF mat2(&white, 3);
	lucy.bxdf = &mat2;

	//InstanceProxy proxy(&scene.device, &lucy);
	//std::mt19937 mt(5);
	//std::uniform_real_distribution<Real> d(0,1);
	//std::vector<Instance*> instances;

	//for (unsigned i = 0; i < 400; ++i) {
	//	instances.push_back(new Instance(&proxy));
	//	instances.back()->bxdf = &glass;
	//	instances.back()->SetScale(Vec3(.1, .1 , .1));
	//	instances.back()->SetPosition(Vec3(d(mt) * 2 - 1, d(mt) * 2, d(mt) * 2 - 1));
	//	rtcSetGeometryTransform(instances.back()->geometry, 0, RTC_FORMAT_FLOAT3X4_COLUMN_MAJOR, &instances.back()->xfm);
	//	scene.AddObject(*instances.back());
	//}
	scene.AddObject(mesh);
	scene.AddObject(lucy);

	AssetImporter ai2;
	ai2.Import("../content/box_light.obj");
	TriangleMesh light;
	light.LoadFromImport(scene.device, ai2);
	light.smoothNormals = false;
	
	Texture emission(1, 1, Colour(1, 1, 1));
	//emission.LoadImageFile("../content/uv_grid_2.png");
	MeshLight meshLight(&light);
	meshLight.emission = &emission;
	meshLight.intensity = 400;
	//scene.AddObject(light);
	//scene.AddLight(&meshLight);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/cloud_layers_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1;
	ibl.offset = Vec2(PI*-.08, 0);
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
	ThinLensCamera cam(Vec3(0, 1, 5), 1, 1, 5);
	cam.aperture = &aperture2;
	cam.aperture->size = 0.09;
	cam.aperture->sampler = &sampler;
	cam.SetFov(.52);
	cam.SetRotation(-PI, 0);

	//Make the integrator.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);

	Film film(1024, 1024);

	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &pathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 200;
	renderDirective.tileSizeX = 64;
	renderDirective.tileSizeY = 64;

	ThreadedMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp, 4);
	rdr.Render();

	Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(0, 0, 0));
	//Convert film to an RGB image;
	film.ToRGBTexture(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png");
	return 0;
}