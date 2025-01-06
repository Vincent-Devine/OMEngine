#include "pch.h"

#include "OMEngine/Resource/IResource.hpp"
#include <filesystem>

namespace OM::Resource
{
	void IResource::CreateNameFromFile()
	{
		if (_path == "")
			return;

		_name = std::filesystem::path(_path).filename().string().c_str();
	}
}