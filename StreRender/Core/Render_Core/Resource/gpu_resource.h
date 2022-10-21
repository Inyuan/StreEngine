#pragma once
#include <wrl.h>
#include "Core/Render_Core/Render_API/d3dx12.h"
#include "stre_render.h"
#include "Resource/cg_object.h"
#include <vector>
using Microsoft::WRL::ComPtr;

// �Զ�����ͼֻ����һ��gpu_resource_element
// ֻ��mesh���ж��� texture ��Ϊ gpu_resource_element
class gpu_resource
{
public:
	std::string name;
	//����GPU_RESOURCE_TYPE�ֱ𱣴�����
	std::vector<gpu_resource_element*> gpu_resource_group[GPU_RESOURCE_LAYOUT::GPU_RESOURCE_TYPE::GPU_RES_TYPE_NUMBER];
};

/// <summary>
/// gpu��Դ��Ԫ�����ͣ���ҪRENDER API �̳й����ڴ�ָ���
/// </summary>
class gpu_resource_element
{
	
public:
	std::string name;
	//??? ֻ��DX ��RTV��Ч
	bool need_clear = false;

	//ֻ���ϴ�����Ч
	BYTE* mapped_data = nullptr;

	UINT element_byte_size = 0; //  Ԫ�� �������� ����Ҫ
	UINT memory_size = 0; // �ֽ�Ϊ��λ�Ĵ�С
	UINT element_number = 0; // Ԫ��������

	//�����ٸ�Ԫ��Ϊһ��
	std::vector<UINT> element_group_number; //���ڴ洢index�����ݵ��ڲ�ƫ��( [0] ��0���ж��ٸ�Ԫ��)

	GPU_RESOURCE_LAYOUT* cpu_resource_layout;
};



