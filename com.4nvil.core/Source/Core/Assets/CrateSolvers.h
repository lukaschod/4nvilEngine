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
#include <Core/Tools/Collections/List.hpp>
#include <Core/Tools/Collections/Set.hpp>
#include <Core/Assets/CrateModule.hpp>

namespace Core
{
    // Finds all resources that are needed for loading
    class CrateResourceSolver
    {
    public:
        Void Solve(Crate* crate, UInt globalIndex)
        {
            SolveRecusriveForGlobal(crate, globalIndex);
        }

        Void Clear()
        {
            resources.clear();
        }

    private:
        Void SolveRecusriveForGlobal(const Crate* crate, UInt globalIndex)
        {
            auto localIndex = crate->globals[globalIndex].localIndex;
            SolveRecusriveForLocal(crate, localIndex);
        }

        Void SolveRecusriveForLocal(const Crate* crate, UInt localIndex)
        {
            auto& resource = crate->locals[localIndex];

            Resource target;
            target.crate = crate;
            target.localIndex = localIndex;
            if (!resources.insert(target).second)
                return;

            for (auto& dependacy : resource.dependencies)
            {
                switch (dependacy.type)
                {
                case ResourceType::Local:
                {
                    auto& resourceLocal = crate->locals[dependacy.index];
                    if (resourceLocal.cachedTransferable != nullptr)
                        continue;
                    SolveRecusriveForLocal(crate, dependacy.index);
                    break;
                }

                case ResourceType::Extern:
                {
                    auto& resourceExtern = crate->externs[dependacy.index];
                    SolveRecusriveForGlobal(resourceExtern.cachedCrate, resourceExtern.globalIndex);
                    break;
                }
                }

            }
        }

    public:
        struct Resource
        {
            inline Bool operator<(const Resource& rhs) const
            { 
                return 
                    (crate < rhs.crate) || // Order by crate
                    ((crate == rhs.crate) && (localIndex < rhs.localIndex)); // If crates are same, eventually order by index
            } // We do not really care about order, we only want grouping
            const Crate* crate;
            UInt localIndex;
        };
        Set<Resource> resources;
    };

    // From all collected dependancy pointers, updates from index values to pointer values
    class CrateDependancySolver
    {
    public:
        Void Add(const Crate* crate, const Transferable*& transferable, const ResourceDependancy& dependancy)
        {
            dependencies.push_back(Dependancy(crate, transferable, dependancy));
        }

        Void Solve()
        {
            for (auto& dependancy : dependencies)
                dependancy.transferable = FindTransferable(dependancy.crate, dependancy.dependancy);
        }

        Void Clear()
        {
            dependencies.clear();
        }

    private:
        const Transferable* FindTransferable(const Crate* crate, const ResourceDependancy& dependancy)
        {
            switch (dependancy.type)
            {
            case ResourceType::Local:
            {
                auto& resource = crate->locals[dependancy.index];
                ASSERT(resource.cachedTransferable != nullptr);
                return resource.cachedTransferable;
            }
            case ResourceType::Extern:
            {
                auto& resource = crate->externs[dependancy.index];

                auto& resourceGlobal = resource.cachedCrate->globals[resource.globalIndex];

                auto& localResource = resource.cachedCrate->locals[resourceGlobal.localIndex];
                ASSERT(localResource.cachedTransferable != nullptr);
                return localResource.cachedTransferable;
            }
            default:
                ERROR("Unknown resource type with index %i", dependancy.type);
                break;
            }
            return nullptr;
        }

    private:
        struct Dependancy
        {
            Dependancy(const Crate* crate, const Transferable*& transferable, const ResourceDependancy& dependancy)
                : crate(crate)
                , transferable(transferable)
                , dependancy(dependancy)
            { }
            const Crate* crate;
            const Transferable*& transferable;
            ResourceDependancy dependancy;
        };
        List<Dependancy> dependencies;
    };
}