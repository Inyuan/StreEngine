#include "deferred_rendering.h"

//利用render_system给物体分配GPU内存
//利用render_system构建多个pass
//利用render_system构建rt
//利用render_system设定输出目标

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