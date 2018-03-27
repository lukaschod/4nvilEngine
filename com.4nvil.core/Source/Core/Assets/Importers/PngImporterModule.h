#pragma once

#include <Core\Tools\Common.h>
#include <Core\Assets\Importers\IImporterModule.h>

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