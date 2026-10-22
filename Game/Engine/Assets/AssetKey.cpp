//
// AssetKey.cpp
//

#include "pch.h"
#include "AssetKey.h"

namespace Engine
{
	AssetKey MakeAssetKey(AssetType type, std::wstring path)
	{
		std::wstring normalized;
		normalized.reserve(path.size());

		bool lastWasSeparator = false;
		for (wchar_t ch : path)
		{
			if (ch == L'/')
			{
				ch = L'\\';
			}

			if (ch == L'\\')
			{
				if (lastWasSeparator)
				{
					continue;
				}
				lastWasSeparator = true;
			}
			else
			{
				lastWasSeparator = false;
			}

			if (ch >= L'A' && ch <= L'Z')
			{
				ch = static_cast<wchar_t>(ch - L'A' + L'a');
			}

			normalized.push_back(ch);
		}

		return AssetKey{ type, std::move(normalized) };
	}
}
