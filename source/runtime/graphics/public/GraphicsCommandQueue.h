#pragma once
#include <memory>

#include <GraphicsTypes.h>

namespace Graphics
{
class GraphicsDevice;

class CommandQueue
{
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

public:
    const bool IsEnabled() const;

    const bool Initialize(const GraphicsDevice& device);

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} /// end of namespace Graphics