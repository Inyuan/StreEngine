#pragma once
#include "stre_engine.h"

//每个功能函数都封装一层命令

class s_command
{
public:
	virtual void execute() = 0;
};


