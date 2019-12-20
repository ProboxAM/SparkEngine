#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"
#include "ResourceBone.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "BoneImporter.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

BoneImporter::BoneImporter()
{
}


BoneImporter::~BoneImporter()
{
}

uint BoneImporter::Import(const char * file, const aiBone * bone, uint id)
{
	ResourceBone* resource = (ResourceBone*)App->resources->CreateResource(Resource::RESOURCE_TYPE::R_ANIMATION, id);
	resource->name = bone->mName.C_Str();
	resource->matrix = float4x4(bone->mOffsetMatrix.a1, bone->mOffsetMatrix.a2, bone->mOffsetMatrix.a3, bone->mOffsetMatrix.a4,
		bone->mOffsetMatrix.b1, bone->mOffsetMatrix.b2, bone->mOffsetMatrix.b3, bone->mOffsetMatrix.b4,
		bone->mOffsetMatrix.c1, bone->mOffsetMatrix.c2, bone->mOffsetMatrix.c3, bone->mOffsetMatrix.c4,
		bone->mOffsetMatrix.d1, bone->mOffsetMatrix.d2, bone->mOffsetMatrix.d3, bone->mOffsetMatrix.d4);

	resource->num_weights = bone->mNumWeights;
	for (uint i = 0; i < resource->num_weights; i++)
	{
		memcpy(&resource->weights[i], &bone->mWeights[i].mWeight, sizeof(float));
		memcpy(&resource->vertex_ids[i], &bone->mWeights[i].mVertexId, sizeof(uint));
	}

	SaveBone(resource);
	resource->UnLoad();
	resource->SetFile(file);

	return resource->GetID();
}

bool BoneImporter::SaveBone(ResourceBone * bone)
{
	uint size = sizeof(uint) + bone->name.size() + sizeof(float4x4) + sizeof(uint) + sizeof(float)*bone->num_weights + sizeof(uint)*bone->num_weights;
	// Allocate
	char* data = new char[size];
	char* cursor = data;

	// Store name size and name
	uint bytes = sizeof(uint);
	uint name_size = bone->name.size();
	memcpy(cursor, &name_size, bytes);
	cursor += bytes;
	bytes = bone->name.size();
	memcpy(cursor, bone->name.c_str(), bytes);
	cursor += bytes;

	//Store matrix
	bytes = sizeof(float4x4);
	memcpy(cursor, &bone->matrix, bytes);
	cursor += bytes;

	//Store num weights
	bytes = sizeof(uint);
	memcpy(cursor, &bone->num_weights, bytes);
	cursor += bytes;

	//Store weights
	bytes = sizeof(float)*bone->num_weights;
	memcpy(cursor, &bone->weights, bytes);
	cursor += bytes;

	//Store vertex ids
	bytes = sizeof(uint)*bone->num_weights;
	memcpy(cursor, &bone->vertex_ids, bytes);
	cursor += bytes;


	std::string file = LIBRARY_BONE_FOLDER;
	file += std::to_string(bone->GetID()) + BONE_EXTENSION;
	uint ret = App->fsystem->Save(file.c_str(), data, size);
	RELEASE_ARRAY(data);

	bone->SetExportedFile(file);

	return true;
}

bool BoneImporter::Load(ResourceBone * resource)
{
	char* buffer;
	uint size = App->fsystem->Load(resource->GetExportedFile(), &buffer);
	char* cursor = buffer;

	//Load name size
	uint bytes = sizeof(uint);
	uint name_size;
	memcpy(&name_size, cursor, bytes);
	cursor += bytes;

	//Load name
	bytes = name_size;
	resource->name.resize(bytes);
	memcpy(&resource->name[0], cursor, bytes);
	cursor += bytes;

	//Load matrix
	bytes = sizeof(float4x4);
	memcpy(&resource->matrix, cursor, bytes);
	cursor += bytes;

	//Load weights num
	bytes = sizeof(uint);
	memcpy(&resource->num_weights, cursor, bytes);
	cursor += bytes;

	resource->weights = new float[resource->num_weights];
	resource->vertex_ids = new uint[resource->num_weights];

	//Load weights
	bytes = sizeof(float)*resource->num_weights;
	memcpy(resource->weights, cursor, bytes);
	cursor += bytes;

	//Load vertex ids
	bytes = sizeof(uint)*resource->num_weights;
	memcpy(resource->vertex_ids, cursor, bytes);
	cursor += bytes;

	LOG("Loaded Bone %s", resource->GetExportedFile());

	RELEASE_ARRAY(buffer);

	return true;
}
