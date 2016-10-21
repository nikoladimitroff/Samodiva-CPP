#pragma once
#include <memory>
#include <vector>
#include <string>

#include <Samodiva/Allocator.h>
#include "StlAllocator.h"
#include "LinearAllocator.h"

namespace Samodiva
{
	extern Samodiva::IAllocator* g_Allocator;

	inline void* SamodivaMalloc(size_t size)
	{
		return g_Allocator->Malloc(size, 0);
	}

	inline void* SamodivaRealloc(void* ptr, size_t size)
	{
		return g_Allocator->Realloc(ptr, size);
	}

	inline void SamodivaFree(void* ptr)
	{
		g_Allocator->Free(ptr);
	}

	namespace
	{
		constexpr size_t tlsAllocatorSize = 4 * 1024 * 1024; // 4MB
		using TempAllocator = ThreadLocalLinearAllocator<tlsAllocatorSize>;
	}
	using ArraySizeType = unsigned;
	template<typename T>
	inline T* SamodivaMallocArray(unsigned count)
	{
		const auto totalSize = sizeof(T) * count + sizeof(ArraySizeType);
		auto ptr = SamodivaMalloc(totalSize);
		*static_cast<ArraySizeType*>(ptr) = static_cast<ArraySizeType>(count);

		auto result = reinterpret_cast<T*>(static_cast<char*>(ptr) + sizeof(ArraySizeType));
		if (!std::is_fundamental<T>::value)
		{
			for (auto i = 0u; i < count; ++i)
			{
				new(result + i) T();
			}
		}
		return result;
	}


	template<typename T>
	inline void SamodivaDestroyArray(const T* ptr)
	{
		if (!ptr)
			return nullptr;

		const ArraySizeType* counterPtr = reinterpret_cast<ArraySizeType*>(ptr);
		const auto count = *counterPtr;

		for (int i = int(count - 1); i >= 0; --i) {
			(ptr + i)->~T();
		}
	}

	template<typename T>
	struct TmpDeleter
	{
		void operator()(T* ptr)
		{
			ptr->~T();
			TempAllocator::GetTlsAllocator().Free(ptr);
		}
	};
	template<typename T>
	struct TmpDeleterArray
	{
		void operator()(T* ptr)
		{
			SamodivaDestroyArray(ptr);
		}
	};

	template<typename T>
	using TmpVector = std::vector<T, StlAllocatorTemplate<TempAllocator, T>>;
	using TmpString = std::basic_string<char, std::char_traits<char>, StlAllocatorTemplate<TempAllocator, char>>;
	template<typename T>
	using TmpUniquePtr = std::unique_ptr<T, TmpDeleter<T>>;
	template<class T>
	using TmpUniqueArray = std::unique_ptr<T[], TmpDeleterArray<T>>;
	template<typename T>
	using TmpSharedPtr = std::shared_ptr<T>;
	template<typename T, typename... Args>
	inline TmpUniquePtr<T> MakeUniqueTmp(Args&&... args)
	{
		auto ptr = new(TempAllocator::GetTlsAllocator().Malloc(sizeof(T)) T(std::forward<Args>(args)...));
		return TmpUniquePtr<T>(ptr);
	}
	template<typename T, typename... Args>
	inline TmpUniqueArray<T> MakeUniqueArrayTmp(Args&&... args)
	{
		auto ptr = new(TempAllocator::GetTlsAllocator().Malloc(sizeof(T)) T(std::forward<Args>(args)...));
		return TmpUniqueArray<T>(ptr);
	}
	template<typename T, typename... Args>
	inline TmpSharedPtr<T> MakeSharedTmp(Args&&... args)
	{
		auto ptr = new(TempAllocator::GetTlsAllocator().Malloc(sizeof(T)) T(std::forward<Args>(args)...));
		return TmpSharedPtr<T>(ptr);
	}
}
