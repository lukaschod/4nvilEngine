#pragma once

class IHeap
{
public:
	virtual void* Allocate() = 0;
	virtual void Deallocate(void* pointer) = 0;
};