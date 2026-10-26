//
// FontData.h - API-neutral glyph metrics for a font, queried by AssetKey through the AssetManager
//

#pragma once

namespace Engine
{
	struct GlyphInfo
	{
		float u0 = 0.0f;
		float v0 = 0.0f;
		float u1 = 0.0f;
		float v1 = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float bearingX = 0.0f;
		float bearingY = 0.0f;
		float advance = 0.0f;
	};

	struct FontMetrics
	{
		float ascender = 0.0f;
		float descender = 0.0f;
		float lineHeight = 0.0f;
	};

	class IFontData
	{
	public:
		virtual ~IFontData() = default;

		// Returns null when the codepoint was not rasterized into the atlas.
		virtual const GlyphInfo* Find(char32_t codepoint) const noexcept = 0;
		virtual const FontMetrics& Metrics() const noexcept = 0;
	};
}
