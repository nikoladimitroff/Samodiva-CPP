#pragma once

namespace Samodiva
{
	class IAllocator
	{
	public:
		virtual ~IAllocator() {}
		virtual void* Malloc(size_t size) = 0;
		virtual void Free(void* ptr) = 0;
		virtual void* Realloc(void* ptr, size_t newSize) = 0;
	};
}