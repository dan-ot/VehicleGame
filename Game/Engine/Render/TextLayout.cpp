//
// TextLayout.cpp
//

#include "pch.h"
#include "TextLayout.h"

namespace
{
	constexpr char32_t kFallbackGlyph = U'?';

	// Decodes one UTF-8 sequence starting at pos and advances pos. Malformed input yields U+FFFD.
	char32_t DecodeUtf8(std::string_view text, size_t& pos)
	{
		const unsigned char lead = static_cast<unsigned char>(text[pos]);
		if (lead < 0x80)
		{
			++pos;
			return lead;
		}

		size_t length = 0;
		char32_t codepoint = 0;
		if ((lead & 0xE0) == 0xC0)
		{
			length = 2;
			codepoint = lead & 0x1F;
		}
		else if ((lead & 0xF0) == 0xE0)
		{
			length = 3;
			codepoint = lead & 0x0F;
		}
		else if ((lead & 0xF8) == 0xF0)
		{
			length = 4;
			codepoint = lead & 0x07;
		}
		else
		{
			++pos;
			return 0xFFFD;
		}

		if (pos + length > text.size())
		{
			++pos;
			return 0xFFFD;
		}

		for (size_t i = 1; i < length; ++i)
		{
			const unsigned char continuation = static_cast<unsigned char>(text[pos + i]);
			if ((continuation & 0xC0) != 0x80)
			{
				++pos;
				return 0xFFFD;
			}
			codepoint = (codepoint << 6) | (continuation & 0x3F);
		}

		pos += length;
		return codepoint;
	}
}

namespace Engine
{
	void LayoutText(std::string_view text, const FontResource& font, const TextStyle& style,
		float originX, float originY, TextQuadList& quads)
	{
		quads.clear();

		const FontMetrics& metrics = font.Metrics();
		const float baseline = originY + metrics.ascender * style.scale;
		const GlyphInfo* fallback = font.Find(kFallbackGlyph);

		float penX = originX;
		size_t pos = 0;
		while (pos < text.size())
		{
			const char32_t codepoint = DecodeUtf8(text, pos);
			const GlyphInfo* glyph = font.Find(codepoint);
			if (!glyph)
			{
				glyph = fallback;
			}
			if (!glyph)
			{
				continue;
			}

			if (glyph->width > 0.0f && glyph->height > 0.0f)
			{
				GlyphQuad quad;
				quad.destination.x = penX + glyph->bearingX * style.scale;
				quad.destination.y = baseline - glyph->bearingY * style.scale;
				quad.destination.width = glyph->width * style.scale;
				quad.destination.height = glyph->height * style.scale;
				quad.uv.u0 = glyph->u0;
				quad.uv.v0 = glyph->v0;
				quad.uv.u1 = glyph->u1;
				quad.uv.v1 = glyph->v1;
				quad.tint = style.tint;
				quads.push_back(quad);
			}

			penX += (glyph->advance + style.spacing) * style.scale;
		}
	}
}
