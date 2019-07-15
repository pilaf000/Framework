#pragma once
#include <stdafx.h>
#include <GraphicsTypes.h>

namespace Graphics
{

class GraphicsDevice;

class CommandQueue
{
	CommandQueue();
	~CommandQueue() = default;

	class Impl;
	std::unique_ptr<Impl> m_impl;
public:
	struct Description
	{
		CommandQueueFlag flag = CommandQueueFlag::None;
		CommandListType type = CommandListType::Direct;
		Description() = default;
	};
	static CommandQueue* Create(GraphicsDevice* device, Description& desc);
};

}/// end of namespace Graphics