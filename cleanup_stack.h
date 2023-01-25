#pragma once
#include "defines.h"

class CleanupStack
{
private:
	std::stack<std::function<void()>> stack;

public:
	void push(std::function<void()>&& fun);
	void flush();
};