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

#include <Core/Tools/Collections/List.hpp>

namespace Core
{
    template<class ReturnType> struct Delegate {};

    template<class ReturnType, class... ArgumentTypes>
    class Delegate<ReturnType(ArgumentTypes...)>
    {
    private:
        struct Callable
        {
            virtual ReturnType Call(ArgumentTypes... arguments) pure;
            virtual Bool IsSame(Callable* other) pure;
        };

        struct Function : public Callable
        {
            Function(ReturnType(*function)(ArgumentTypes...))
                : function(function)
            {
            }

            virtual ReturnType Call(ArgumentTypes... arguments) override
            {
                function(arguments...);
            }

            virtual Bool IsSame(Callable* other) override
            {
                auto _other = dynamic_cast<Function*>(other);
                if (_other == nullptr)
                    return false;

                return function == _other->function;
            }

            ReturnType(*function)(ArgumentTypes...);
        };

        template<class ClassType>
        struct Method : public Callable
        {
            Method(ReturnType(ClassType::*function)(ArgumentTypes...), ClassType* instance)
                : function(function)
                , instance(instance)
            {
            }

            virtual ReturnType Call(ArgumentTypes... arguments) override
            {
                (instance->*function)(arguments...);
            }

            virtual Bool IsSame(Callable* other) override
            {
                auto _other = dynamic_cast<Method*>(other);
                if (_other == nullptr)
                    return false;

                return function == _other->function && instance == _other->instance;
            }

            ClassType* instance;
            ReturnType(ClassType::*function)(ArgumentTypes...);
        };
    public:
        ~Delegate()
        {
            for (auto callable : callables)
                delete callable;
        }

        ReturnType operator()(ArgumentTypes... arguments)
        {
            for (auto callable : callables)
                callable->Call(arguments...);
        }

        Void AddFunction(ReturnType(*function)(ArgumentTypes...))
        {
            auto callable = new Function(function);
            callables.push_back(callable);
        }

        template<class ClassType>
        Void AddMethod(ReturnType(ClassType::*method)(ArgumentTypes...), ClassType* instance)
        {
            auto callable = new Method<ClassType>(method, instance);
            callables.push_back(callable);
        }

        Void RemoveFunction(ReturnType(*function)(ArgumentTypes...))
        {
            Function temp(function);
            for (auto itr = callables.begin(); itr != callables.end(); itr++)
            {
                if ((*itr)->IsSame((Callable*) &temp))
                {
                    delete *itr;
                    callables.erase(itr);
                    return;
                }
            }
        }

        template<class ClassType>
        Void RemoveMethod(ReturnType(ClassType::*method)(ArgumentTypes...), ClassType* instance)
        {
            Method<ClassType> temp(method, instance);
            for (auto itr = callables.begin(); itr != callables.end(); itr++)
            {
                if ((*itr)->IsSame((Callable*) &temp))
                {
                    delete *itr;
                    callables.erase(itr);
                    return;
                }
            }
        }

    private:
        std::vector<Callable*> callables;
    };
}