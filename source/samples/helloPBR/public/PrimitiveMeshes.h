#pragma once
#include <DXSample.h>

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

    struct SphereVertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 uv;
    };
    const std::vector<SphereVertex>& GetVertexList() const;
    const std::vector<std::uint16_t>& GetIndexList() const;

    const ComPtr<ID3D12Resource> GetVertexBuffer() const;
    const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const;
    const ComPtr<ID3D12Resource> GetIndexBuffer() const;
    const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const;

private:
    void
    CreateVertices(const float& radius, const int& stack, const int& slice);
    void CreateIndices(const int& stack, const int& slice);
    void CreateBufferView();

private:
    ComPtr<ID3D12Device> m_device;

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    ComPtr<ID3D12Resource> m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

    std::vector<SphereVertex> m_vertices;
    std::vector<std::uint16_t> m_indices;
};
