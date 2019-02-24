#include "stdafx.h"
#include "GraphicsCommandQueue.h"

#include "GraphicsDevice.h"

namespace Graphics
{

using Microsoft::WRL::ComPtr;

class CommandQueue::Impl
{
public:
	Impl();
	~Impl();
public:
	Description Desc;
	ComPtr<ID3D12CommandQueue> NativeCommandQueue;
};

inline D3D12_COMMAND_QUEUE_FLAGS TypeConversion(CommandQueueFlag& flag)
{
	switch (flag)
	{
		case CommandQueueFlag::None:
			return D3D12_COMMAND_QUEUE_FLAG_NONE;
			break;
		case CommandQueueFlag::DisableGPUFlag:
			return D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
			break;
		default:
			return D3D12_COMMAND_QUEUE_FLAG_NONE;
			break;
	}
}

inline D3D12_COMMAND_LIST_TYPE TypeConversion(CommandListType& type)
{
	switch (type)
	{
		case CommandListType::Direct:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case CommandListType::Bundle:
			return D3D12_COMMAND_LIST_TYPE_BUNDLE;
			break;
		case CommandListType::Compute:
			return D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		case CommandListType::Copy:
			return D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		case CommandListType::VideoDecode:
			return D3D12_COMMAND_LIST_TYPE_VIDEO_DECODE;
			break;
		case CommandListType::VideoProcess:
			return D3D12_COMMAND_LIST_TYPE_VIDEO_PROCESS;
			break;
		default:
			return D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
	}
}

//////////////////////////////////////////////////////


CommandQueue* CommandQueue::Create(GraphicsDevice* device, Description& desc)
{
	return nullptr;
}

CommandQueue::CommandQueue()
	: m_Impl(std::make_unique<Impl>())
{
}



}/// end of namespace Graphics