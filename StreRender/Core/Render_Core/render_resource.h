#pragma once
#include "gpu_resource.h"

/// <summary>
/// ģ����Դ���������ͣ��������� ���� ���ʵȵ���ģ�Ͱ󶨵���Դ��Ҳ������Ϊ��Ļ�ռ���Ƶ��������ͣ�ֻ����4������������������κζ������ݣ���
/// </summary>
struct mesh_shader_resource
{

	std::vector<const gpu_shader_resource*> sr_res;
};

/// <summary>
/// ��װs_pass������֡����
/// </summary>
class render_pass : public s_pass
{

};
