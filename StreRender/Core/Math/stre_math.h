#pragma once
#include "base_type.h"
#include <string>
#include <ctime>

template<typename t_type>
void generate_unique_identifier(s_uid & in_out_uid)
{
	std::string name_str(typeid(t_type).name());

	name_str += std::to_string(time(NULL));
	name_str += std::to_string((unsigned int)(&in_out_uid));

	memcpy(in_out_uid.name, name_str.c_str(), 256 * sizeof(char));
}