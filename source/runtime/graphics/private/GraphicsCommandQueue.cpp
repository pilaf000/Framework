#include <stdafx.h>

#include <GraphicsCommandQueue.h>
#include <GraphicsDevice.h>

namespace
{
inline D3D12_COMMAND_QUEUE_FLAGS TypeConversion(Graphics::CommandQueueFlag& flag)
{
    switch (flag)
    {
    case Graphics::CommandQueueFlag::None:
        return D3D12_COMMAND_QUEUE_FLAG_NONE;
    case Graphics::CommandQueueFlag::DisableGPUFlag:
        return D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
    default:
        return D3D12_COMMAND_QUEUE_FLAG_NONE;
    }
}

inline D3D12_COMMAND_LIST_TYPE TypeConversion(Graphics::CommandListType& type)
{
    switch (type)
    {
    case Graphics::CommandListType::Direct:
        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    case Graphics::CommandListType::Bundle:
        return D3D12_COMMAND_LIST_TYPE_BUNDLE;
    case Graphics::CommandListType::Compute:
        return D3D12_COMMAND_LIST_TYPE_COMPUTE;
    case Graphics::CommandListType::Copy:
        return D3D12_COMMAND_LIST_TYPE_COPY;
    case Graphics::CommandListType::VideoDecode:
        return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
    case Graphics::CommandListType::VideoProcess:
        return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
    default:
        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

inline D3D12_COMMAND_QUEUE_DESC TypeConversion(Graphics::CommandQueue::Description& desc)
{
    return { TypeConversion(desc.type), TypeConversion(desc.flag) };
}

} /// end of namespace

namespace Graphics
{

using Microsoft::WRL::ComPtr;

class CommandQueue::Impl
{
public:
    Impl(const Description& desc);
    ~Impl() = default;

    const bool Initialize(const GraphicsDevice& device);

private:
    Description m_desc;
    ComPtr<ID3D12CommandQueue> m_nativeCommandQueue;
};

CommandQueue::Impl::Impl(const Description& desc)
    : m_desc(desc)
{
}

const bool CommandQueue::Impl::Initialize(const GraphicsDevice& device)
{
    auto nativeDevice = device.NativeDevice();
    auto nativeDesc = TypeConversion(m_desc);
    nativeDevice->CreateCommandQueue(&nativeDesc, IID_PPV_ARGS(&m_nativeCommandQueue));
}

//////////////////////////////////////////////////////

CommandQueue::CommandQueue(const Description& desc)
    : m_impl(std::make_unique<Impl>(desc))
{
}

const bool CommandQueue::IsEnabled() const
{
    return;
}

const bool CommandQueue::Initialize(const GraphicsDevice& device)
{
    return m_impl->Initialize(device);
}

} /// end of namespace Graphics