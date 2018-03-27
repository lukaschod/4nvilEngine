#include <Core\Assets\Importers\PngImporterModule.hpp>

using namespace Core;

const char* PngImporterModule::GetSupportedExtension() { return "png"; }

SERIALIZE_METHOD_ARG1(PngImporterModule, Import, String&);

bool PngImporterModule::ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode)
{
	switch (commandCode)
	{
		DESERIALIZE_METHOD_ARG1_START(Import, String, pathToString);
		TRACE("%s", pathToString.c_str());
		DESERIALIZE_METHOD_END;
	}
	return false;
}
