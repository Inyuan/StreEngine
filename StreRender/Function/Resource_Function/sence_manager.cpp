#include "resource_factory.h"

cpu_sence* sence_manager::create_resource()
{
	return new cpu_sence();
}


cpu_sence* sence_manager::load_resource(wchar_t* in_path)
{
	return nullptr;
}

void sence_manager::update_gpu(cpu_sence* in_cpu_data)
{

}