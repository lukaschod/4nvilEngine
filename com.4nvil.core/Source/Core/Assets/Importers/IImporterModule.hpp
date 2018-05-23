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
#include <Core/Tools/String.hpp>
#include <Core/Tools/IO/Directory.hpp>
#include <Core/Foundation/PipeModule.hpp>
#include <Core/Foundation/TransferModule.hpp>

namespace Core
{
    struct Package;
    struct Transferable;
}

namespace Core
{
    class IImporterModule : public PipeModule
    {
    public:
        BASE_IS(PipeModule);

    public:
        // Returns extension name that is supported by this importer
        // Extension must be without dot (etc png) and lower case
        virtual bool IsSupported(const DirectoryExtension& extension) = 0;

    public:
        virtual const Transferable* Import(const ExecutionContext& context, const Directory& directory) = 0;
        virtual const Transferable* Import(const ExecutionContext& context, const Guid& guid) = 0;
    };
}