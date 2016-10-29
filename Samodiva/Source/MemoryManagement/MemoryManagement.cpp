#include "MemoryManagement.h"
#include <cstddef>

namespace Samodiva
{
#ifdef SAMODIVA_TEST_BUILD
class MallocAllocator : public Samodiva::IAllocator
{
public:
	virtual void* Malloc(size_t size, unsigned)
	{
		return std::malloc(size);
	}
	virtual void Free(void* ptr)
	{
		std::free(ptr);
	}
	virtual void* Realloc(void* ptr, size_t newSize)
	{
		return std::realloc(ptr, newSize);
	}
};
MallocAllocator g_TestingMallocAllocator;
Samodiva::IAllocator* g_Allocator = &g_TestingMallocAllocator;
#else
Samodiva::IAllocator* g_Allocator = nullptr;
#endif
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