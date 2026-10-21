//
// GltfLoader.cpp
//

#include "pch.h"
#include "GltfLoader.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include <cstring>

namespace Engine
{
	namespace
	{
		std::string ToUtf8(const std::wstring& text)
		{
			std::string result;
			result.reserve(text.size());
			for (wchar_t ch : text)
			{
				result.push_back(ch < 0x80 ? static_cast<char>(ch) : '?');
			}
			return result;
		}

		const cgltf_accessor* FindAttribute(const cgltf_primitive& primitive, cgltf_attribute_type type)
		{
			for (cgltf_size i = 0; i < primitive.attributes_count; ++i)
			{
				if (primitive.attributes[i].type == type)
				{
					return primitive.attributes[i].data;
				}
			}
			return nullptr;
		}
	}

	bool LoadGltf(const std::wstring& path, DecodedModel& model)
	{
		const std::string utf8Path = ToUtf8(path);

		cgltf_options options = {};
		cgltf_data* data = nullptr;
		if (cgltf_parse_file(&options, utf8Path.c_str(), &data) != cgltf_result_success)
		{
			return false;
		}

		bool loaded = false;
		if (cgltf_load_buffers(&options, data, utf8Path.c_str()) == cgltf_result_success &&
			data->meshes_count > 0 && data->meshes[0].primitives_count > 0)
		{
			const cgltf_primitive& primitive = data->meshes[0].primitives[0];
			const cgltf_accessor* position = FindAttribute(primitive, cgltf_attribute_type_position);
			const cgltf_accessor* normal = FindAttribute(primitive, cgltf_attribute_type_normal);
			const cgltf_accessor* texcoord = FindAttribute(primitive, cgltf_attribute_type_texcoord);

			if (position && position->count > 0)
			{
				const cgltf_size vertexCount = position->count;
				model.vertices.assign(vertexCount, DecodedVertex{});

				for (cgltf_size i = 0; i < vertexCount; ++i)
				{
					DecodedVertex& v = model.vertices[i];
					cgltf_accessor_read_float(position, i, v.position, 3);
					if (normal)
					{
						cgltf_accessor_read_float(normal, i, v.normal, 3);
					}
					if (texcoord)
					{
						cgltf_accessor_read_float(texcoord, i, v.uv, 2);
					}
				}

				model.indices.clear();
				if (primitive.indices)
				{
					model.indices.resize(primitive.indices->count);
					for (cgltf_size i = 0; i < primitive.indices->count; ++i)
					{
						model.indices[i] = static_cast<uint32_t>(cgltf_accessor_read_index(primitive.indices, i));
					}
				}
				else
				{
					model.indices.resize(vertexCount);
					for (cgltf_size i = 0; i < vertexCount; ++i)
					{
						model.indices[i] = static_cast<uint32_t>(i);
					}
				}

				loaded = true;
			}
		}

		cgltf_free(data);
		return loaded;
	}
}
