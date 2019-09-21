#pragma once
#include <stdafx.h>
#include <GraphicsTypes.h>

namespace Graphics
{
class GraphicsDevice;

class CommandQueue
{
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
	
	CommandQueue(const Description& desc);
	~CommandQueue() = default;
};

}/// end of namespace Graphics