#pragma once

namespace Graphics
{

enum class CommandQueueFlag
{
	None = 0x00,
	DisableGPUFlag = 0x01,
};

enum class CommandListType
{
	Direct = 0,
	Bundle,
	Compute,
	Copy,
	VideoDecode,
	VideoProcess
};

}