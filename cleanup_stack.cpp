#include "cleanup_stack.h"

void CleanupStack::push(std::function<void()>&& fun)
{
	stack.push(fun);
}

void CleanupStack::flush()
{
	while (!stack.empty())
	{
		std::function<void()> fun = stack.top();
		fun();
		stack.pop();
	}
}