//
// GlyphAtlasPacker.h - Shelf packer that places glyph bitmaps into a fixed-size atlas
//

#pragma once

#include <cstdint>
#include <optional>

namespace Engine
{
	struct AtlasRect
	{
		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;
	};

	class GlyphAtlasPacker
	{
	public:
		GlyphAtlasPacker(uint32_t atlasWidth, uint32_t atlasHeight, uint32_t padding) noexcept
			: m_width(atlasWidth), m_height(atlasHeight), m_padding(padding)
		{
		}

		// Reserves a cell for a bitmap of the given size. Returns nullopt when the atlas is full.
		std::optional<AtlasRect> Place(uint32_t width, uint32_t height) noexcept
		{
			const uint32_t cellWidth = width + m_padding;
			const uint32_t cellHeight = height + m_padding;
			if (cellWidth > m_width || cellHeight > m_height)
			{
				return std::nullopt;
			}

			if (m_cursorX + cellWidth > m_width)
			{
				m_cursorX = 0;
				m_cursorY += m_shelfHeight;
				m_shelfHeight = 0;
			}

			if (m_cursorY + cellHeight > m_height)
			{
				return std::nullopt;
			}

			AtlasRect rect;
			rect.x = m_cursorX;
			rect.y = m_cursorY;
			rect.width = width;
			rect.height = height;

			m_cursorX += cellWidth;
			if (cellHeight > m_shelfHeight)
			{
				m_shelfHeight = cellHeight;
			}
			return rect;
		}

		uint32_t Width() const noexcept { return m_width; }
		uint32_t Height() const noexcept { return m_height; }

	private:
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_padding = 0;
		uint32_t m_cursorX = 0;
		uint32_t m_cursorY = 0;
		uint32_t m_shelfHeight = 0;
	};
}
