#pragma once
#include "stre_render.h"
#include "render_system.h"
#include <map>
#include <string>


template<typename T_render>
class deferred_rendering : public s_rendering<T_render>
{
private:
	render_system<T_render>* deferred_render_system;

	std::map<std::string, constant_pass* > pass_group;

	void pass_init();

public:

	virtual void init(HINSTANCE in_instance) override
	{
		deferred_render_system = new render_system<T_render>();
		deferred_render_system->init(in_instance);
		pass_init();
	}

	virtual void draw(s_sence* in_sence) override;

	virtual void over() override;
};
