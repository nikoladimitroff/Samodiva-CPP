#pragma once

template<size_t Bytes>
class LinearAllocator
{
public:
	LinearAllocator()
		: m_Marker(m_Buffer)
	{}
	inline void* Malloc(size_t size)
	{
		auto ptr = m_Marker;
		m_Marker += size;
		return ptr;
	}
	inline void Free(void* ptr)
	{
	}
	inline void* Realloc(void* ptr, size_t newSize)
	{
		auto copy = Malloc(newSize);
		std::memcpy(copy, ptr, newSize);
		return copy;
	}

	inline void Reset(void* ptr)
	{
		m_Marker = ptr;
	}

private:
	const char m_Buffer[Bytes];
	const char* m_Marker;
};