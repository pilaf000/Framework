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
}

void SphereMesh::CreateVertices(const float& radius, const int& stack, const int& slice)
{
    std::vector<SphereVertex> vertices;
    constexpr DirectX::XMFLOAT4 color = { 255.f, 255.f, 255.f, 255.f };
    constexpr auto pi = DirectX::XM_PI;

    float x, y, z, xz;
    float nx, ny, nz, lengthInv = 1.f / radius;
    float u, v;

    auto sliceStep = 2.f * pi / slice;
    auto stackStep = pi / stack;
    float stackAngle, sliceAngle = 0.f;

    for(auto i = 0; i <= stack; ++i)
    {
        stackAngle = pi / 2 - i * stackStep;
        xz = radius * std::cosf(stackAngle);
        y = radius * std::sinf(stackAngle);
        for(auto j = 0; j <= slice; ++j)
        {
            sliceAngle = j * sliceStep;

            x = xz * std::cosf(sliceAngle);
            z = xz * std::sinf(sliceAngle);
            DirectX::XMFLOAT3 pos = { x, y, z };

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            DirectX::XMFLOAT3 normal = { nx, ny, nz };

            u = (float)j / slice;
            v = (float)i / stack;
            DirectX::XMFLOAT2 uv = { u, v };

            vertices.push_back({ pos, color, normal, uv });
        }
    }
    // vertex buffer view
    const UINT vertexBufferSize = sizeof(SphereVertex) * vertices.size();
    ThrowIfFailed(m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer)));
    std::uint8_t* vtxData;
    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vtxData)));
    memcpy(vtxData, vertices.data(), vertexBufferSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.StrideInBytes = sizeof(SphereVertex);
    m_vertexBufferView.SizeInBytes = vertexBufferSize;
}

void SphereMesh::CreateIndices(const int& stack, const int& slice)
{
    std::vector<std::uint16_t> indices;
    int v1, v2 = 0;
    for(int i = 0; i < stack; ++i)
    {
        v1 = i * (slice + 1);
        v2 = v1 + slice + 1;
        for(int j = 0; j < slice; ++j, ++v1, ++v2)
        {
            if(i != 0)
            {
                indices.push_back(v1);
                indices.push_back(v1 + 1);
                indices.push_back(v2);
            }
            if(i != (stack - 1))
            {
                indices.push_back(v1 + 1);
                indices.push_back(v2 + 1);
                indices.push_back(v2);
            }
        }
    }

    // index buffer view
    m_indexSize = indices.size();
    const UINT indexBufferSize = sizeof(std::uint16_t) * indices.size();
    m_device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_indexBuffer));
    std::uint8_t* indexData;
    CD3DX12_RANGE readRange(0, 0);
    m_indexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&indexData));
    memcpy(indexData, indices.data(), indexBufferSize);
    m_indexBuffer->Unmap(0, nullptr);

    m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
    m_indexBufferView.SizeInBytes = indexBufferSize;
    m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

void SphereMesh::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commandList->IASetIndexBuffer(&m_indexBufferView);
    commandList->DrawIndexedInstanced(m_indexSize, 1, 0, 0, 0);
}