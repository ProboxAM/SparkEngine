#include "Resource.h"

Resource::~Resource()
{
}

Resource::Resource(uint id, RESOURCE_TYPE type)
{
	this->id = id;
	this->type = type;
}

Resource::RESOURCE_TYPE Resource::GetType() const
{
	return type;
}

uint Resource::GetID() const
{
	return id;
}

const char* Resource::GetFile() const
{
	return file.c_str();
}

const char * Resource::GetExportedFile() const
{
	return exported_file.c_str();
}

bool Resource::IsLoaded() const
{
	return references>0;
}

void Resource::AddReference()
{
	references++;
}

void Resource::RemoveReference()
{
	references--;
	if (references == 0)
	{
		UnLoad();
	}
}

void Resource::SetFile(std::string f)
{
	file = f;
}

void Resource::SetExportedFile(std::string f)
{
	exported_file = f;
}
