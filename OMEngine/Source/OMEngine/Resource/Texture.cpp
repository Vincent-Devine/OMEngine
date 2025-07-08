#include "pch.h"

#include "OMEngine/Resource/Texture.hpp"
#include <OMLogger/Logger.hpp>

namespace OM::Resource
{
	Texture::Texture(const int id, const char* path)
		: _witdh(0), _height(0), _channels(0), _data(), _idTexture(0), _type("default type")
	{
		_path = path;
		_id = id;
		_isInitialisate = false;
		CreateNameFromFile();
	}

	const bool Texture::LoadFromFile()
	{
		_isInitialisate = false;
		// Load data from wrapper
		if (!_data)
		{
			OM_LOG_WARNING("texture[" + std::to_string(_id) + "] " + _name + " failed to load.");
			return false;
		}

		OM_LOG_DEBUG("finish to load texture[" + std::to_string(_id) + "] " + _name);
		return true;
	}

	const bool Texture::LoadInRHI()
	{
		// Call RHI
		// Realese texture
		_isInitialisate = true;
		return true;
	}

	void Texture::Draw(const unsigned int shaderProgram) const
	{
		if (!_isInitialisate)
			return;

		// Call RHI
	}
}