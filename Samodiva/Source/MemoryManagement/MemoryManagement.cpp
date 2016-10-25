#include "MemoryManagement.h"
#include <cstddef>

namespace Samodiva
{
Samodiva::IAllocator* g_Allocator = nullptr;
thread_local Samodiva::LinearAllocator<tls_TempAllocatorSize> Samodiva::TempAllocator::tls_Alloc;
}

void* operator new(std::size_t size)
{
	return Samodiva::g_Allocator->Malloc(size, 0);
}
void operator delete(void* ptr)
{
	Samodiva::g_Allocator->Free(ptr);
}
void* operator new[](std::size_t size)
{
	return Samodiva::g_Allocator->Malloc(size, 0);
}
void operator delete[](void* ptr)
{
	Samodiva::g_Allocator->Free(ptr);
}