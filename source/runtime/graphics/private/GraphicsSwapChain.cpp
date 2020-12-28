#include <window.h>

#include <GraphicsSwapChain.h>

namespace Graphics
{

class SwapChain::Impl
{
public:
    Impl(const Common::Window& window);
    ~Impl();

public:
    bool Initialize(const SwapChain::Description& desc);

private:
};

//////////////////////////////////////////////////////////

SwapChain::SwapChain(const Common::Window& window)
    : m_impl(std::make_unique<Impl>(window))
{
}

SwapChain::~SwapChain()
{
    m_impl.reset();
}
} /// end of namespace Graphics