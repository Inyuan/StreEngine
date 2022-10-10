#include "stre_render.h"
#include "cg_object.h"
#include "Core/Memory/s_memory.h"
#include "Core/File_Manager/s_fbx.h"
#include "cg_resource_factory.h"
////////////////////////////////////////////////////////////////////
//create object

s_memory_allocater_register material_resource_allocater("material_resource_allocater");

s_material* s_material_manager::create_material(s_resource* in_resource)
{
	auto allocater = memory_allocater_group["material_resource_allocater"];

	return allocater->allocate<cg_material>((cg_resource*)in_resource);

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


s_resource* s_material_manager::create_material_resource()
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
s_resource* s_material_manager::load_local_resource( wchar_t* in_path)
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

void s_material_manager::change_material_texture()
{

}