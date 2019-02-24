#include "GraphicsDevice.h"

#include "stdafx.h"
#include "GraphicsHelper.h"

namespace Graphics
{
using namespace DirectX;
using Microsoft::WRL::ComPtr;

class GraphicsDevice::Impl
{
public:
	Impl();
	~Impl();
public:
	const ID3D12Device* NativeDevice() const;
private:
	void GetHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** adapters);
private:
	ComPtr<ID3D12Debug> m_DebugController;
	ComPtr<IDXGIFactory4> m_Factory;
	ComPtr<IDXGIAdapter1> m_HardWareAdapter;

	ComPtr<ID3D12Device> m_NativeDevice;

	UINT m_DxgiFactoryFlags;
};

GraphicsDevice::Impl::Impl()
	: m_DebugController(nullptr)
	, m_Factory(nullptr)
	, m_HardWareAdapter(nullptr)
	, m_NativeDevice(nullptr)
	, m_DxgiFactoryFlags(0)
{
#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController))))
	{
		m_DebugController->EnableDebugLayer();

		m_DxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	ThrowIfFailed(CreateDXGIFactory2(m_DxgiFactoryFlags, IID_PPV_ARGS(&m_Factory)));
	GetHardwareAdapter(m_Factory.Get(), &m_HardWareAdapter);

	ThrowIfFailed(D3D12CreateDevice(
		m_HardWareAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_NativeDevice)
	));
}

GraphicsDevice::Impl::~Impl()
{
}

const ID3D12Device* GraphicsDevice::Impl::NativeDevice() const
{
	return m_NativeDevice.Get();
}


void GraphicsDevice::Impl::GetHardwareAdapter(IDXGIFactory2* factory, IDXGIAdapter1** adapters)
{
	ComPtr<IDXGIAdapter1> adapter;

	for (auto adapterIndex = 0U; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc = {};
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			break;
		}
	}
}

//////////////////////////////////////////////////////////

GraphicsDevice* GraphicsDevice::Create()
{
	
	return nullptr;
}

const ID3D12Device* GraphicsDevice::NativeDevice() const
{
	return m_Impl->NativeDevice();
}

GraphicsDevice::GraphicsDevice()
	: m_Impl(std::make_unique<Impl>())
{
}

GraphicsDevice::~GraphicsDevice()
{
	m_Impl.reset();
}

}/// end of namespace Graphics