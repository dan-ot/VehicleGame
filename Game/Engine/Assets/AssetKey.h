//
// AssetKey.h - Identity of an engine asset: normalized source path plus asset type
//

#pragma once

#include <cstddef>
#include <functional>
#include <string>

namespace Engine
{
	enum class AssetType : unsigned char
	{
		Texture,
		Model,
		Sound,
		Music,
		Shader,
		Font,
	};

	struct AssetKey
	{
		AssetType type = AssetType::Texture;
		std::wstring path;

		bool operator==(const AssetKey& other) const noexcept
		{
			return type == other.type && path == other.path;
		}

		bool operator!=(const AssetKey& other) const noexcept
		{
			return !(*this == other);
		}
	};

	// Normalizes separators to backslashes, lowercases ASCII, and collapses redundant separators
	// so that equivalent source paths map to the same key.
	AssetKey MakeAssetKey(AssetType type, std::wstring path);
}

namespace std
{
	template <>
	struct hash<Engine::AssetKey>
	{
		size_t operator()(const Engine::AssetKey& key) const noexcept
		{
			const size_t h = hash<wstring>{}(key.path);
			return h ^ (static_cast<size_t>(key.type) + 0x9e3779b9u + (h << 6) + (h >> 2));
		}
	};
}
