#include "Device.h"
#include <iostream>


Device::Device() {}

Device::~Device() {}

void Device::Initialize()
{
	HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf()));

	if (FAILED(hr))
	{
		std::cout << "Failed to initialize" << std::endl;
		return;
	}

    HRESULT hrF = CreateDXGIFactory1(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
    if (FAILED(hrF)) {
        std::cout << "Failed to create DXGI factory" << std::endl;
        return;
    }


    HRESULT hrFactory = dxgiFactory->EnumAdapters(0, dxgiAdapter.GetAddressOf());
    if (FAILED(hrFactory)) {
        std::cout << "Failed to enumerate adapters" << std::endl;
        return;
    }

}


void Device::PrintDeviceInfo() {
    DXGI_ADAPTER_DESC adapterDesc;
    dxgiAdapter->GetDesc(&adapterDesc);

    std::wcout << L"Adapter Name: " << adapterDesc.Description << std::endl;
}

ID3D12Device* Device::GetDevice() const 
{
	return device.Get();
}