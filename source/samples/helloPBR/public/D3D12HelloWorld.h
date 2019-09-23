#pragma once
#include <DXSample.h>
#include <PrimitiveMeshes.h>

using Microsoft::WRL::ComPtr;

class D3D12HelloWindow : public DXSample
{
public:
    D3D12HelloWindow(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

private:
    struct Matrix
    {
        DirectX::XMFLOAT4X4 Model;
        DirectX::XMFLOAT4X4 View;
        DirectX::XMFLOAT4X4 Projection;
    };

    static constexpr UINT FrameCount = 2;
    static constexpr UINT TextureWidth = 1200;
    static constexpr UINT TextureHeight = 1200;
    static constexpr UINT PixelPerSize = 4;

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
    ComPtr<ID3D12Resource> m_constantBuffer;
    UINT m_constantBufferSize;
    UINT8* m_constantBufferBegin;
    Matrix m_matrix;

    float m_angle;

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
};
