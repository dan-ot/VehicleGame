//
// AssetManifest.h - The set of assets a mode declares it needs
//

#pragma once

#include "AssetKey.h"

#include <vector>

namespace Engine
{
	class AssetManifest
	{
	public:
		void Add(AssetType type, std::wstring path)
		{
			m_keys.push_back(MakeAssetKey(type, std::move(path)));
		}

		void Clear() noexcept { m_keys.clear(); }

		const std::vector<AssetKey>& Keys() const noexcept { return m_keys; }

	private:
		std::vector<AssetKey> m_keys;
	};
}
