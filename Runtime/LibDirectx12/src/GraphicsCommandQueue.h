#pragma once
#include "GraphicsTypes.h"

namespace Graphics
{

class GraphicsDevice;

class CommandQueue
{
	CommandQueue();
	~CommandQueue() = default;

	class Impl;
	std::unique_ptr<Impl> m_Impl;
public:
	struct Description
	{
		CommandQueueFlag Flag = CommandQueueFlag::None;
		CommandListType Type = CommandListType::Direct;
		Description() = default;
	};
	static CommandQueue* Create(GraphicsDevice* device, Description& desc);
};

}/// end of namespace Graphics