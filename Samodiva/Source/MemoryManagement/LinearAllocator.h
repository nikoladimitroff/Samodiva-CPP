#pragma once

template<size_t Bytes>
class LinearAllocator
{
public:
	LinearAllocator()
		: m_Marker(&m_Buffer[0])
	{}
	void Initialize()
	{}
	inline void* Malloc(size_t size)
	{
		auto ptr = m_Marker;
		m_Marker += size;
		return ptr;
	}
	inline void Free(void*)
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
	char m_Buffer[Bytes];
	char* m_Marker;
};