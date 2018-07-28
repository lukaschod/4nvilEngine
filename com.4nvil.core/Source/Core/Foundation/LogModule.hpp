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

#pragma once

#include <Core/Tools/Common.hpp>
#include <Core/Tools/IO/FileStream.hpp>
#include <Core/Foundation/PipeModule.hpp>

namespace Core
{
    class LogModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

        CORE_API virtual Void Execute(const ExecutionContext& context) override;
        CORE_API Void RecWrite(const ExecutionContext& context, const Char* name);
        CORE_API Void RecWriteFmt(const ExecutionContext& context, const Char* format, ...);

    private:
        Void OpenStream();
        Void CloseStream();

    protected:
        CORE_API virtual Bool ExecuteCommand(const ExecutionContext& context, CommandStream& stream, CommandCode commandCode) override;

    private:
        IO::FileStream output;
    };
}