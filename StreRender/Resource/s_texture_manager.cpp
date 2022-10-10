#include "stre_render.h"
#include "cg_object.h"
#include "Core/Memory/s_memory.h"
#include "Core/File_Manager/s_fbx.h"
#include "cg_resource_factory.h"
////////////////////////////////////////////////////////////////////
//create object

s_memory_allocater_register texture_resource_allocater("texture_resource_allocater");

s_texture* s_texture_manager::create_texture(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["texture_resource_allocater"];

	return allocater->allocate<cg_texture>((cg_resource*)in_resource);

}

/***
************************************************************
*
* Init Resource
*
************************************************************
*/
//资源的构建依赖cg_resource_factory
// 
//cpu资源构建过程
//1.利用reource_factory构建默认的resource
//2.再利用resource 构建对应物体类型
//3.物体类型的构造函数中构建 GPU layout

s_resource* s_texture_manager::create_texture_resource()
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

//按照后缀区分类型，进而读取
s_resource* s_texture_manager::load_local_resource( wchar_t* in_path)
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
