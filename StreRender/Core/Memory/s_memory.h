#pragma once
#include <map>
#include <string>


class s_memory
{

private:
	void* memory_ptr;
public:

	s_memory() // 10000
	{
	}

	template<typename T>
	T* allocate();
	//获取一片全0的空间
	void* custom_allocate(std::size_t in_size);

	template<typename T,typename in_T>
	T* allocate(in_T * in_t);

	template<typename T, typename in_T>
	T* allocate(in_T in_t);
};

template<typename T>
T* s_memory::allocate()
{
	return new T();
}

template<typename T, typename in_T>
T* s_memory::allocate(in_T* in_t)
{
	return new T(in_t);
}

template<typename T, typename in_T>
T* s_memory::allocate(in_T in_t)
{
	return new T(in_t);
}

//获取一片全0的空间
void* s_memory::custom_allocate(std::size_t in_size)
{
	void* ptr = malloc(in_size);
	if (ptr)
	{
		memset(ptr, 0, in_size);
	}
	return ptr;
}


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


