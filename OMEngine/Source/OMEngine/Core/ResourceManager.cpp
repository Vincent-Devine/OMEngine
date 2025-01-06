#include "pch.h"

#include "OMEngine/Core/ResourceManager.hpp"
#include <OMLogger/Logger.hpp>
#include "OMEngine/Core/ThreadPool.hpp"
#include "OMEngine/Resource/Texture.hpp"

// Call by Thread
void LoadResource()
{
	OM::Resource::IResource* resourceToLoad = OM::Core::ResourceManager::GetInstance()->GetResourceToLoad();
	if (!resourceToLoad)
		return;

	resourceToLoad->LoadFromFile();
	// Load in RHI
}

namespace OM::Core
{
	ResourceManager* ResourceManager::GetInstance()
	{
		if (!_instance)
			_instance = new ResourceManager();
		return _instance;
	}

	Resource::IResource* ResourceManager::UseResource(const char* path, const Resource::EResourceType resourceType)
	{
		Resource::IResource* resource = nullptr;
		resource = GetResource(path);

		if (!resource)
		{
			resource = CreateResource(path, resourceType);
			AddResourceToLoad(resource);
		}

		resource->StartUsing();
		return resource;
	}

	void ResourceManager::StopUsedResource(Resource::IResource* resource)
	{
		resource->StopUsing();
		if (resource->GetUsedBy() == 0)
			DeleteResource(resource);
	}

	Resource::IResource* ResourceManager::GetResourceToLoad()
	{
		if (_resourcesToLoad.empty())
			return nullptr;

		if (_mutexResourceToLoad.try_lock())
		{
			if (_resourcesToLoad.empty())
			{
				_mutexResourceToLoad.unlock();
				return nullptr;
			}

			Resource::IResource* resourceToLoad = _resourcesToLoad.front();
			_resourcesToLoad.pop();
			_mutexResourceToLoad.unlock();
			return resourceToLoad;
		}
		return nullptr;
	}

	Resource::IResource* ResourceManager::GetResource(const char* path)
	{
		Resource::IResource* resource = nullptr;

		if (_resources.count(path) != 0)
			resource = static_cast<Resource::IResource*>(_resources.find(path)->second);

		return resource;
	}

	Resource::IResource* ResourceManager::CreateResource(const char* path, const Resource::EResourceType resourceType)
	{
		const unsigned int id = _resources.size();
		Resource::IResource* resource = nullptr;

		switch (resourceType)
		{
		case Resource::EResourceType::RT_Texture:
			resource = static_cast<Resource::IResource*>(new Resource::Texture(id, path));
			break;

		default:
			break;
		}

		auto [it, _] = _resources.insert_or_assign(path, resource);
		LOG_DEBUG("Add resource: " + std::string(path) + " in resource manager.");
		return static_cast<Resource::IResource*>(it->second);
	}

	void ResourceManager::AddResourceToLoad(Resource::IResource* resource)
	{
		if (_mutexResourceToLoad.try_lock())
		{
			_resourcesToLoad.push(resource);
			_mutexResourceToLoad.unlock();
		}
		Core::ThreadPool::GetInstance()->AddTask([]() { LoadResource(); });
	}

	void ResourceManager::DeleteResource(Resource::IResource* resource)
	{
		resource->Unload();
		_resources.erase(resource->GetPath());
		LOG_DEBUG("Unload and delete resource: " + std::string(resource->GetName()));
		delete resource;
	}
}