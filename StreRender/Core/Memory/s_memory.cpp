#include "s_memory.h"

template<typename T>
T* s_memory::allocate()
{
	
	return new T();
}