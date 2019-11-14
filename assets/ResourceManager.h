#pragma once
#include <unordered_map>
#include <core/Object.h>
#include <image/Texture.h>
#include <shading/Material.h>
#include <core/SceneGraph.h>

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

		inline bool Find(const std::string &_tag, T *&_res) const {
			const std::unordered_map<std::string, T*>::const_iterator it = pool.find(_tag);
			if (it != pool.end()) {
				_res = it->second;
				return true;
			}
			return false;
		}
};

/*
	General container for asset objects.
		- Automatically owns everything pushed to it.
*/
class ResourceManager {
	public:
		ResourcePool<Object> objectPool;
		ResourcePool<Texture> texturePool;
		ResourcePool<Material> materialPool;
		ResourcePool<SceneNode> graphPool;

		ResourceManager() {}
};