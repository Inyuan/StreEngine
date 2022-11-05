#pragma once
#include "gpu_resource.h"

/// <summary>
/// 模型资源的输入类型，包括顶点 索引 材质等等与模型绑定的资源。也可以作为屏幕空间绘制的输入类型（只输入4个顶点和索引并不带任何额外数据）。
/// </summary>
struct mesh_shader_resource
{

	std::vector<const gpu_shader_resource*> sr_res;
};

/// <summary>
/// 封装s_pass，控制帧内容
/// </summary>
class render_pass : public s_pass
{

};
