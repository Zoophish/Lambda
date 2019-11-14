#include "GraphImport.h"

namespace GraphImport {

	/*
		Links the scene graph node, _sceneNode, to corresponding objects held in _resources.
			- Objects are linked by name tag.
	*/
	void LinkNodeToObjects(const aiScene *_aiScene, const aiNode *_aiNode, SceneNode *_sceneNode, const ResourceManager *_resources, ImportMetrics *_metrics) {
		const unsigned *mEnd = &_aiNode->mMeshes[_aiNode->mNumMeshes];
		for (unsigned *it = _aiNode->mMeshes; it < mEnd; ++it) {
			const std::string objName = _aiScene->mMeshes[*it]->mName.C_Str();
			Object *objRef = nullptr;
			_resources->objectPool.Find(objName, objRef);
			if (!objRef) _metrics->AppendError(objName + " not found in resources.");
			else {
				objRef->xfm = aiMatToAff3(_aiNode->mTransformation);
				_sceneNode->objects.push_back(objRef);
			}
		}
	}

	/*
		
	*/
	void LoadNode(const aiScene *_aiScene, const aiNode *_aiNode, SceneNode *_me, const ResourceManager *_resources, ImportMetrics *_metrics) {
		_me->name = _aiNode->mName.C_Str();
		_me->transform = aiMatToAff3(_aiNode->mTransformation);
		LinkNodeToObjects(_aiScene, _aiNode, _me, _resources, _metrics);
		if (_aiNode->mChildren) {
			_me->children.resize(_aiNode->mNumChildren);
			for (unsigned i = 0; i < _aiNode->mNumChildren; ++i) {
				LoadNode(_aiScene, _aiNode->mChildren[i], &_me->children[i], _resources, _metrics);
			}
		}
		_metrics->AppendMetric("Node added: " + _me->name);
	}

	/*
		
	*/
	bool PushGraph(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
		if (_aiScene->mRootNode) {
			SceneNode *rootNode = new SceneNode;
			_resources->graphPool.Append(_aiScene->mRootNode->mName.C_Str(), rootNode);
			LoadNode(_aiScene, _aiScene->mRootNode, rootNode, _resources, _metrics);
			_metrics->AppendMetric("Root node: " + rootNode->name);
			return !_metrics->HasErrors();
		}
		_metrics->AppendError("No graph (root node) in asset.");
		return false;
	}

}