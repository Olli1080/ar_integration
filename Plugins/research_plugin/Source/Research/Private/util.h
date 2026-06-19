#pragma once

#include "pch.h"

// Note: WinRT and DirectX conversion utilities have been removed.
// The research_plugin now uses native Unreal and Meta XR types.

template<typename out, typename in>
out convert(const in&);
