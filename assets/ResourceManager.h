#pragma once
#include <unordered_map>
#include <core/Object.h>
#include <image/Texture.h>
#include <core/Instance.h>

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

		inline void Remove(const std::string &_tag) {
			pool.erase(_tag);
		}

		inline size_t Size() const {
			return pool.size();
		}

		inline T &Get(const std::string &_tag) {
			return pool.find(_tag);
		}
};

class ResourceManager {
	public:
		ResourcePool<Object> objectPool;
		ResourcePool<InstanceProxy> proxyPool;
		ResourcePool<Texture> texturePool;

		ResourceManager() {}
};