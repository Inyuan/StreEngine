#pragma once
#include "constant_pass.h"
#include "Core/Render_Core/render.h"
#include<map>


class constant_pass_factory
{
private:
	std::map<std::string, constant_pass*> pass_group;

public: 
	void create_pass();

	
};