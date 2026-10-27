//
// SoundResource.cpp
//

#include "pch.h"
#include "SoundResource.h"

namespace Engine
{
	SoundResource::SoundResource(std::wstring sourcePath)
		: m_sourcePath(std::move(sourcePath))
	{
	}

	bool SoundResource::Rebuild(DX::DeviceResources&, ID3D12GraphicsCommandList*)
	{
		return Load();
	}

	bool SoundResource::Load()
	{
		if (m_loaded)
		{
			return true;
		}

		m_loaded = LoadWav(m_sourcePath, m_sound);
		return m_loaded;
	}
}
