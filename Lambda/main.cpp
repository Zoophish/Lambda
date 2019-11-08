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

	Texture red(1, 1, Colour(.6, .9, .79));
	Texture yellow(1, 1, Colour(.9, .6, .6));
	Texture white(1, 1, Colour(1, 1, 1));
	Texture blue(1, 1, Colour(.63, .3, 1));
	Texture grid; grid.LoadImageFile("../content/uv_grid.png"); grid.interpolationMode = InterpolationMode::INTERP_NEAREST;
	white.interpolationMode = InterpolationMode::INTERP_NEAREST;
	OrenNayarBRDF mat(&white, 1.8);
	const Real alpha = MicrofacetDistribution::RoughnessToAlpha(0.009);
	BeckmannDistribution d(alpha, alpha);
	FresnelDielectric fres(2.5);
	MicrofacetBRDF tbr(&blue, &d, &fres);
	tbr.etaT = .00001;
	SpecularBRDF mat2(&white, &fres);

	AssetImporter ai;

	ai.Import(&resources, "../content/Birch.obj");
	MeshImport::LoadMeshes(ai.scene, &resources);
	TriangleMesh mesh2;
	MeshImport::LoadMeshBuffers(ai.scene->mMeshes[0], &mesh2);
	mesh2.bxdf = &mat;
	mesh2.smoothNormals = true;
	//scene.AddObject(mesh2);
	for (auto &it : resources.objectPool.pool) {
		it.second->bxdf = &mat;
		scene.AddObject(*it.second);
	}

	//InstanceProxy proxy(&mesh2);
	//proxy.Commit(scene.device);
	//Instance l1(&proxy);
	//l1.bxdf = &mat;
	//l1.SetScale(Vec3(.5, .5, .5));
	//scene.AddObject(l1);
	//
	//Instance l2(&proxy);
	//l2.bxdf = &tbr;
	//l2.SetPosition(Vec3(1, 0, -.7));
	//l2.SetScale(Vec3(.65, .65, .65));
	//scene.AddObject(l2);
	//
	//FresnelBSDF redMat(&red, 2.4);
	//Instance l3(&proxy);
	//l3.bxdf = &mat2;
	//l3.SetPosition(Vec3(-1, 0, -.7));
	//l3.SetScale(Vec3(.65, .65, .65));
	//scene.AddObject(l3);
	
	TriangleMesh mesh3;
	ai.Import(&resources, "../content/Backdrop.obj");
	MeshImport::LoadMeshBuffers(ai.scene->mMeshes[0], &mesh3);
	//mesh3.smoothNormals = false;
	OrenNayarBRDF mat3(&grid, 2);
	mesh3.bxdf = &mat3;
	scene.AddObject(mesh3);

	ai.Import(&resources, "../content/AreaLight.obj");
	TriangleMesh lightMesh;
	MeshImport::LoadMeshBuffers(ai.scene->mMeshes[0], &lightMesh);
	lightMesh.smoothNormals = false;
	MeshLight light(&lightMesh);
	Real cs[3] = { 78.3583, 79.1876, 64.5809 };
	Spectrum blck = Spectrum::FromXYZ(cs);
	texture_t<Spectrum> blckbdy(1, 1, blck);
	blckbdy.interpolationMode = InterpolationMode::INTERP_NEAREST;
	light.emission = &blckbdy;
	light.intensity = 1.5;
	scene.AddLight(&light);
	scene.AddObject(lightMesh);
	
	ai.Import(&resources, "../content/SideLight.obj");
	TriangleMesh lightMesh2;
	MeshImport::LoadMeshBuffers(ai.scene->mMeshes[1], &lightMesh2);
	MeshLight light2(&lightMesh2);
	Real cs2[3] = { 60.8556, 62.7709, 103.5163 };
	Spectrum blck2 = Spectrum::FromXYZ(cs2);
	texture_t<Spectrum> blckbdy2(1, 1, blck2);
	light2.emission = &blckbdy2;
	light2.intensity = 4.5;
	scene.AddLight(&light2);
	scene.AddObject(lightMesh2);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/qwantani_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 0;
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
	ThinLensCamera cam(Vec3(0, 1, 2.2), 16, 9, 2.2);
	//SphericalCamera cam(Vec3(0,1,0));
	cam.aperture = &aperture2;
	cam.aperture->size = .002;
	cam.aperture->sampler = &sampler;
	cam.SetFov(1.5);
	cam.SetRotation(PI, -PI*.03f);

	//Make some integrators.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);
	NormalPass normalPass(&sampler);

	Film film(1280, 720);

	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &pathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 16;
	renderDirective.tileSizeX = 32;
	renderDirective.tileSizeY = 32;

	ThreadedMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp, 4);
	rdr.RenderOmp();

	Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(0, 0, 0));
	//Convert film to an RGB image;
	film.ToRGBTexture(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png");
	return 0;
}