//
// TextBlock.h - Cached glyph-quad list for text that changes infrequently
//

#pragma once

#include "../Assets/AssetKey.h"
#include "QuadRenderer.h"
#include "TextLayout.h"

#include <string>
#include <string_view>

namespace Engine
{
	class TextBlock
	{
	public:
		// Re-lays out the text only when the text, font, style, or origin differ from the last call.
		void SetText(std::string_view text, const AssetKey& fontKey, const IFontData& font, const TextStyle& style, float originX, float originY)
		{
			if (m_valid && m_fontKey == fontKey && m_text == text && SameStyle(m_style, style) &&
				m_originX == originX && m_originY == originY)
			{
				return;
			}

			m_text.assign(text);
			m_fontKey = fontKey;
			m_style = style;
			m_originX = originX;
			m_originY = originY;
			LayoutText(m_text, font, style, originX, originY, m_quads);
			m_valid = true;
		}

		void Draw(Renderer& renderer) const
		{
			if (!m_valid)
			{
				return;
			}

			for (const GlyphQuad& quad : m_quads)
			{
				renderer.Draw(m_fontKey, quad.destination, quad.uv, quad.tint);
			}
		}

		const TextQuadList& Quads() const noexcept { return m_quads; }

	private:
		static bool SameStyle(const TextStyle& a, const TextStyle& b) noexcept
		{
			return a.scale == b.scale && a.spacing == b.spacing &&
				a.tint.r == b.tint.r && a.tint.g == b.tint.g && a.tint.b == b.tint.b && a.tint.a == b.tint.a;
		}

		std::string m_text;
		AssetKey m_fontKey;
		TextStyle m_style;
		float m_originX = 0.0f;
		float m_originY = 0.0f;
		TextQuadList m_quads;
		bool m_valid = false;
	};
}
