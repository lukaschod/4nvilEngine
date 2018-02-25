#pragma once

#include <Tools\Testing.h>

typedef signed char        int8_t;
typedef short              int16_t;
typedef int                int32_t;
typedef long long          int64_t;
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

namespace Core
{
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

#define IMPLEMENT_ENUM_FLAG(Name) \
inline Name operator|(Name a, Name b) { return static_cast<Name>(static_cast<int>(a) | static_cast<int>(b)); } \
inline Name operator&(Name a, Name b) { return static_cast<Name>(static_cast<int>(a) & static_cast<int>(b)); }
}