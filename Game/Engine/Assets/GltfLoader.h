//
// GltfLoader.h - glTF / GLB model reader (cgltf), compatible with Blender export
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Engine
{
	struct DecodedVertex
	{
		float position[3];
		float normal[3];
		float uv[2];
	};

	struct DecodedModel
	{
		std::vector<DecodedVertex> vertices;
		std::vector<uint32_t> indices;
	};

	// Reads the first mesh primitive of a .gltf or .glb file, expanding indices to 32-bit.
	bool LoadGltf(const std::wstring& path, DecodedModel& model);
}
