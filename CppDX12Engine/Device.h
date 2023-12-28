#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include <dxgi1_4.h> 

/*
Implement the Device class responsible for initializing and managing the DirectX12 device. 
This involves creating the device, command queues, and other necessary components.
*/

class Device
{
public :
	Device();
	~Device();

	void Initialize();
	void PrintDeviceInfo();
	ID3D12Device* GetDevice() const;

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGIFactory> dxgiFactory;
	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
};

