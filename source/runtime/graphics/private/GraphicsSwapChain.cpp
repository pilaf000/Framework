#include <stdafx.h>
#include <window.h>

#include <GraphicsSwapChain.h>

namespace
{
inline DXGI_FORMAT TypeConversion(Graphics::FormatType& format)
{
    switch (format)
    {
    case Graphics::FormatType::R8G8B8A8_UNORM:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    default:
        return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    }
}
}

namespace Graphics
{

class SwapChain::Impl
{
public:
    Impl(std::shared_ptr<Common::Window> window);
    ~Impl();

public:
    bool Initialize(const SwapChain::Description& desc);

private:
    std::weak_ptr<Common::Window> m_window;
};

Graphics::SwapChain::Impl::Impl(std::shared_ptr<Common::Window> window)
    : m_window(window)
{
}

bool SwapChain::Impl::Initialize(const SwapChain::Description& desc)
{
    DXGI_SWAP_CHAIN_DESC1 nativeDesc = {};
    nativeDesc.BufferCount = desc.frameCount;
    nativeDesc.Width = desc.width;
    nativeDesc.Height = desc.height;
    /// todo:fix me.
    nativeDesc.Format = TypeConversion(const_cast<SwapChain::Description&>(desc).format);
    return true;
}

//////////////////////////////////////////////////////////

SwapChain::SwapChain(std::shared_ptr<Common::Window> window)
    : m_impl(std::make_unique<Impl>(window))
{
}

SwapChain::~SwapChain()
{
    m_impl.reset();
}
} /// end of namespace Graphics