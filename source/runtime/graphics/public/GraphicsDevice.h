#pragma once
#include <memory>

namespace Graphics
{

class GraphicsCommandQueue;

class GraphicsDevice
{
public:
    GraphicsDevice();
    ~GraphicsDevice();

public:
    ID3D12Device* NativeDevice() const;

    GraphicsCommandQueue* CreateCommandQueue(const CommandQueue::Description& desc) const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} /// end of namespace Graphics