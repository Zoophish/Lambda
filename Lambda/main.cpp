#include <assets/AssetImporter.h>
#include <shading/graph/GraphInputs.h>
#include <shading/graph/GraphBXDF.h>
#include <shading/surface/OrenNayar.h>
#include <shading/surface/Specular.h>
#include <shading/surface/Microfacet.h>
#include <shading/surface/Ghost.h>
#include <shading/media/HomogenousMedium.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/PathIntegrator.h>
#include <integrators/UtilityIntegrators.h>
#include <sampling/HaltonSampler.h>
#include <camera/Camera.h>
#include <core/TriangleMesh.h>
#include <core/Instance.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <lighting/MeshPortal.h>
#include <render/MosaicRenderer.h>
#include <lighting/Blackbody.h>
#include <random>

int main() {
	ResourceManager resources;
	Scene scene;

	Texture red(1, 1, Colour(1, .1, .1));
	Texture yellow(1, 1, Colour(.9, .6, .6));
	Texture white(1, 1, Colour(1, 1, 1));
	Texture blue(1, 1, Colour(.63, .1, 1));
	Texture grid; grid.LoadImageFile("../content/uv_grid.png"); grid.interpolationMode = InterpolationMode::INTERP_NEAREST;
	white.interpolationMode = InterpolationMode::INTERP_NEAREST;
	//const Real alpha = MicrofacetDistribution::RoughnessToAlpha(0.009);
	BeckmannDistribution d;
	FresnelDielectric fres(2.5);

	ShaderGraph::RGBInput whiteNode({1,1,1});
	ShaderGraph::ImageTextureInput gridNode(&grid);
	ShaderGraph::ScalarInput sigmaNode(2.1);
	ShaderGraph::OrenNayarBxDFNode mat(&whiteNode.outputSockets[0], &gridNode.outputSockets[1]);

	ShaderGraph::MicrofacetBRDFNode microfacetBRDF(&whiteNode.outputSockets[0], &gridNode.outputSockets[1], &d, &fres);

	//OrenNayarBRDF mat(&white, 1.8);
	//OrenNayarBRDF blueMat(&blue, 2);
	//MicrofacetBRDF tbr(&white, &d, &fres);
	//tbr.etaT = .00001;
	//MixBSDF mat2(&blueMat, &tbr, .1);

	AssetImporter ai;

	//ai.Import("../content/lucy.obj");
	//TriangleMesh lucy;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lucy);
	//lucy.bxdf = &mat2;
	//lucy.smoothNormals = true;
	//scene.AddObject(&lucy);

	//InstanceProxy proxy(&lucy);
	//proxy.Commit(scene.device);
	//Instance l1(&proxy);
	//l1.bxdf = &mat2;
	//l1.SetScale(Vec3(2, 2, 2));
	//scene.AddObject(&l1);

	AssetImporter ai2;
	ai2.Import("../content/sphere.obj");
	ai2.PushToResourceManager(&resources);
	for (auto &it : resources.objectPool.pool) {
		it.second->bxdf = &microfacetBRDF;
		scene.AddObject(it.second);
	}



	//ai.Import(&resources, "../content/AreaLight.obj");
	//TriangleMesh lightMesh;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lightMesh);
	//lightMesh.smoothNormals = false;
	//MeshLight light(&lightMesh);
	//Real cs[3] = { 78.3583, 79.1876, 64.5809 };
	//Spectrum blck = Spectrum::FromXYZ(cs);
	//texture_t<Spectrum> blckbdy(1, 1, blck);
	//blckbdy.interpolationMode = InterpolationMode::INTERP_NEAREST;
	//light.emission = &blckbdy;
	//light.intensity = .5;
	//scene.AddLight(&light);
	//scene.AddObject(&lightMesh);
	
	//ai.Import(&resources, "../content/SideLight.obj");
	//TriangleMesh lightMesh2;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[1], &lightMesh2);
	//MeshLight light2(&lightMesh2);
	//Real cs2[3] = { 60.8556, 62.7709, 103.5163 };
	//Spectrum blck2 = Spectrum::FromXYZ(cs2);
	//texture_t<Spectrum> blckbdy2(1, 1, blck2);
	//light2.emission = &blckbdy2;
	//light2.intensity = 6;
	//scene.AddLight(&light2);
	//scene.AddObject(&lightMesh2);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/quarry_01_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1.5;
	ibl.offset = Vec2(PI*.5, 0);
	scene.AddLight(&ibl);
	scene.envLight = &ibl;

	//ai.Import("D:\\Assets\\sponza_portal.obj");
	//ai.PushToResourceManager(&resources);
	//TriangleMesh portal;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &portal);
	//MeshPortal portalLight(&ibl, &portal);
	//scene.AddLight(&portalLight);
	//scene.AddObject(&portal);

	scene.Commit();

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	sampleShifter.maskDimensionStart = 3;
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	CircularAperture aperture2(.05);
	ThinLensCamera cam(Vec3(-9, 1, 0), 16, 9, 9, &aperture2);
	aperture2.size = .05;
	aperture2.sampler = &sampler;
	cam.SetFov(.4);
	cam.SetRotation(PI*.5, PI*0);

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
	renderDirective.spp = 32;
	renderDirective.tileSizeX = 32;
	renderDirective.tileSizeY = 32;

	ThreadedMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp, 4);
	rdr.RenderOmp();

	Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(0, 0, 0));
	//Convert film to an RGB image;
	film.ToRGBTexture(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png");
	system("pause");
	return 0;
}