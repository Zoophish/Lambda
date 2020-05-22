#include "GraphImport.h"
#include <core/Scene.h>
#include <core/Instance.h>
#include <iostream>

LAMBDA_BEGIN

namespace GraphImport {

	/*
		Links the scene graph node, _sceneNode, to corresponding instance held in _resources.
			- Objects / instances are linked by name tag.
	*/
	static void LinkNodeToInstances(const aiScene *_aiScene, const aiNode *_aiNode, SceneNode *_sceneNode, ResourceManager *_resources, ImportMetrics *_metrics) {
		for (unsigned i = 0; i < _aiNode->mNumMeshes; ++i) {
			const aiMesh *mMesh = _aiScene->mMeshes[_aiNode->mMeshes[i]];
			const std::string objName = mMesh->mName.C_Str();
			aiString matName;
			_aiScene->mMaterials[mMesh->mMaterialIndex]->Get(AI_MATKEY_NAME, matName);
			InstanceProxy *iProxy = _resources->proxyPool.Find(objName + "_INST_PROXY");
			if (!iProxy) {
				Object *iObject = _resources->objectPool.Find(objName);
				if (!iObject) {
					_metrics->AppendError("Could not find instance's original object: " + objName);
					return;
				}
				iProxy = new InstanceProxy(iObject);
				_resources->proxyPool.Append(objName + "_INST_PROXY", iProxy);
			}
			Instance *inst = new Instance(iProxy);
			inst->xfm = aiMatToAff3(_aiNode->mTransformation);
			Material *instMat = _resources->materialPool.Find(matName.C_Str());
			if (instMat) inst->material->bxdf = instMat->bxdf;
			else _metrics->AppendError("Could not find material for instance: " + objName);
			_sceneNode->objects.push_back(inst);
			_resources->objectPool.Append(objName + "_INST", inst);
		}
	}

	/*
		Loads a node and its constituent network into _node.
	*/
	static void LoadNode(const aiScene *_aiScene, const aiNode *_aiNode, SceneNode *_node, ResourceManager *_resources, ImportMetrics *_metrics) {
		_node->name = _aiNode->mName.C_Str();
		_node->xfm = aiMatToAff3(_aiNode->mTransformation);
		if(_aiNode->mNumMeshes > 0) LinkNodeToInstances(_aiScene, _aiNode, _node, _resources, _metrics);
		else {
			Empty *empty = new Empty(aiMatToAff3(_aiNode->mTransformation));
			_node->objects.push_back(empty);
			_resources->objectPool.Append(_node->name, empty);
		}
		if (_aiNode->mChildren) {
			_node->children.resize(_aiNode->mNumChildren);
			for (unsigned i = 0; i < _aiNode->mNumChildren; ++i) {
				_node->children[i].parent = _node;
				LoadNode(_aiScene, _aiNode->mChildren[i], &_node->children[i], _resources, _metrics);
				//for(auto &it : _node->children[i].objects) _node->objects[0]->AddChild(it);
			}
		}
		_metrics->AppendMetric("Node added: " + _node->name);
	}

	static void AddNodeToScene(Scene &_scene, const SceneNode &_sceneNode, Object *_parent = nullptr) {
		const size_t os = _sceneNode.objects.size();
		for (unsigned i = 0; i < os; ++i) { //Number of objects usually = 1
			_scene.AddObject(_sceneNode.objects[i]);
			if (_parent) _parent->AddChild(_sceneNode.objects[i]);
		}
		for (unsigned i = 0; i < _sceneNode.children.size(); ++i) {
			AddNodeToScene(_scene, _sceneNode.children[i], _sceneNode.objects[0]); //Therefore use first object as parent
		}
	}

	/*
		
	*/
	bool PushGraph(const aiScene *_aiScene, ResourceManager *_resources, ImportMetrics *_metrics) {
		if (_aiScene->mRootNode) {
			SceneNode *rootNode = new SceneNode;
			LoadNode(_aiScene, _aiScene->mRootNode, rootNode, _resources, _metrics);
			_resources->graphPool.Append(_aiScene->mRootNode->mName.C_Str(), rootNode);
			_metrics->AppendMetric("Root node: " + rootNode->name);
			return !_metrics->HasErrors();
		}
		_metrics->AppendError("No graph (root node) in asset.");
		return false;
	}

	void InstantiateGraph(Scene &_scene, const SceneNode &_node) {
		AddNodeToScene(_scene, _node);
	}

}

LAMBDA_END