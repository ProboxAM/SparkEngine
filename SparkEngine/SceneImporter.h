#pragma once
#include "Importer.h"

class GameObject;
class aiNode;
class aiScene;

class SceneImporter :
	public Importer
{
public:
	SceneImporter();
	~SceneImporter();

	bool Init();
	bool CleanUp();

	bool Import(const char* file);

	bool Load(const char* exported_file, GameObject* resource);

private:
	void LoadNode(const aiNode* node, const aiScene* scene, GameObject* parent);
	void FixScaleUnits(float3 &scale);
};

