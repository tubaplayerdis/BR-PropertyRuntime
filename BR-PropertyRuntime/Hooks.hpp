#pragma once
#include <vadefs.h>

namespace Hooks
{
	bool IsEditorBinary();
	uintptr_t GetSymbolAddress(const char* Module, const char* Symbol);
	void SetupCreateObjectHook();
}