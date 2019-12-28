#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

#include "Importer.h"

#include "ResourceModel.h"
#include "ResourceAnimation.h"
#include "nlohmann\json.hpp"
#include <map>

class aiNode;
class aiScene;
class ModelMetaFile;
class AnimationMetaFile;

class ModelImporter :
	public Importer
{
public:
	ModelImporter();
	~ModelImporter();

	bool Init();
	bool CleanUp();

	bool Import(const char * file, std::string & output_file, ModelMetaFile*& meta);
	bool Load(ResourceModel* resource);
	bool Save(std::string file, const std::vector<ResourceModel::ModelNode>& nodes);

	bool SaveMeta(ModelMetaFile* meta);
	bool LoadMeta(const char* file, ModelMetaFile* meta);

private:
	bool LoadNode(nlohmann::json::iterator it, ResourceModel* resource);
	void ImportNode(const aiNode * node, const aiScene * scene, uint parent_id, const char* file, std::vector<ResourceModel::ModelNode>& nodes);
	void FixScaleUnits(float3 &scale);

private:
	std::vector<uint> meshes;
	std::map<std::string, uint> bones;
	std::vector<AnimationMetaFile*> animations;
};

#endif // !_MODEL_IMPORTER_H_