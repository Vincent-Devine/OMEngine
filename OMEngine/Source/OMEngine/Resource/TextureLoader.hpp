#pragma once

namespace OM::Wrapper
{
	unsigned char* LoadTexture(const char* path, int* witdh, int* height, int* channels);
	void ReleaseTexture(unsigned char* data);
}