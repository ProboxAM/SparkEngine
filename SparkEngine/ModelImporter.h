#ifndef _MODEL_IMPORTER_H_
#define _MODEL_IMPORTER_H_

#include "Importer.h"
#include "ResourceModel.h"

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
	bool Load(const char* exported_file);
	bool Save(std::string file, std::vector<ResourceModel::ModelNode> nodes);

private:
	void ImportNode(const aiNode* node, const aiScene* scene, uint parent_id, std::vector<ResourceModel::ModelNode>& nodes);
	void FixScaleUnits(float3 &scale);
};

#endif // !_MODEL_IMPORTER_H_



