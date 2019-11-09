#include "Resource.h"



Resource::Resource()
{
}


Resource::~Resource()
{
}

Resource::Resource(uint id, RESOURCE_TYPE type)
{
}

RESOURCE_TYPE Resource::GetType() const
{
	return type;
}

uint Resource::GetID() const
{
	return id;
}

const char * Resource::GetFile() const
{
	return file.c_str();
}

const char * Resource::GetExportedFile() const
{
	return exported_file.c_str();
}

bool Resource::IsLoaded() const
{
	return loaded;
}

bool Resource::Load()
{
	return false;
}

uint Resource::CountReferences() const
{
	return uint();
}
