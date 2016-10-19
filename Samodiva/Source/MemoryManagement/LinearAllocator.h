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
		Free(ptr);
		return copy;
	}

	inline void Reset(void* ptr)
	{
		m_Marker = ptr;
	}

	// Used to reset 
	struct Scope
	{
		Scope(LinearAllocator* allocator)
			: m_Allocator(allocator)
			, m_Marker(allocator->GetMarker())
		{}
		Scope(const Scope&) = delete;
		Scope& operator=(const Scope&) = delete;
		~Scope()
		{
			allocator->Reset(m_Marker);
		}
	private:
		LinearAllocator* m_Allocator;
		void* m_Marker;
	};
	inline Scope ScopeNow()
	{
		return Scope(this);
	}
private:
	char m_Buffer[Bytes];
	char* m_Marker;

	inline void* GetMarker() const
	{
		return m_Marker;
	}
};
