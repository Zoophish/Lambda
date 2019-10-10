#pragma once
#include <unordered_map>
#include "AssetImporter.h"
#include <core/Object.h>
#include <image/Texture.h>

template<class T>
class ResourcePool {
	public:
		std::unordered_map<std::string, std::unique_ptr<T>> pool;

		bool Append(const std::string &_tag, Object *_object) {
			return pool.insert({ _tag, std::make_unique<T>(_object) });
		}

		bool Remove(const std::string &_tag) {
			return pool.erase(_tag);
		}

		T &Get(const std::string &_tag) { return pool.find(_tag); }
};

class ResourceManager {
	public:
		ResourcePool<Object> objectPool;
		ResourcePool<Texture> texturePool;
};