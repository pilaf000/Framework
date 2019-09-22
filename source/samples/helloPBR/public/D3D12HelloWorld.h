#pragma once
#include "DXSample.h"

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
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};
	
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
	CD3DX12_VIEWPORT					m_Viewport;
	CD3DX12_RECT						m_ScissorRect;

	ComPtr<ID3D12Device>				m_Device;
	
	ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	ComPtr<ID3D12Resource>				m_RenderTargets[FrameCount];
	ComPtr<ID3D12DescriptorHeap>		m_RTVDescriptorHeap;
	UINT								m_RTVDescriptorSize;
	
	ComPtr<ID3D12RootSignature>			m_RootSignature;
	ComPtr<ID3D12PipelineState>			m_PipelineState;

	//Resources
	ComPtr<ID3D12Resource>				m_VertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW			m_VertexBufferView;
	ComPtr<ID3D12Resource>				m_IndexBuffer;
	D3D12_INDEX_BUFFER_VIEW				m_IndexBufferView;
	ComPtr<ID3D12Resource>				m_Texture;
	ComPtr<ID3D12DescriptorHeap>		m_SRVCBVDescriptorHeap;
	UINT								m_SRVCBVDescriptorSize;
	ComPtr<ID3D12Resource>				m_ConstantBuffer;
	UINT								m_ConstantBufferSize;
	UINT8*								m_ConstantBufferBegin;
	Matrix								m_CBMatrix;

	float								m_angle;

	// Synchronization objects.
	UINT								m_FrameIndex;
	HANDLE								m_FenceEvent;
	ComPtr<ID3D12Fence>					m_Fence;
	UINT64								m_FenceValue;

	void LoadPipeline();
	void LoadAssets();
	void InitializeMatrix();
	void PopulateCommandList();
	void WaitForPreviousFrame();
};
