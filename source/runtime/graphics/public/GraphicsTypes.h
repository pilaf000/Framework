#pragma once

namespace Graphics
{

enum class CommandQueueFlag : std::uint8_t
{
    None = 0x00,
    DisableGPUFlag = 0x01,
};

enum class CommandListType : std::uint8_t
{
    Direct = 0,
    Bundle,
    Compute,
    Copy,
    VideoDecode,
    VideoProcess
};

}