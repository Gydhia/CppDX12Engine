#include "DXDebugLayer.h"

bool DXDebugLayer::Init()
{
#ifdef _DEBUG
//init D3D12 Debug Layer
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug))))
	{
		m_debug->EnableDebugLayer();
		// Init DXGI Debug
		if (SUCCEEDED(DXGIGetDebugInterface1(0,IID_PPV_ARGS(&m_dxgiDebug))))
		{
			m_dxgiDebug->EnableLeakTrackingForThread();
			return true;
		}
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
	m_debug.Release();
#endif // _DEBUG
}
