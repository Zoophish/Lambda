#pragma once
#include "ImportUtilities.h"
#include <core/SceneGraph.h>

namespace GraphImport {

	using namespace ImportUtilities;

	/*
		Pushes _aiScene's scene graph (mRootNode) to a SceneNode graph in resources.
	*/
	bool PushGraph(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics);

}