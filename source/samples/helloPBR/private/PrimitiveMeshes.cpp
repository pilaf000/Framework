#include <stdafx.h>

#include <PrimitiveMeshes.h>

void SphereMesh::Init(const ComPtr<ID3D12Device> device)
{
    m_device = device;
}

void SphereMesh::CreateBuffers(const float radius, const int stack, const int slice)
{
    CreateVertices(radius, stack, slice);
    CreateIndices(stack, slice);
    CreateBufferView();
}

void SphereMesh::CreateVertices(const float& radius, const int& stack, const int& slice)
{
    constexpr DirectX::XMFLOAT4 color = { 255.f, 255.f, 255.f, 255.f };
    constexpr auto pi = DirectX::XM_PI;

    float x, y, z, xz;
    float nx, ny, nz, lengthInv = 1.f / radius;
    float u, v;

    auto sliceStep = 2.f * pi * slice;
    auto stackStep = pi / stack;
    float stackAngle, sliceAngle = 0.f;

    for(auto i = 0; i < stack; ++i)
    {
        stackAngle = pi / 2 - i * stackStep;
        xz = radius * std::cosf(stackAngle);
        y = radius * std::sinf(stackAngle);
        for(auto j = 0; j < slice; ++j)
        {

            sliceAngle = j * sliceStep;

            x = xz + std::cosf(sliceAngle);
            z = xz + std::sinf(sliceAngle);
            DirectX::XMFLOAT3 pos = { x, y, z };

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            DirectX::XMFLOAT3 normal = { nx, ny, nz };

            u = (float)j / slice;
            v = (float)i / stack;
            DirectX::XMFLOAT2 uv = { u, v };

            m_vertices.push_back({ pos, color, normal, uv });
        }
    }
}

void SphereMesh::CreateIndices(const int& stack, const int& slice)
{
    int v1, v2 = 0;
    for(int i = 0; i < stack; ++i)
    {
        v1 = i * (slice + 1);
        v2 = v1 + slice + 1;
        for(int j = 0; j < slice; ++j)
        {
            if(i != 0)
            {
                m_indices.push_back(v1);
                m_indices.push_back(v2);
                m_indices.push_back(v1 + 1);
            }
            if(i != (stack - 1))
            {
                m_indices.push_back(v1 + 1);
                m_indices.push_back(v2);
                m_indices.push_back(v2 + 1);
            }
        }
    }
}

void SphereMesh::CreateBufferView()
{
    // vertex buffer view
    {
        const std::uint32_t vertexBufferSize = sizeof(SphereVertex) * m_vertices.size();
		ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));
        void* vtxData;
        ThrowIfFailed(m_vertexBuffer->Map(0, nullptr, &vtxData));
        memcpy(vtxData, m_vertices.data(), vertexBufferSize);
        m_vertexBuffer->Unmap(0, nullptr);

        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = sizeof(SphereVertex);
        m_vertexBufferView.SizeInBytes = vertexBufferSize;
    }

    // index buffer view
    {
        const std::uint32_t indexBufferSize = sizeof(std::uint16_t) * m_indices.size();
        m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_indexBuffer));
        void* indexData;
        m_indexBuffer->Map(0, nullptr, &indexData);
        memcpy(indexData, m_indices.data(), indexBufferSize);
        m_indexBuffer->Unmap(0, nullptr);

        m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
        m_indexBufferView.SizeInBytes = indexBufferSize;
        m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    }
}

const std::vector<SphereMesh::SphereVertex>& SphereMesh::GetVertexList() const
{
    return m_vertices;
}

const std::vector<std::uint16_t>& SphereMesh::GetIndexList() const
{
    return m_indices;
}

const ComPtr<ID3D12Resource> SphereMesh::GetVertexBuffer() const
{
    return m_vertexBuffer;
}
const D3D12_VERTEX_BUFFER_VIEW& SphereMesh::GetVertexBufferView() const
{
    return m_vertexBufferView;
}
const ComPtr<ID3D12Resource> SphereMesh::GetIndexBuffer() const
{
    return m_indexBuffer;
}
const D3D12_INDEX_BUFFER_VIEW& SphereMesh::GetIndexBufferView() const
{
    return m_indexBufferView;
}