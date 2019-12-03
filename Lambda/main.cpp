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
	BeckmannDistribution d;
	FresnelDielectric fres(2.5);

	ShaderGraph::RGBInput whiteNode({1,.5,.5});
	ShaderGraph::ImageTextureInput gridNode(&grid);
	ShaderGraph::ScalarInput sigmaNode(.3);
	ShaderGraph::OrenNayarBxDFNode mat(&gridNode.outputSockets[0], &gridNode.outputSockets[1]);
	  
	ShaderGraph::MicrofacetBRDFNode microfacetBRDF(&whiteNode.outputSockets[0], &sigmaNode.outputSockets[0], &d, &fres);

	AssetImporter ai;

	AssetImporter ai2;
	ai2.Import("../content/lucy.obj");
	ai2.PushToResourceManager(&resources);
	for (auto &it : resources.objectPool.pool) {
		it.second->bxdf = &microfacetBRDF;
		scene.AddObject(it.second);
	}

	ai.Import("../content/Backdrop.obj");
	ai.PushToResourceManager(&resources);
	TriangleMesh backdropMesh;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &backdropMesh);
	backdropMesh.bxdf = &mat;
	scene.AddObject(&backdropMesh);


	ai.Import("../content/AreaLight.obj");
	ai.PushToResourceManager(&resources);
	TriangleMesh lightMesh;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lightMesh);
	lightMesh.smoothNormals = false;
	MeshLight light(&lightMesh);
	Real cs[3] = { 78.3583, 79.1876, 64.5809 };
	Spectrum blck = Spectrum::FromXYZ(cs);
	texture_t<Spectrum> blckbdy(1, 1, blck);
	blckbdy.interpolationMode = InterpolationMode::INTERP_NEAREST;
	ShaderGraph::SpectralTextureInput blckbdyNode(&blckbdy);
	ShaderGraph::ScalarInput temp1(2700);
	ShaderGraph::BlackbodyInput blckInpt1(&temp1.outputSockets[0]);
	light.emission = &blckInpt1.outputSockets[0];
	light.intensity = 35;
	scene.AddLight(&light);
	scene.AddObject(&lightMesh);
	
	//ai.Import("../content/SideLight.obj");
	//ai.PushToResourceManager(&resources);
	//TriangleMesh lightMesh2;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[1], &lightMesh2);
	//MeshLight light2(&lightMesh2);
	//Real cs2[3] = { 60.8556, 62.7709, 103.5163 };
	//Spectrum blck2 = Spectrum::FromXYZ(cs2);
	//texture_t<Spectrum> blckbdy2(1, 1, blck2);
	//ShaderGraph::SpectralTextureInput blckbdyNode2(&blckbdy2);
	//ShaderGraph::ScalarInput temp(4500);
	//ShaderGraph::BlackbodyInput blckInpt(&temp.outputSockets[0]);
	//light2.emission = &blckInpt.outputSockets[0];
	//light2.intensity = .8;
	//scene.AddLight(&light2);
	//scene.AddObject(&lightMesh2);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/quarry_01_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 0;
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
	ThinLensCamera cam(Vec3(0, 1, 2.2), 16, 9, 2.2, &aperture2);
	aperture2.size = .001;
	aperture2.sampler = &sampler;
	cam.SetFov(1.5);
	cam.SetRotation(PI, PI*-.01);

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
	renderDirective.spp = 8;
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