#pragma once

#include <Windows.h>

namespace Patches {
    void ApplyDebugPatches(HMODULE ExeModule, HMODULE dllModule);
}
