#pragma once
#include "DXSample.h"

#include <cstdint>
#include <memory>
#include <vector>

using Microsoft::WRL::ComPtr;

class SphereMesh
{
public:
    SphereMesh() = default;
    ~SphereMesh() = default;

    void Init(const ComPtr<ID3D12Device> device);
    void CreateBuffers(const float radius, const int stack, const int slice);
    void Draw(ID3D12GraphicsCommandList* commandList);

    struct SphereVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 uv;
    };

private:
    void CreateVertices(const float& radius, const int& stack, const int& slice);
    void CreateIndices(const int& stack, const int& slice);

private:
    ComPtr<ID3D12Device> m_device;

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    ComPtr<ID3D12Resource> m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

    std::uint32_t m_vertexSize;
    std::uint32_t m_indexSize;
};
