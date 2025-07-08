#pragma once

#include "OMEngine/Base.hpp"
#include <unordered_map>
#include <queue>
#include <mutex>
#include "OMEngine/Resource/IResource.hpp"

namespace OM::Core
{
	/*
	class OM_ENGINE_API ResourceManager
	{
	public:
		static ResourceManager* GetInstance();

		Resource::IResource* UseResource(const char* path, const Resource::EResourceType resourceType);
		void StopUsedResource(Resource::IResource* resource);

		Resource::IResource* GetResourceToLoad();

	private:
		static inline ResourceManager* _instance = nullptr;

		std::unordered_map<const char*, Resource::IResource*> _resources;
		std::mutex _mutexResourceToLoad;
		std::queue<Resource::IResource*> _resourcesToLoad;

		Resource::IResource* GetResource(const char* path);
		Resource::IResource* CreateResource(const char* path, const Resource::EResourceType resourceType);
		void AddResourceToLoad(Resource::IResource* resource);
		void DeleteResource(Resource::IResource* resource);
	};
	*/
}