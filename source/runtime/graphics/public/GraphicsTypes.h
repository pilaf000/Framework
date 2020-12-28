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
    Direct = 0x00,
    Bundle,
    Compute,
    Copy,
    VideoDecode,
    VideoProcess
};

enum class FormatType : std::uint8_t
{
    R8G8B8A8_UNORM = 0x00,
};

}