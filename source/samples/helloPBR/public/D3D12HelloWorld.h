#pragma once
#include <DXSample.h>
#include <PrimitiveMeshes.h>

using Microsoft::WRL::ComPtr;

class D3D12HelloWindow : public DXSample
{
public:
    D3D12HelloWindow(UINT width, UINT height, std::wstring name);
    ~D3D12HelloWindow();
    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

private:
    struct Matrix
    {
        DirectX::XMFLOAT4X4 World;
        DirectX::XMFLOAT4X4 View;
        DirectX::XMFLOAT4X4 Projection;
    };

    struct MaterialInfo
    {
        float baseColor[3] = { 1.f, 1.f, 1.f };
        float roughness = 0.1f;
        float metallic = 0.1f;
        float reflectance = 0.1f;
    };
    struct LightInfo
    {
        float direction[3] = { 0.f };
        float color[3] = { 1.f, 1.f, 1.f };
        float intensity = 0.1f;
    };

    static const UINT FrameCount = 2;
    static const UINT TextureWidth = 1300;
    static const UINT TextureHeight = 962;
    static const UINT PixelPerSize = 4;

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;

    ComPtr<ID3D12Device> m_device;

    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    ComPtr<ID3D12CommandQueue> m_commandQueue;

    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
    UINT m_rtvDescriptorSize;

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    //Resources
    ComPtr<ID3D12Resource> m_texture;
    ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
    UINT m_descriptorSize;

    //matrix cbuf
    ComPtr<ID3D12Resource> m_matrixConstantBuffer;
    UINT m_matrixConstantBufferSize;
    UINT8* m_matrixConstantBufferBegin;
    Matrix m_matrix;

    float m_angle;

    // material cbuf
    ComPtr<ID3D12Resource> m_materialConstantBuffer;
    UINT m_materialConstantBufferSize;
    UINT8* m_materialConstantBufferBegin;
    MaterialInfo m_material;

    // material cbuf
    ComPtr<ID3D12Resource> m_lightConstantBuffer;
    UINT m_lightConstantBufferSize;
    UINT8* m_lightConstantBufferBegin;
    LightInfo m_light;

    float m_clearColor[4];

    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue;

    // primitive mesh
    SphereMesh m_sphere;

    void LoadPipeline();
    void LoadAssets();
    void InitializeMatrix();
    void PopulateCommandList();
    void WaitForPreviousFrame();
    void ImGuiUpdate();
};
