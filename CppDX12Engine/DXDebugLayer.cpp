#include "DXDebugLayer.h"
#include "iostream"

bool DXDebugLayer::Init()
{

std::cout << "Entering init"<<std::endl;
#ifdef _DEBUG
//init D3D12 Debug Layer
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_d3d12Debug))))
	{

		m_d3d12Debug->EnableDebugLayer();
		// Init DXGI Debug
		if (SUCCEEDED(DXGIGetDebugInterface1(0,IID_PPV_ARGS(&m_dxgiDebug))))
		{
			m_dxgiDebug->EnableLeakTrackingForThread();
			return true;
		}
	}
	else
	{
		std::cout << "Not succeed";
	}
#endif

	return false;
}

void DXDebugLayer::Shutdown()
{

	if (m_dxgiDebug) 
	{
		OutputDebugString(L"Reports living objects : ");
		m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL,DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
#ifdef _DEBUG
	m_dxgiDebug.Release();
	m_d3d12Debug.Release();
#endif // _DEBUG
}
