#pragma once
#include <stdafx.h>

namespace Graphics
{

class GraphicsDevice
{
	class Impl;
	std::unique_ptr<Impl> m_impl;
public:
	GraphicsDevice();
	~GraphicsDevice();

	const ID3D12Device* NativeDevice() const;
};

}/// end of namespace Graphics