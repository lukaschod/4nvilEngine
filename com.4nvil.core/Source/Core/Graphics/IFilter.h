#pragma once

#include <Core\Tools\Common.h>

namespace Core::Graphics
{
	struct FilterOptions {};

	struct IFilter
	{
		IFilter(const FilterOptions& options) {}
	};
}