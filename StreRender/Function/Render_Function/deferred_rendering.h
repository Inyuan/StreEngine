#pragma once
#include "stre_render.h"
#include "render_system.h"
#include <map>
#include <string>



class deferred_rendering : public s_rendering
{
private:
	render_system* deferred_render_system;

	std::map<std::string, constant_pass* > pass_group;


public:

	virtual void init(HINSTANCE in_instance,REDNER_API in_render_api) override;

	virtual void draw(s_sence* in_sence) override;

	virtual void over() override;
};