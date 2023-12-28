#include "WinInclude.h"
#include "ComPointer.h"
#include "DXDebugLayer.h"

int main() 
{


	DXDebugLayer::Get().Init();

	DXDebugLayer::Get().Shutdown();
	//ComPointer<IUnknown> pointer;
	//pointer.QueryInterface(pointer);
	//pointer.Release();
	// 
	//Device device;
	//device.Initialize();

	//ID3D12Device* d3d12Device = device.GetDevice();

	//if (d3d12Device != nullptr)
	//{
	//	std::cout << "Init sucess : " << device.GetDevice() << std::endl;
	//	device.PrintDeviceInfo();
	//}
	//else
	//{
	//	std::cout << "Init failed" << std::endl;
	//}
}