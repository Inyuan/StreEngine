#pragma once
#include <map>
#include <string>


class s_memory
{
public:
	template<typename T>
	T* allocate();
	//获取一片全0的空间
	void* custom_allocate(std::size_t in_size);
};


std::map<std::string, s_memory* > memory_allocater_group;


class s_memory_allocater_register
{
	s_memory_allocater_register() = delete;
public:
	s_memory_allocater_register(std::string in_allocater)
	{
		memory_allocater_group[in_allocater] = new s_memory();
	}
};


