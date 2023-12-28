#pragma once

#define NOMINMAX

#include "Device.h"
#include "iostream"
#include <d3d12.h>
#include <wrl/client.h>
#include <Windows.h>
#include <dxgi1_6.h>

#ifdef _DEBUG
#include <d3d12sdklayers.h>
#include <dxgidebug.h>
#endif