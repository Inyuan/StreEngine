#include "s_memory.h"


template<class T>
T* s_memory::allocate()
{
	return new T();
}

template<class T, class in_T>
T* s_memory::allocate(in_T* in_t)
{
	return new T(in_t);
}

template<class T, class in_T>
T* s_memory::allocate(in_T in_t)
{
	return new T(in_t);
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

