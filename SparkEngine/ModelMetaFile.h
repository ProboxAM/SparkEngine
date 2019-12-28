#include "MetaFile.h"
class AnimationMetaFile;

class ModelMetaFile : public MetaFile {
public:
	ModelMetaFile();
	~ModelMetaFile();

	//Resource ids
	std::vector<uint> meshes;
	std::vector<AnimationMetaFile*> animations;
	std::map<std::string, uint> bones;

	bool import_animation = false;
	uint max_ticks = 0;

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
