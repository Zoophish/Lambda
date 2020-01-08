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
#include <lighting/Portal.h>
#include <render/MosaicRenderer.h>
#include <utility/Memory.h>
#include <random>

int main() {
	ResourceManager resources;
	Scene scene;

	Texture red(1, 1, Colour(1, .1, .1));
	Texture yellow(1, 1, Colour(.9, .6, .6));
	Texture white(1, 1, Colour(1, 1, 1));
	Texture blue(1, 1, Colour(.63, .1, 1));
	Texture grid; grid.LoadImageFile("../content/uv_grid.png"); grid.interpolationMode = InterpolationMode::INTERP_NEAREST;
	Texture checker; checker.LoadImageFile("../content/checker.jpg"); checker.interpolationMode = InterpolationMode::INTERP_NEAREST;
	BeckmannDistribution d;
	FresnelDielectric fres(2.5);

	MemoryArena graphArena;
	namespace sg = ShaderGraph;
	sg::RGBInput *whiteNode = graphArena.New<sg::RGBInput>(Colour(1, 1, 1));
	sg::ImageTextureInput *gridNode = graphArena.New<sg::ImageTextureInput>(&grid);
	sg::ImageTextureInput *checkNode = graphArena.New<sg::ImageTextureInput>(&checker);
	Texture scratches(1,1, Colour(.5,.5,.5)); //scratches.LoadImageFile("D:\\Assets\\POLIIGON Surface Imperfections\\Stains Liquid\\StainsLiquidGeneric003_OVERLAY_VAR1_HIRES.jpg");
	sg::ImageTextureInput *sigmaNode = graphArena.New<sg::ImageTextureInput>(&scratches);
	sg::ScalarInput *iorNode = graphArena.New<sg::ScalarInput>(1.3);
	sg::OrenNayarBxDFNode *mat = graphArena.New<sg::OrenNayarBxDFNode>(&gridNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::FresnelBSDFNode *fresMat = graphArena.New<sg::FresnelBSDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	  
	sg::MicrofacetBRDFNode *microfacetBRDF = graphArena.New<sg::MicrofacetBRDFNode>(&whiteNode->outputSockets[0], &sigmaNode->outputSockets[1], &d, &fres);

	sg::MixBxDFNode *mixMat = graphArena.New<sg::MixBxDFNode>(&fresMat->outputSockets[0], &mat->outputSockets[0], &checkNode->outputSockets[1]);

	AssetImporter ai2;
	ai2.Import("D:\\Assets\\gltf 2.0\\Sponza\\glTF\\Sponza.gltf");
	ai2.PushToResourceManager(&resources);
	for (auto &it : resources.objectPool.pool) {
		Material *m = MaterialImport::GetMaterial(ai2.scene, &resources, it.first);
		if (m) {
			it.second->bxdf = m->bxdf;
			scene.AddObject(it.second);
		}
	}

	AssetImporter ai;
	//ai.Import("../content/Backdrop.obj");
	//ai.PushToResourceManager(&resources);
	//TriangleMesh backdropMesh;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &backdropMesh);
	//backdropMesh.bxdf = mat;
	//scene.AddObject(&backdropMesh);


	ai.Import("../content/TopLight.obj");
	ai.PushToResourceManager(&resources);
	TriangleMesh lightMesh;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lightMesh);
	lightMesh.smoothNormals = false;
	MeshLight light(&lightMesh);
	sg::ScalarInput temp1(2900);
	sg::BlackbodyInput blckInpt1(&temp1.outputSockets[0]);
	light.emission = &blckInpt1.outputSockets[0];
	light.intensity = 10000;
	//scene.AddLight(&light);
	//scene.AddObject(&lightMesh);
	
	//ai.Import("../content/SideLight.obj");
	//ai.PushToResourceManager(&resources);
	//TriangleMesh lightMesh2;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[1], &lightMesh2);
	//MeshLight light2(&lightMesh2);
	//sg::ScalarInput temp(10000);
	//sg::BlackbodyInput blckInpt(&temp.outputSockets[0]);
	//light2.emission = &blckInpt.outputSockets[0];
	//light2.intensity = 32;
	//scene.AddLight(&light2);
	//scene.AddObject(&lightMesh2);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/small_cave_4k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1.8;
	ibl.offset = Vec2(PI*0, 0);
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
	ThinLensCamera cam(Vec3(0, 2, 0), 16, 9, 2, &aperture2);
	aperture2.size = .001;
	aperture2.sampler = &sampler;
	cam.SetFov(1.4);
	cam.SetRotation(PI*0, PI*-.0478);

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
	renderDirective.spp = 1;
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