#pragma once

#include "OMEngine/Base.hpp"

namespace OM::Resource
{
	class OM_ENGINE_API IResource
	{
	public:
		virtual ~IResource() {}
		virtual const bool LoadFromFile() { return true; }
		virtual const bool LoadInRHI() { return true; }
		virtual void Unload() {}

		virtual void StartUsing() { _usedBy++; }
		virtual void StopUsing() { _usedBy--; }

		virtual const char* GetName() const { return _name; }
		virtual const char* GetPath() const { return _path; }
		virtual const bool GetIsInitilisate() const { return _isInitialisate; }
		virtual const unsigned int GetUsedBy() const { return _usedBy; }

		virtual void SetName(const char* name) { _name = name; }

	protected:
		const char* _name = "default name";
		const char* _path = "default path";

		int _id = -1;
		bool _isInitialisate = false;
		unsigned int _usedBy = 0;

		virtual void CreateNameFromFile();
	};

	enum EResourceType
	{
		RT_Texture,
		RT_Mesh,
		RT_VertexShader,
		RT_FragmentShader,
	};
}