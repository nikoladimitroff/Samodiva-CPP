#include "MemoryManagement.h"
#include <cstddef>

namespace Samodiva
{
Samodiva::IAllocator* g_Allocator = nullptr;
}

void* operator new(std::size_t size)
{
	return Samodiva::g_Allocator->Malloc(size);
}
void operator delete(void* ptr)
{
	Samodiva::g_Allocator->Free(ptr);
}
void* operator new[](std::size_t size)
{
	return Samodiva::g_Allocator->Malloc(size);
}
void operator delete[](void* ptr)
{
	Samodiva::g_Allocator->Free(ptr);
}