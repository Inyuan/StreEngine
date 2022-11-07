#include "resource_factory.h"

cpu_sence* sence_manager::create_resource()
{
	auto instance = new cpu_sence();
	generate_unique_identifier<cpu_sence>(instance->uid);
	return instance;
}


cpu_sence* sence_manager::load_resource(wchar_t* in_path)
{
	return nullptr;
}

void sence_manager::update_gpu(cpu_sence* in_cpu_data)
{

}