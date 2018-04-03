/*
* Copyright (c) Lukas Chodosevicius
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <Core/Assets/Importers/PngImporterModule.hpp>

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
