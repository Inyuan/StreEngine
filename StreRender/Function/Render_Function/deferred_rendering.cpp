#include "deferred_rendering.h"

//����render_system���������GPU�ڴ�
//����render_system�������pass
//����render_system����rt
//����render_system�趨���Ŀ��

void deferred_rendering::init(HINSTANCE in_instance, REDNER_API in_render_api)
{
	deferred_render_system = new render_system();
	deferred_render_system->init(in_instance, in_render_api);


}

void deferred_rendering::over()
{

}

void deferred_rendering::draw(s_sence* in_sence)
{

}