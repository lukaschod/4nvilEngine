#pragma once

#include <functional>
#include <Tools\Common.h>
#include <Tools\Collections\List.h>

namespace Core
{
	class Module;

	struct ModuleJob
	{
		ModuleJob()
			: module(nullptr)
			, offset(0)
			, size(0)
			, userData(nullptr)
		{
		}
		Module* module;
		uint32_t offset;
		size_t size;
		void* userData; // TODO: Maybe we can get rid of it, currently one implementation really needs it for perforamnce
	};

	class IModulePlanner
	{
	public:
		// Recreates the Modules execution plan
		virtual void Recreate(List<Module*>& modules) = 0;

		// Prepares for new frame
		virtual void Reset() = 0;

		// Pulls next executable job, if no job is available nullptr will be returned in Module field
		// Thread-safe
		virtual ModuleJob TryGetNext() = 0;

		// Marks the job as finished
		// Thread-safe
		virtual void SetFinished(const ModuleJob& job) = 0;

	public:
		AUTOMATED_PROPERTY_SET(std::function<void(size_t)>, jobFinishCallback);
		AUTOMATED_PROPERTY_SET(std::function<void(void)>, finishCallback);
	};
}