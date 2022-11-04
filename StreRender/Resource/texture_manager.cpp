#ifdef  DLL_GRAPHICS_API
#else
#define DLL_GRAPHICS_API _declspec(dllexport)
#endif

#include "resource_factory.h"
#include "Core/Memory/s_memory.h"
////////////////////////////////////////////////////////////////////
//create object