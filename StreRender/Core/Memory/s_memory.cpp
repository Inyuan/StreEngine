#include "s_memory.h"

template<typename T>
T* s_memory::allocate()
{
	
	return new T();
}

//��ȡһƬȫ0�Ŀռ�
void* s_memory::custom_allocate(std::size_t in_size)
{
	void* ptr = malloc(in_size);
	if (ptr)
	{
		memset(ptr, 0, in_size);
	}
	return ptr;
}