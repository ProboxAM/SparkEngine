#include "Assimp/include/postprocess.h"
#include "ModelMetaFile.h"



ModelMetaFile::ModelMetaFile()
{
}


ModelMetaFile::~ModelMetaFile()
{
}

uint ModelMetaFile::GetImportSettings()
{
	uint flags = 0;
	switch (setting)
	{
	case MAX_QUALITY:
		flags |= aiProcessPreset_TargetRealtime_MaxQuality;
		break;
	case QUALITY:
		flags |= aiProcessPreset_TargetRealtime_Quality;
		break;
	case FAST:
		flags |= aiProcessPreset_TargetRealtime_Fast;
		break;
	case CUSTOM:
		if (calc_tangent_space)
			flags |= aiPostProcessSteps::aiProcess_CalcTangentSpace;
		if (gen_smooth_normals)
			flags |= aiPostProcessSteps::aiProcess_GenSmoothNormals;
		if (join_vertices)
			flags |= aiPostProcessSteps::aiProcess_JoinIdenticalVertices;
		if (triangulate)
			flags |= aiPostProcessSteps::aiProcess_Triangulate;
		if (gen_uv_coords)
			flags |= aiPostProcessSteps::aiProcess_GenUVCoords;
		if (sort_by_type)
			flags |= aiPostProcessSteps::aiProcess_SortByPType;
		if (improve_cache_locality)
			flags |= aiPostProcessSteps::aiProcess_ImproveCacheLocality;
		if (limit_bone_weigths)
			flags |= aiPostProcessSteps::aiProcess_LimitBoneWeights;
		if (remove_redundant_mats)
			flags |= aiPostProcessSteps::aiProcess_RemoveRedundantMaterials;
		if (split_large_meshes)
			flags |= aiPostProcessSteps::aiProcess_SplitLargeMeshes;
		if (find_degenerates)
			flags |= aiPostProcessSteps::aiProcess_FindDegenerates;
		if (find_invalid_data)
			flags |= aiPostProcessSteps::aiProcess_FindInvalidData;
		if (find_instances)
			flags |= aiPostProcessSteps::aiProcess_FindInstances;
		if (validate_structures)
			flags |= aiPostProcessSteps::aiProcess_ValidateDataStructure;
		if (optimize_meshes)
			flags |= aiPostProcessSteps::aiProcess_OptimizeMeshes;
		break;
	}

	return flags;
}

void ModelMetaFile::SetImportSettings(MODEL_IMPORT_SETTING set)
{
	switch (set)
	{
	case MAX_QUALITY:
		setting = set;
		find_instances = true;
		validate_structures = true;
		optimize_meshes = true;
		calc_tangent_space = true;
		gen_smooth_normals = true;
		join_vertices = true;
		improve_cache_locality = true;
		limit_bone_weigths = true;
		remove_redundant_mats = true;
		split_large_meshes = true;
		triangulate = true;
		gen_uv_coords = true;
		sort_by_type = true;
		find_degenerates = true;
		find_invalid_data = true;
		break;
	case QUALITY:
		setting = set;
		find_instances = false;
		validate_structures = false;
		optimize_meshes = false;
		calc_tangent_space = true;
		gen_smooth_normals = true;
		join_vertices = true;
		improve_cache_locality = true;
		limit_bone_weigths = true;
		remove_redundant_mats = true;
		split_large_meshes = true;
		triangulate = true;
		gen_uv_coords = true;
		sort_by_type = true;
		find_degenerates = true;
		find_invalid_data = true;
		break;
	case FAST:
		setting = set;
		find_instances = false;
		validate_structures = false;
		optimize_meshes = false;
		calc_tangent_space = true;
		gen_smooth_normals = false;
		join_vertices = true;
		improve_cache_locality = false;
		limit_bone_weigths = false;
		remove_redundant_mats = false;
		split_large_meshes = false;
		triangulate = true;
		gen_uv_coords = true;
		sort_by_type = true;
		find_degenerates = false;
		find_invalid_data = false;
		break;
	case CUSTOM:
		setting = set;
		break;
	}
}