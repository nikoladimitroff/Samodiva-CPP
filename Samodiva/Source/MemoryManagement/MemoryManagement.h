#pragma once
#include <deque>
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
		constexpr size_t tls_TempAllocatorSize = 4 * 1024 * 1024; // 4MB
		using TempAllocator = ThreadLocalLinearAllocator<tls_TempAllocatorSize>;
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
	
	namespace tmp
	{

		template<typename Base>
		struct TempDeleter
		{
			TempDeleter() {}
			template<typename Derived>
			TempDeleter(const TempDeleter<Derived>&)
			{
				static_assert(std::is_base_of<Base, Derived>::value, "Only inherited casting is allowed");
				static_assert(std::has_virtual_destructor<Base>::value, "Type needs virtual destructor!");
			}
			void operator()(Base* ptr)
			{
				ptr->~Base();
				TempAllocator::GetTlsAllocator().Free(ptr);
			}
		};
		template<typename T>
		struct TempDeleterArray
		{
			TempDeleterArray() {}
			template<typename Derived>
			TempDeleterArray(const TempDeleterArray<Derived>&)
			{
				static_assert(std::is_base_of<Base, Derived>::value, "Only inherited casting is allowed");
				static_assert(std::has_virtual_destructor<Base>::value, "Type needs virtual destructor!");
			}
			void operator()(T* ptr)
			{
				SamodivaDestroyArray(ptr);
			}
		};

		template<typename T>
		using vector = std::vector<T, StlAllocatorTemplate<TempAllocator, T>>;
		using string = std::basic_string<char, std::char_traits<char>, StlAllocatorTemplate<TempAllocator, char>>;
		template<typename T>
		using unique_ptr = std::unique_ptr<T, TempDeleter<T>>;
		template<typename T>
		using unique_array = std::unique_ptr<T[], TempDeleterArray<T>>;
		template<typename T>
		using shared_ptr = std::shared_ptr<T>;
		template<typename T, typename... Args>
		inline unique_ptr<T> make_unique(Args&&... args)
		{
			auto ptr = new(TempAllocator::GetTlsAllocator().Malloc(sizeof(T)) T(std::forward<Args>(args)...));
			return unique_ptr<T>(ptr);
		}
		template<typename T, typename... Args>
		inline unique_array<T> make_unique_array(Args&&... args)
		{
			auto ptr = new(TempAllocator::GetTlsAllocator().Malloc(sizeof(T)) T(std::forward<Args>(args)...));
			return unique_array<T>(ptr);
		}
		// Convert to intrusive ptr
		template<typename T, typename... Args>
		inline shared_ptr<T> make_shared(Args&&... args)
		{
			auto ptr = new(TempAllocator::GetTlsAllocator().Malloc(sizeof(T)) T(std::forward<Args>(args)...));
			return shared_ptr<T>(ptr);
		}
	}

	class DefaultAllocator
	{
	public:
		void Initialize()
		{}
		inline void* Malloc(size_t size, unsigned alignment)
		{
			auto ptr = g_Allocator->Malloc(size, alignment);
			return ptr;
		}
		inline void Free(void* ptr)
		{
			g_Allocator->Free(ptr);
		}
		inline void* Realloc(void* ptr, size_t newSize)
		{
			return g_Allocator->Realloc(ptr, newSize);
		}
	};

	namespace stl
	{
		template<typename Base>
		struct StdDeleter
		{
			StdDeleter() {}
			template<typename Derived>
			StdDeleter(const StdDeleter<Derived>&)
			{
				static_assert(std::is_base_of<Base, Derived>::value, "Only inherited casting is allowed");
				static_assert(std::has_virtual_destructor<Base>::value, "Type needs virtual destructor!");
			}
			void operator()(Base* ptr)
			{
				ptr->~Base();
				SamodivaFree(ptr);
			}
		};
		template<typename Base>
		struct StdDeleterArray
		{
			StdDeleterArray() {}
			template<typename Derived>
			StdDeleterArray(const StdDeleterArray<Derived>&)
			{
				static_assert(std::is_base_of<Base, Derived>::value, "Only inherited casting is allowed");
				static_assert(std::has_virtual_destructor<Base>::value, "Type needs virtual destructor!");
			}
			void operator()(Base* ptr)
			{
				SamodivaDestroyArray(ptr);
			}
		};

		template<typename T>
		using vector = std::vector<T, StlAllocatorTemplate<DefaultAllocator, T>>;
		template<typename T>
		using deque = std::deque<T, StlAllocatorTemplate<DefaultAllocator, T>>;
		using string = std::basic_string<char, std::char_traits<char>, StlAllocatorTemplate<DefaultAllocator, char>>;
		template<typename T>
		using unique_ptr = std::unique_ptr<T, StdDeleter<T>>;
		template<class T>
		using unique_array = std::unique_ptr<T[], StdDeleterArray<T>>;
		template<typename T>
		using shared_ptr = std::shared_ptr<T>;
		template<typename T, typename... Args>
		inline unique_ptr<T> make_unique(Args&&... args)
		{
			auto ptr = new (g_Allocator->Malloc(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
			return unique_ptr<T>(ptr);
		}
		template<typename T, typename... Args>
		inline unique_array<T> make_unique_array(Args&&... args)
		{
			auto ptr = new (g_Allocator->Malloc(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
			return unique_array<T>(ptr);
		}
		template<typename T, typename... Args>
		inline shared_ptr<T> make_shared(Args&&... args)
		{
			auto ptr = new (g_Allocator->Malloc(sizeof(T), alignof(T))) T(std::forward<Args>(args)...);
			return shared_ptr<T>(ptr);
		}
	}
}
