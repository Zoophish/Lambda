#pragma once
#include <unordered_map>
#include <core/Object.h>
#include <image/Texture.h>

template<class T>
class ResourcePool {
	public:
		std::unordered_map<std::string, T*> pool;

		ResourcePool() {}

		~ResourcePool() {
			for (auto &it : pool) delete it.second;
		}

		void Append(const std::string &_tag, T *_item) {
			pool.insert({ _tag, _item });
		}

		void Remove(const std::string &_tag) {
			pool.erase(_tag);
		}

		inline size_t Size() const { return pool.size(); }

		T &Get(const std::string &_tag) { return pool.find(_tag); }
};

/*
For holding and sharing large data types with the rest of application.
	- It is crucial that any ResourceManager instance should exist for the entirety of program duration.
	- Any resource given to ResourceManager should become owned by the resource manager
	and will be deleted when the ResourceManager is. You should not externally delete pooled resources.
	- Not necessary for more than one per device.
*/
class ResourceManager {
	public:
		ResourcePool<Object> objectPool;
		ResourcePool<Texture> texturePool;

		ResourceManager() {}
};