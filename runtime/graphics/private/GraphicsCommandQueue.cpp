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
			break;
		case Graphics::CommandQueueFlag::DisableGPUFlag:
			return D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
			break;
		default:
			return D3D12_COMMAND_QUEUE_FLAG_NONE;
			break;
	}
}

inline D3D12_COMMAND_LIST_TYPE TypeConversion(Graphics::CommandListType& type)
{
	switch (type)
	{
		case Graphics::CommandListType::Direct:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case Graphics::CommandListType::Bundle:
			return D3D12_COMMAND_LIST_TYPE_BUNDLE;
			break;
		case Graphics::CommandListType::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case Graphics::CommandListType::Copy:
			return D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		case Graphics::CommandListType::VideoDecode:
			return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
			break;
		case Graphics::CommandListType::VideoProcess:
			return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
			break;
		default:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
	}
}
}

namespace Graphics
{

using Microsoft::WRL::ComPtr;

class CommandQueue::Impl
{
public:
	Impl(const Description& desc);
	~Impl() = default;
public:
	Description m_desc;
	ComPtr<ID3D12CommandQueue> m_nativeCommandQueue;
};

CommandQueue::Impl::Impl(const Description& desc)
{
(void)desc;
}



//////////////////////////////////////////////////////

CommandQueue* CommandQueue::Create(GraphicsDevice* device, Description& desc)
{
	return nullptr;
}

CommandQueue::CommandQueue(const Description& desc)
	: m_impl(std::make_unique<Impl>(desc))
{
}

}/// end of namespace Graphics