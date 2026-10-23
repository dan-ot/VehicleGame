//
// TextLayout.h - Converts UTF-8 text into positioned glyph quads using a FontResource atlas
//

#pragma once

#include "../Assets/FontResource.h"
#include "QuadRenderer.h"

#include <string_view>
#include <vector>

namespace Engine
{
	struct TextStyle
	{
		float scale = 1.0f;
		ColorF tint;
		float spacing = 0.0f;
	};

	struct GlyphQuad
	{
		RectF destination;
		UvRectF uv;
		ColorF tint;
	};

	using TextQuadList = std::vector<GlyphQuad>;

	// Appends one quad per visible glyph to 'quads' after clearing it. originX is the pen start,
	// originY is the top of the line. Characters missing from the atlas fall back to '?'.
	void LayoutText(std::string_view text, const FontResource& font, const TextStyle& style,
		float originX, float originY, TextQuadList& quads);
}
