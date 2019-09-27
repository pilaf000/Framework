#include "stdafx.h"
#include <filesystem>
#include <string>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

#include "D3D12HelloWorld.h"
#include "DDSTextureLoader12.h"

D3D12HelloWindow::D3D12HelloWindow(UINT width, UINT height, std::wstring name)
    : DXSample(width, height, name)
    , m_frameIndex(0)
    , m_viewport(0.f, 0.f, static_cast<float>(width), static_cast<float>(height))
    , m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
    , m_rtvDescriptorSize(0)
    , m_matrixConstantBufferBegin(nullptr)
    , m_materialConstantBufferBegin(nullptr)
    , m_lightConstantBufferBegin(nullptr)
    , m_angle(0.f)
    , m_clearColor{ 0.f, 0.2f, 0.4f, 1.f }
{
    InitializeMatrix();
}

D3D12HelloWindow::~D3D12HelloWindow()
{
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void D3D12HelloWindow::OnInit()
{
    InitAssetDir();
    LoadPipeline();
    LoadAssets();
    // imgui intialize
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init(Win32Application::GetHwnd());
        ImGui_ImplDX12_Init(
            m_device.Get(),
            3,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(),
            m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
    }
}

// Update frame-based values.
void D3D12HelloWindow::OnUpdate()
{
    m_angle += 0.0005f;
    if(m_angle > 360.f)
    {
        m_angle = 0.f;
    }

    float radian = m_angle * 180 / DirectX::XM_PI;

    DirectX::XMStoreFloat4x4(&m_matrix.World, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(radian)));

    memcpy(m_matrixConstantBufferBegin, &m_matrix, sizeof(m_matrix));
    ImGuiUpdate();
}

// Render the scene.
void D3D12HelloWindow::OnRender()
{
    PopulateCommandList();

    ID3D12CommandList* commandList[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(commandList), commandList);

    ThrowIfFailed(m_swapChain->Present(1, 0));

    WaitForPreviousFrame();
}

void D3D12HelloWindow::OnDestroy()
{
    if(m_matrixConstantBuffer)
    {
        m_matrixConstantBuffer->Unmap(0, nullptr);
        m_matrixConstantBufferBegin = nullptr;
    }

    if(m_materialConstantBuffer)
    {
        m_materialConstantBuffer->Unmap(0, nullptr);
        m_materialConstantBufferBegin = nullptr;
    }
    if(m_lightConstantBuffer)
    {
        m_lightConstantBuffer->Unmap(0, nullptr);
        m_lightConstantBufferBegin = nullptr;
    }

    WaitForPreviousFrame();

    CloseHandle(m_fenceEvent);
}

namespace
{
inline std::wstring find_root_path_w(std::wstring& path)
{
    constexpr auto rootName = L"root";
    if(std::filesystem::exists(path + rootName))
    {
        return path;
    }
    return find_root_path_w(path + L"..\\");
}
}

void D3D12HelloWindow::InitAssetDir()
{
    using namespace std::string_literals;
    wchar_t currentDir[255];
    ::GetCurrentDirectoryW(255, currentDir);

    std::wstring currentDirW(currentDir);
    m_assetsDir = find_root_path_w(currentDirW + L"\\"s) + L"assets\\"s;
};

// Load the rendering pipeline dependencies.
void D3D12HelloWindow::LoadPipeline()
{
    UINT dxgiFactoryFlag = 0;

    ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory2(dxgiFactoryFlag, IID_PPV_ARGS(&factory));

    //Create the GraphicsDevice.
    ThrowIfFailed(D3D12CreateDevice(
        nullptr,
        D3D_FEATURE_LEVEL_11_0,
        IID_PPV_ARGS(&m_device)));

    //Create the command queue.
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
    }

    //Create the swap chain.
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = m_width;
        swapChainDesc.Height = m_height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        ComPtr<IDXGISwapChain1> swapChain;
        factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            Win32Application::GetHwnd(),
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain);

        factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
        swapChain.As(&m_swapChain);
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    // Create descriptor heap.
    {
        // Render target view.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap));

        // Shader Resource view.
        D3D12_DESCRIPTOR_HEAP_DESC srvcbvHeapDesc = {};
        srvcbvHeapDesc.NumDescriptors = 100;
        srvcbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvcbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        m_device->CreateDescriptorHeap(&srvcbvHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for(UINT i = 0; i < FrameCount; i++)
        {
            m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    // Create the command allocator.
    m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
}

// Load the sample assets.
void D3D12HelloWindow::LoadAssets()
{
    using namespace std::string_literals;

    // Create the root signature.
    {
        D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
        if(FAILED(m_device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_DESCRIPTOR_RANGE1 ranges[4];
        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 10, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 2, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

        CD3DX12_ROOT_PARAMETER1 rootParameters[3];
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[2].InitAsDescriptorTable(2, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(m_device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)));
    }

    // Create the pipeline state.
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;
#ifdef _DEBUG
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        ThrowIfFailed(D3DCompileFromFile((m_assetsDir + L"VS.hlsl"s).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile((m_assetsDir + L"PS.hlsl"s).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        // Create the graphics pipeline state objects (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
    }
    // Creaate matrix constant buffer.
    {
        m_matrixConstantBufferSize = (sizeof(Matrix) + 255) & ~255;
        CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_matrixConstantBufferSize);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &constantBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_matrixConstantBuffer)));

        m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, m_descriptorSize);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_matrixConstantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = m_matrixConstantBufferSize;
        m_device->CreateConstantBufferView(&cbvDesc, cbvCpuHandle);

        CD3DX12_RANGE readRenge(0, 0);
        ThrowIfFailed(m_matrixConstantBuffer->Map(0, &readRenge, reinterpret_cast<void**>(&m_matrixConstantBufferBegin)));
        ZeroMemory(m_matrixConstantBufferBegin, m_matrixConstantBufferSize);
    }
    // material const buffer
    {
        m_materialConstantBufferSize = (sizeof(MaterialInfo) + 255) & ~255;
        CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_materialConstantBufferSize);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &constantBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_materialConstantBuffer)));

        m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 2, m_descriptorSize);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_materialConstantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = m_materialConstantBufferSize;
        m_device->CreateConstantBufferView(&cbvDesc, cbvCpuHandle);

        CD3DX12_RANGE readRenge(0, 0);
        ThrowIfFailed(m_materialConstantBuffer->Map(0, &readRenge, reinterpret_cast<void**>(&m_materialConstantBufferBegin)));
        ZeroMemory(m_materialConstantBufferBegin, m_materialConstantBufferSize);
    }

    // light const buffer
    {
        m_lightConstantBufferSize = (sizeof(LightInfo) + 255) & ~255;
        CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_lightConstantBufferSize);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &constantBufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_lightConstantBuffer)));

        m_descriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 3, m_descriptorSize);

        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = m_lightConstantBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = m_lightConstantBufferSize;
        m_device->CreateConstantBufferView(&cbvDesc, cbvCpuHandle);

        CD3DX12_RANGE readRenge(0, 0);
        ThrowIfFailed(m_lightConstantBuffer->Map(0, &readRenge, reinterpret_cast<void**>(&m_lightConstantBufferBegin)));
        ZeroMemory(m_lightConstantBufferBegin, m_lightConstantBufferSize);
        // Create mesh buffers.
        m_sphere.Init(m_device);
        m_sphere.CreateBuffers(1.f, 72, 72);

        // Create the command list.
        m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));

        ComPtr<ID3D12Resource> textureUploadHeap;
        // Create the texture.
        {
            std::unique_ptr<uint8_t[]> ddsData;
            std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;
            ThrowIfFailed(DirectX::LoadDDSTextureFromFile(m_device.Get(),
                (m_assetsDir + L"earth.dds"s).c_str(),
                &m_texture,
                ddsData,
                subresouceData));
            D3D12_RESOURCE_DESC textureDesc = m_texture->GetDesc();

            const UINT subresoucesize = static_cast<UINT>(subresouceData.size());
            const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture.Get(), 0, subresoucesize);

            // Create the GPU upload buffer.
            ThrowIfFailed(m_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&textureUploadHeap)));

            UpdateSubresources(m_commandList.Get(), m_texture.Get(), textureUploadHeap.Get(), 0, 0, subresoucesize, &subresouceData[0]);
            m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

            // Describe and create a SRV for the texture.
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = subresoucesize;
            CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart(), 10, m_descriptorSize);
            m_device->CreateShaderResourceView(m_texture.Get(), &srvDesc, srvHandle);

            m_commandList->DiscardResource(textureUploadHeap.Get(), nullptr);
        }
        ThrowIfFailed(m_commandList->Close());
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // Create sync objects.
        {
            m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
            m_fenceValue = 1;

            m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if(!m_fenceEvent)
            {
                ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
            }
            WaitForPreviousFrame();
        }
    }
}

void D3D12HelloWindow::InitializeMatrix()
{
    float fovAngleY = 70.0f * DirectX::XM_PI / 180.0f;

    if(m_aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(
        fovAngleY,
        m_aspectRatio,
        0.01f,
        100.0f);

    DirectX::XMFLOAT4X4 orientation = DirectX::XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
    DirectX::XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

    XMStoreFloat4x4(
        &m_matrix.Projection,
        XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

    m_material.view[0] = 0.f;
    m_material.view[1] = 0.f;
    m_material.view[2] = -5.f;

    constexpr DirectX::XMVECTORF32 eye = { 0.0f, 0.0f, -5.f, 0.0f };
    constexpr DirectX::XMVECTORF32 at = { 0.0f, 0.0f, 0.0f, 0.0f };
    constexpr DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
    XMStoreFloat4x4(&m_matrix.View, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
}

void D3D12HelloWindow::PopulateCommandList()
{
    ThrowIfFailed(m_commandAllocator->Reset());

    ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

    // Set necessary state.
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { m_descriptorHeap.Get() };
    m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 10, m_descriptorSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE matrixCbvHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1, m_descriptorSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE materialCbvHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), 2, m_descriptorSize);
    m_commandList->SetGraphicsRootDescriptorTable(0, srvHandle);
    m_commandList->SetGraphicsRootDescriptorTable(1, matrixCbvHandle);
    m_commandList->SetGraphicsRootDescriptorTable(2, materialCbvHandle);

    m_commandList->RSSetViewports(1, &m_viewport);
    m_commandList->RSSetScissorRects(1, &m_scissorRect);

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Set render target;
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    m_commandList->ClearRenderTargetView(rtvHandle, m_clearColor, 0, nullptr);

    m_sphere.Draw(m_commandList.Get());

    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), m_commandList.Get());

    m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    ThrowIfFailed(m_commandList->Close());
}

void D3D12HelloWindow::WaitForPreviousFrame()
{
    // Signal and increment the fence value.
    const UINT64 fence = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
    m_fenceValue++;

    // Wait until the previous frame is finished.
    if(m_fence->GetCompletedValue() < fence)
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void D3D12HelloWindow::ImGuiUpdate()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Material Info");
        ImGui::ColorEdit3("Base Color", (float*)&m_material.baseColor);
        ImGui::SliderFloat("Reflectance", &m_material.reflectance, 0.0f, 1.0f);
        ImGui::SliderFloat("Metallic", &m_material.metallic, 0.0f, 1.0f);
        ImGui::SliderFloat("Roughness", &m_material.roughness, 0.0f, 1.0f);
        ImGui::End();

        ImGui::Begin("Light Info");
        ImGui::ColorEdit3("Light Color", (float*)m_light.color);
        ImGui::SliderFloat3("Direction", (float*)m_light.direction, -2.f, 2.f);
        ImGui::SliderFloat("Intensity", &m_light.intensity, 0.01f, 10.f);
        ImGui::End();
    }
    memcpy(m_materialConstantBufferBegin, &m_material, sizeof(m_material));
    memcpy(m_lightConstantBufferBegin, &m_light, sizeof(m_light));
}