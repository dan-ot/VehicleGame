//
// AssetKit.h - Retained asset set owned by one Core Mode
//

#pragma once

#include "AssetKey.h"
#include "AssetManifest.h"
#include "AssetRequester.h"

#include <unordered_set>

namespace Engine
{
	class AssetKit
	{
	public:
		explicit AssetKit(RequesterId requester) : m_requester(requester) {}

		RequesterId Requester() const noexcept { return m_requester; }

		AssetManifest& Manifest() noexcept { return m_manifest; }
		const AssetManifest& Manifest() const noexcept { return m_manifest; }

		const std::unordered_set<AssetKey>& HeldKeys() const noexcept { return m_held; }

		bool Holds(const AssetKey& key) const { return m_held.find(key) != m_held.end(); }

		void MarkHeld(const AssetKey& key) { m_held.insert(key); }
		void MarkReleased(const AssetKey& key) { m_held.erase(key); }
		void Clear() noexcept { m_held.clear(); }

	private:
		RequesterId m_requester;
		AssetManifest m_manifest;
		std::unordered_set<AssetKey> m_held;
	};
}
