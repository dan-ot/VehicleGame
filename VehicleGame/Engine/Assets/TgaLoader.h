//
// TgaLoader.h - Hand-parsed uncompressed 32-bit TGA (type 2) reader
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Engine
{
	struct DecodedImage
	{
		uint32_t width = 0;
		uint32_t height = 0;
		std::vector<uint8_t> pixelsRgba8;
	};

	// Reads an uncompressed, true-colour 32-bit TGA file. Rows are returned top-down as RGBA8.
	bool LoadTga(const std::wstring& path, DecodedImage& image);
}
