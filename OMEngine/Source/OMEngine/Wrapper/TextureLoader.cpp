#include "pch.h"

#include "OMEngine/Wrapper/TextureLoader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace OM::Wrapper
{
	unsigned char* LoadTexture(const char* path, int* witdh, int* height, int* channels)
	{
		unsigned char* data = stbi_load(path, witdh, height, channels, 0);
		return data;
	}

	void ReleaseTexture(unsigned char* data)
	{
		stbi_image_free(data);
	}
}