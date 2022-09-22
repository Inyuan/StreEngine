#include "deferred_rendering.h"


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