#pragma once
#include <map>
#include <string>
#include "gpu_resource.h"
class gpu_resource_factory
{
private:
	std::map<std::string, gpu_resource*> gpu_resource_group;

public:
	void create_gpu_resource();
};