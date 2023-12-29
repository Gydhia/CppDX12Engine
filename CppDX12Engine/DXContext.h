#pragma once

#include "WinInclude.h"
#include "ComPointer.h"

class DXContext
{
public : 
	bool Init();
	void Shutdown();

public:
	DXContext(const DXContext&) = delete;
	DXContext& operator=(const DXContext&) = delete;

	inline static DXContext& Get()
	{
		static DXContext instance;
		return instance;
	}

private:
	DXContext() = default;
	ComPointer<ID3D12Device10> m_device;

};

