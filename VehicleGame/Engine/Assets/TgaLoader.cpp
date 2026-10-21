//
// TgaLoader.cpp
//

#include "pch.h"
#include "TgaLoader.h"

#include <fstream>

namespace Engine
{
	bool LoadTga(const std::wstring& path, DecodedImage& image)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return false;
		}

		uint8_t header[18] = {};
		file.read(reinterpret_cast<char*>(header), sizeof(header));
		if (!file)
		{
			return false;
		}

		const uint8_t idLength = header[0];
		const uint8_t colorMapType = header[1];
		const uint8_t imageType = header[2];
		const uint16_t width = static_cast<uint16_t>(header[12] | (header[13] << 8));
		const uint16_t height = static_cast<uint16_t>(header[14] | (header[15] << 8));
		const uint8_t bpp = header[16];
		const uint8_t descriptor = header[17];

		if (colorMapType != 0 || imageType != 2 || bpp != 32 || width == 0 || height == 0)
		{
			return false;
		}

		file.seekg(idLength, std::ios::cur);

		const size_t byteCount = static_cast<size_t>(width) * height * 4;
		std::vector<uint8_t> bgra(byteCount);
		file.read(reinterpret_cast<char*>(bgra.data()), static_cast<std::streamsize>(byteCount));
		if (!file)
		{
			return false;
		}

		const bool topDown = (descriptor & 0x20) != 0;

		image.width = width;
		image.height = height;
		image.pixelsRgba8.resize(byteCount);

		for (uint32_t row = 0; row < height; ++row)
		{
			const uint32_t destRow = topDown ? row : (height - 1 - row);
			for (uint32_t col = 0; col < width; ++col)
			{
				const size_t src = (static_cast<size_t>(row) * width + col) * 4;
				const size_t dst = (static_cast<size_t>(destRow) * width + col) * 4;
				image.pixelsRgba8[dst + 0] = bgra[src + 2];
				image.pixelsRgba8[dst + 1] = bgra[src + 1];
				image.pixelsRgba8[dst + 2] = bgra[src + 0];
				image.pixelsRgba8[dst + 3] = bgra[src + 3];
			}
		}

		return true;
	}
}
