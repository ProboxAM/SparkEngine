#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

#include "Importer.h"
#include "ResourceModel.h"
#include "nlohmann\json.hpp"

class aiNode;
class aiScene;

class ModelImporter :
	public Importer
{
public:
	ModelImporter();
	~ModelImporter();

	bool Init();
	bool CleanUp();

	bool Import(const char* file, std::string& output_file, uint id);
	bool Load(ResourceModel* resource);
	bool Save(std::string file, const std::vector<ResourceModel::ModelNode>& nodes);
	bool CreateMeta(std::string file, uint id, const std::vector<ResourceModel::ModelNode>& nodes);

private:
	bool LoadNode(nlohmann::json::iterator it, ResourceModel* resource);
	void ImportNode(const aiNode* node, const aiScene* scene, uint parent_id, std::vector<ResourceModel::ModelNode>& nodes, std::vector<uint> meshes_id);
	void GetMeshesID(std::string file, std::vector<uint>& meshes_id);
	void FixScaleUnits(float3 &scale);
};

#endif // !_MODEL_IMPORTER_H_


