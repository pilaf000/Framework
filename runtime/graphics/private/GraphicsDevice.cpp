#include <../public/GraphicsDevice.h>

#include <stdafx.h>
#include <GraphicsHelper.h>

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
	ComPtr<ID3D12Debug> m_debugController;
	ComPtr<IDXGIFactory4> m_factory;
	ComPtr<IDXGIAdapter1> m_hardWareAdapter;

	ComPtr<ID3D12Device> m_nativeDevice;

	UINT m_dxgiFactoryFlags;
};

GraphicsDevice::Impl::Impl()
	: m_debugController(nullptr)
	, m_factory(nullptr)
	, m_hardWareAdapter(nullptr)
	, m_nativeDevice(nullptr)
	, m_dxgiFactoryFlags(0)
{
#ifdef _DEBUG
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController))))
	{
		m_debugController->EnableDebugLayer();

		m_dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif
	ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));
	GetHardwareAdapter(m_factory.Get(), &m_hardWareAdapter);

	ThrowIfFailed(D3D12CreateDevice(
		m_hardWareAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&m_nativeDevice)
	));
}

GraphicsDevice::Impl::~Impl()
{
}

const ID3D12Device* GraphicsDevice::Impl::NativeDevice() const
{
	return m_nativeDevice.Get();
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
	return m_impl->NativeDevice();
}

GraphicsDevice::GraphicsDevice()
	: m_impl(std::make_unique<Impl>())
{
}

GraphicsDevice::~GraphicsDevice()
{
	m_impl.reset();
}

}/// end of namespace Graphics