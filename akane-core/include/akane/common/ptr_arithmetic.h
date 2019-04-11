#pragma once
#include <cstdint>

namespace akane
{
	inline void* AdvancePtr(void* ptr, int bytes)
	{
		return reinterpret_cast<uint8_t*>(ptr) + bytes;
	}
	inline const void* AdvancePtr(const void* ptr, int bytes)
	{
		return reinterpret_cast<const uint8_t*>(ptr) + bytes;
	}
	inline void* AdvancePtr(void* ptr, size_t bytes)
	{
		return reinterpret_cast<uint8_t*>(ptr) + bytes;
	}
	inline const void* AdvancePtr(const void* ptr, size_t bytes)
	{
		return reinterpret_cast<const uint8_t*>(ptr) + bytes;
	}

	inline ptrdiff_t PtrDistance(const void* p1, const void* p2)
	{
		return reinterpret_cast<const uint8_t*>(p2) - reinterpret_cast<const uint8_t*>(p1);
	}
}