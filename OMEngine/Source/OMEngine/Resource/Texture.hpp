#pragma once

#include "OMEngine/Base.hpp"
#include "OMEngine/Resource/IResource.hpp"

namespace OM::Resource
{
	class OM_ENGINE_API Texture : public IResource
	{
	public:
		Texture(const int id, const char* path);

		const bool LoadFromFile() override;
		const bool LoadInRHI() override;

		void Draw(const unsigned int shaderProgram) const;

	private:
		int _witdh, _height, _channels;
		unsigned char* _data;
		unsigned int _idTexture;
		const char* _type;
	};
}