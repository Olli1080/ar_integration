#pragma once

#if defined(PLATFORM_HOLOLENS) || defined(PLATFORM_WINDOWS)
#define PLATFORMS 1
#endif

#if (defined(__INTELLISENSE__) || defined(__RESHARPER__))
#define __PARSER__ 1
#endif

#if (defined(PLATFORMS)) 
//Before writing any code, you need to disable common warnings in WinRT headers
#pragma warning(disable : 5205 4265 4268 4946 4191)

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/MinWindows.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/PreWindowsApi.h"

#include <future>
#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include <array>
#include <queue>

#include <unknwn.h>
#include <d2d1_2.h>
#include <d3d11_4.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <dwrite_2.h>

#include <wincodec.h>
#include <WindowsNumerics.h>
#include <directxmath.h>

#include <Windows.Graphics.Directx.Direct3D11.Interop.h>
#include <wrl/client.h>

#include <winrt/base.h>
#include <winrt/Windows.Perception.Spatial.Preview.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.Gaming.Input.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Graphics.Holographic.h>
#include <winrt/Windows.Perception.People.h>
#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.Spatial.h>

#include "researchmode/ResearchModeApi.h"

#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"

#elif

#include <future>
#include <map>
#include <mutex>
#include <vector>
#include <memory>
#include <array>
#include <queue>

#endif