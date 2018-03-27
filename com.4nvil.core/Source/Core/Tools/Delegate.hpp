#pragma once

#include <Core\Tools\Collections\List.hpp>

namespace Core
{
	template<class ReturnType> struct Delegate {};

	template<class ReturnType, class... ArgumentTypes>
	class Delegate<ReturnType(ArgumentTypes...)>
	{
	private:
		struct Callable
		{
			virtual ReturnType Call(ArgumentTypes... arguments) = 0;
			virtual bool IsSame(Callable* other) = 0;
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

			virtual bool IsSame(Callable* other) override
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

			virtual bool IsSame(Callable* other) override
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

		void AddFunction(ReturnType(*function)(ArgumentTypes...))
		{
			auto callable = new Function(function);
			callables.push_back(callable);
		}

		template<class ClassType>
		void AddMethod(ReturnType(ClassType::*method)(ArgumentTypes...), ClassType* instance)
		{
			auto callable = new Method<ClassType>(method, instance);
			callables.push_back(callable);
		}

		void RemoveFunction(ReturnType(*function)(ArgumentTypes...))
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
		void RemoveMethod(ReturnType(ClassType::*method)(ArgumentTypes...), ClassType* instance)
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