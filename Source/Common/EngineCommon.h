#pragma once

#include <cstdint>
#include <assert.h>

#define DebugAssert(Test) assert(Test)
#define LogError(Message) assert(false)

#define FOR_EACH(Vector, ItemName) \
	for (auto ItemName = Vector.begin(); ItemName != Vector.end(); ItemName++)

#define FOR_INC(Count, ItemName) \
	for (auto ItemName = 0; ItemName < Count; ItemName++)

#define AUTOMATED_PROPERTY_GETPTR(Type, Name) \
	protected: Type Name; \
	public: inline Type* Get_##Name() { return &Name; } \
	protected:

#define AUTOMATED_PROPERTY_GETADR(Type, Name) \
	protected: Type Name; \
	public: inline Type& Get_##Name() { return Name; } \
	protected:

#define AUTOMATED_PROPERTY_GET(Type, Name) \
	protected: Type Name; \
	public: GETTER(Type, Name) \
	protected:

#define AUTOMATED_PROPERTY_SET(Type, Name) \
	protected: Type Name; \
	public: SETTER(Type, Name) \
	protected:

#define AUTOMATED_PROPERTY_GETSET(Type, Name) \
	protected: Type Name; \
	public: GETTER(Type, Name) SETTER(Type, Name) \
	protected:

#define SETTER(Type, Name) inline void Set_##Name(Type value) { Name = value; }
#define GETTER(Type, Name) inline Type Get_##Name() const { return Name; }

#define SAFE_DELETE(Pointer) if (Pointer != nullptr) delete Pointer;

#define SAFE_VECTOR_DELETE(Vector) \
	while (Vector.size() != 0) \
	{ \
		auto item = Vector.back(); \
		SAFE_DELETE(item); \
		Vector.pop_back(); \
	}
		