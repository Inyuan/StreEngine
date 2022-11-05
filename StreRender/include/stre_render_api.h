#pragma once 
#include "cpu_resource.h"
#include "gpu_resource.h"
#include "d3dx12.h"
/***
************************************************************
*
* Renderer
*
************************************************************
*/

/// <summary>
/// 封装的directx图形接口，外部控制GPU的基本类型，允许外部更进一步自定义控制内存分配，pass构建等操作。
/// </summary>
class s_directx_render
{
public:
	virtual gpu_pass* allocate_pass() = 0;

	virtual gpu_shader_resource* allocate_shader_resource(
		gpu_shader_resource::SHADER_RESOURCE_TYPE in_shader_res_type) = 0;

	virtual void allocate_upload_resource(
		gpu_shader_resource* in_res_elem,
		UINT in_elem_size,
		UINT in_number) = 0;

	virtual void allocate_default_resource(
		gpu_shader_resource* in_res_elem,
		UINT in_elem_size,
		UINT in_number,
		void* in_cpu_data) = 0;

	virtual void package_textures(
		std::vector<const gpu_shader_resource*>& in_texture_group,
		gpu_shader_resource* in_out_table) = 0;

	virtual void load_rootparpameter(
		std::vector<CD3DX12_ROOT_PARAMETER>& in_out_root_parameter,
		const gpu_shader_resource* in_gpu_sr) = 0;

	virtual void create_rootsignature(
		CD3DX12_ROOT_SIGNATURE_DESC& in_rootsig_desc,
		gpu_pass* in_gpu_pass) = 0;

	virtual void create_pso(
		shader_layout in_shader_layout,
		gpu_pass* in_gpu_pass,
		UINT in_rt_number,
		bool is_translate = false) = 0;


	/// <summary>
	/// 刷新自定义资源
	/// 所有自定义资源被使用的都是同步的current_frame索引
	/// </summary>
	/// <param name="in_out_gpu_res">gpu资源指针</param>
	/// <param name="in_data">数据块</param>
	/// <param name="in_update_element_index">刷新元素起始位置</param>
	/// <param name="int_update_element_count">刷新元素个数</param>
	virtual void update_gpu_resource(
		gpu_shader_resource* in_out_gpu_res,
		const void* in_data,
		UINT in_update_element_index,
		size_t int_update_element_count) = 0;


};