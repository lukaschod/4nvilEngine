#pragma once

#include <alloca.h>
#include <Tools\Common.h>

namespace Core
{
	struct IView;

	struct Input
	{
		uint8* GetData() { return (uint8*)this + sizeof(const IView*) + sizeof(size_t); }
		const IView* view;
		size_t size;
	};

	template<size_t Size>
	Input* AllocateInputOnStack()
	{
		void *alloca(size_t size);
	}
}