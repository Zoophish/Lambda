#pragma once
#include "ImportUtilities.h"

LAMBDA_BEGIN

class Scene;
class SceneNode;

namespace GraphImport {

	using namespace ImportUtilities;

	/*
		Pushes _aiScene's scene graph (mRootNode) to a SceneNode graph in resources.
	*/
	bool PushGraph(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics);

	/*
		
	*/
	void InstantiateGraph(Scene &_scene, const SceneNode &_node);

}

LAMBDA_END