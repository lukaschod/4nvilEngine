#pragma once

#include <Core\Tools\Common.hpp>
#include <Core\Assets\Importers\IImporterModule.hpp>

namespace Core
{
	class PngImporterModule : public IImporterModule
	{
	public:
		virtual const char* GetSupportedExtension() override;

	public:
		virtual void RecImport(const ExecutionContext& context, String& pathToFile) override;

	protected:
		virtual bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;
	};
}