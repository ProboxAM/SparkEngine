#include "ResourceModel.h"
#include "Assimp/include/postprocess.h"

ResourceModel::ResourceModel(uint id) : Resource(id, Resource::RESOURCE_TYPE::R_MODEL)
{

}


ResourceModel::~ResourceModel()
{

}

uint ResourceModel::ModelMetaFile::GetImportSettings()
{
	uint flags = 0;
	switch (setting)
	{
	case MAX_QUALITY:
		flags |= aiProcessPreset_TargetRealtime_Fast;
		break;
	case QUALITY:
		flags |= aiProcessPreset_TargetRealtime_Quality;
		break;
	case FAST:
		flags |= aiProcessPreset_TargetRealtime_MaxQuality;
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

void ResourceModel::ModelMetaFile::SetImportSettings(MODEL_IMPORT_SETTING set)
{
	switch (set)
	{
		case MAX_QUALITY:
			setting = set;
			break; 
		case QUALITY:
			setting = set;
			break; 
		case FAST:
			setting = set;
			break; 
		case CUSTOM:
			setting = set;
			break;
	}
}
