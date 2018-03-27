#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Tools\String.hpp>
#include <Core\Foundation\PipeModule.hpp>

namespace Core
{
	class IImporterModule : public PipeModule
	{
	public:
		BASE_IS(PipeModule);

	public:
		// Returns extension name that is supported by this importer
		// Extension must be without dot (etc png) and lower case
		virtual const char* GetSupportedExtension() = 0;

	public:
		// Records the to import data
		virtual void RecImport(const ExecutionContext& context, String& pathToFile) = 0;
	};
}