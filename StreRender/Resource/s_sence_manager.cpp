#include "stre_render.h"
#include "cg_object.h"
#include "Core/Memory/s_memory.h"
#include "Core/File_Manager/s_fbx.h"
#include "cg_resource_factory.h"
////////////////////////////////////////////////////////////////////
//create object

s_memory_allocater_register sence_resource_allocater("sence_resource_allocater");

s_camera* s_sence_manager::create_camera(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["sence_resource_allocater"];

	return allocater->allocate<cg_camera>((cg_resource*)in_resource);

}
s_light* s_sence_manager::create_light(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["sence_resource_allocater"];

	return allocater->allocate<cg_light>((cg_resource*)in_resource);

}
s_sence* s_sence_manager::create_sence(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["sence_resource_allocater"];

	return allocater->allocate<cg_sence>((cg_resource*)in_resource);

}

/***
************************************************************
*
* Init Resource
*
************************************************************
*/
//��Դ�Ĺ�������cg_resource_factory
// 
//cpu��Դ��������
//1.����reource_factory����Ĭ�ϵ�resource
//2.������resource ������Ӧ��������
//3.�������͵Ĺ��캯���й��� GPU layout

s_resource* s_sence_manager::create_camera_resource()
{

}

s_resource* s_sence_manager::create_light_resource()
{

}

s_resource* s_sence_manager::create_sence_resource()
{

}



////////////////////////////////////////////////////////////////////
//read resource

/***
************************************************************
*
* Auxiliary Function
*
************************************************************
*/


/***
************************************************************
*
* Read Resource
*
************************************************************
*/

//���պ�׺�������ͣ�������ȡ
s_resource* s_sence_manager::load_local_resource( wchar_t* in_path)
{

}


////////////////////////////////////////////////////////////////////


/***
************************************************************
*
* Update Resource
*
************************************************************
*/
