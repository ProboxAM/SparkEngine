#ifndef _RESOURCE_MODEL_H_
#define _RESOURCE_MODEL_H_

#include "Resource.h"
#include "MathGeoLib\Math\Quat.h"
#include <map>

class ResourceModel : public Resource
{
public:
	struct ModelNode
	{
		std::string name;
		float3 position;
		float3 scale;
		Quat rotation;

		uint parent;
		uint mesh = 0;
		uint texture = 0;
		uint bone = 0;
	};

	struct ModelMetaFile : public MetaFile
	{
		std::vector<uint> meshes;
		std::vector<uint> animations;
		std::map<std::string, uint> bones;

		enum MODEL_IMPORT_SETTING { MAX_QUALITY, QUALITY, FAST, CUSTOM };
		MODEL_IMPORT_SETTING setting = MAX_QUALITY;

		bool find_instances = true;
		bool validate_structures = true;
		bool optimize_meshes = true;
		bool calc_tangent_space = true;
		bool gen_smooth_normals = true;
		bool join_vertices = true;
		bool improve_cache_locality = true;
		bool limit_bone_weigths = true;
		bool remove_redundant_mats = true;
		bool split_large_meshes = true;
		bool triangulate = true;
		bool gen_uv_coords = true;
		bool sort_by_type = true;
		bool find_degenerates = true;
		bool find_invalid_data = true;

		uint GetImportSettings();
		void SetImportSettings(MODEL_IMPORT_SETTING setting);
		uint GetSelectedPreset() {
			return setting;
		}
		void SetDefault() {
			SetImportSettings(MAX_QUALITY);
		}
	};

public:
	ResourceModel(uint id);
	~ResourceModel();

	void UnLoad();
	void ReLoad();
	void Load();

	std::vector<ModelNode> nodes;
	std::string GetTypeString() const;
};


#endif // !_RESOURCE_MODEL_H_

