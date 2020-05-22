#pragma once
#include <unordered_map>
#include <core/Instance.h>
#include <image/Texture.h>
#include <shading/Material.h>
#include <core/Scene.h>

LAMBDA_BEGIN

template<class T>
class ResourcePool {
	public:
		std::unordered_map<std::string, T*> pool;

		ResourcePool() {}

		~ResourcePool() {
			for (auto &it : pool) delete it.second;
		}

		inline void Append(const std::string &_tag, T *_item) {
			pool.insert({ _tag, _item });
		}

		inline bool Remove(const std::string &_tag) {
			return pool.erase(_tag) == 1;
		}

		inline size_t Size() const {
			return pool.size();
		}

		inline T *Find(const std::string &_tag) const {
			const std::unordered_map<std::string, T *>::const_iterator it = pool.find(_tag);
			if (it != pool.end()) return it->second;
			return nullptr;
		}
};

/*
	General container for asset objects.
		- Automatically owns everything pushed to it.
*/
struct ResourceManager {
		ResourcePool<Object> objectPool;
		ResourcePool<Texture> texturePool;
		ResourcePool<Material> materialPool;
		ResourcePool<SceneNode> graphPool;
		ResourcePool<InstanceProxy> proxyPool;
};

LAMBDA_END