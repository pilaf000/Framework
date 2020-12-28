#pragma once
#include <GraphicsTypes.h>

#include <memory>

namespace Common
{
class Window;
}

namespace Graphics
{

class SwapChain
{
public:
    struct Description
    {
        std::uint32_t frameCount;
        FormatType format = FormatType::R8G8B8A8_UNORM;
        std::uint32_t width;
        std::uint32_t height;
    };

public:
    SwapChain(const Common::Window& window);
    ~SwapChain();

public:
    bool Initialize(const Description& desc);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} /// end of namespace Graphics;