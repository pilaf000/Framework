#include "stdafx.h"
#include <string>

#include "D3D12HelloWorld.h"
#include "DDSTextureLoader12.h"

D3D12HelloWindow::D3D12HelloWindow(UINT width, UINT height, std::wstring name)
	: DXSample(width, height, name)
	, m_FrameIndex(0)
	, m_Viewport(0.f, 0.f, static_cast<float>(width), static_cast<float>(height))
	, m_ScissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height))
	, m_RTVDescriptorSize(0)
	, m_ConstantBufferBegin(nullptr)
	, m_angle(0.f)
{
	InitializeMatrix();
}

void D3D12HelloWindow::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

// Update frame-based values.
void D3D12HelloWindow::OnUpdate()
{
	m_angle += 0.0005f;
	if (m_angle > 360.f)
	{
		m_angle = 0.f;
	}
	
	float radian = m_angle * 180 / DirectX::XM_PI;

	DirectX::XMStoreFloat4x4(&m_CBMatrix.Model, DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationY(radian)));

	//・ｽE・ｽ關費ｿｽo・ｽE・ｽb・ｽE・ｽt・ｽE・ｽ@・ｽE・ｽ[ ・ｽE・ｽ・ｽE・ｽ・ｽE・ｽ\・ｽE・ｽ[・ｽE・ｽX・ｽE・ｽ・ｽE・ｽ・ｽE・ｽX・ｽE・ｽV・ｽE・ｽ・ｽE・ｽ・ｽE・ｽﾜゑｿｽ・ｽE・ｽB
	memcpy(m_ConstantBufferBegin, &m_CBMatrix, sizeof(m_CBMatrix));
}

// Render the scene.
void D3D12HelloWindow::OnRender()
{
	PopulateCommandList();

	ID3D12CommandList* commandList[] = {m_CommandList.Get()};
	m_CommandQueue->ExecuteCommandLists(_countof(commandList), commandList);

	ThrowIfFailed(m_SwapChain->Present(1, 0));

	WaitForPreviousFrame();
}

void D3D12HelloWindow::OnDestroy()
{
	m_ConstantBuffer->Unmap(0, nullptr);
	m_ConstantBufferBegin = nullptr;

	WaitForPreviousFrame();

	CloseHandle(m_FenceEvent);
}

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
		IID_PPV_ARGS(&m_Device)));

	//Create the command queue.
	{
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
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
			m_CommandQueue.Get(),
			Win32Application::GetHwnd(),
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain);

		factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER);
		swapChain.As(&m_SwapChain);
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

	// Create descriptor heap.
	{
		// Render target view.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTVDescriptorHeap));

		// Shader Resource view.
		D3D12_DESCRIPTOR_HEAP_DESC srvcbvHeapDesc = {};
		srvcbvHeapDesc.NumDescriptors = 2;
		srvcbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvcbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		m_Device->CreateDescriptorHeap(&srvcbvHeapDesc, IID_PPV_ARGS(&m_SRVCBVDescriptorHeap));

		m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT i = 0; i < FrameCount; i++)
		{
			m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_RTVDescriptorSize);
		}
	}

	// Create the command allocator.
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));
}

// Load the sample assets.
void D3D12HelloWindow::LoadAssets()
{
	using namespace std::string_literals;
	const auto assetsDir = L"../../../source/samples/helloPBR/assets/"s;

	// Create the root signature.
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_VERTEX);

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
		ThrowIfFailed(m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
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
		ThrowIfFailed(D3DCompileFromFile((assetsDir + L"VS.hlsl"s).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
		ThrowIfFailed(D3DCompileFromFile((assetsDir + L"PS.hlsl"s).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

		// Define the vertex input layout.
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = 
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		// Create the graphics pipeline state objects (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = {inputLayout, _countof(inputLayout)};
		psoDesc.pRootSignature = m_RootSignature.Get();
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
		m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PipelineState));
	}

	// Create the vertex buffer.
	{
		Vertex triangleVertices[]=
		{
			{{-0.25, 0.25f, -0.25f}, {0.f, 0.f}}, // -z
			{{0.25f, 0.25f, -0.25f}, {1.f, 0.f}},
			{{0.25f, -0.25f, -0.25f}, {1.f, 1.f,}},
			{{-0.25f, -0.25f, -0.25f}, {0.f, 1.f}},

			{{0.25, 0.25f, 0.25f}, {0.f, 0.f}}, // +z
			{{-0.25f, 0.25f, 0.25f}, {1.f, 0.f}},
			{{-0.25f, -0.25f, 0.25f}, {1.f, 1.f,}},
			{{0.25f, -0.25f, 0.25f}, {0.f, 1.f}},

			{{0.25, 0.25f, -0.25f}, {0.f, 0.f}}, // +x
			{{0.25f, 0.25f, 0.25f}, {1.f, 0.f}},
			{{0.25f, -0.25f, 0.25f}, {1.f, 1.f,}},
			{{0.25f, -0.25f, -0.25f}, {0.f, 1.f}},

			{{-0.25, 0.25f, 0.25f}, {0.f, 0.f}}, // -x
			{{-0.25f, 0.25f, -0.25f}, {1.f, 0.f}},
			{{-0.25f, -0.25f, -0.25f}, {1.f, 1.f,}},
			{{-0.25f, -0.25f, 0.25f}, {0.f, 1.f}},


			{{-0.25, 0.25f, 0.25f}, {0.f, 0.f}}, // +y
			{{0.25f, 0.25f, 0.25f}, {1.f, 0.f}},
			{{0.25f, 0.25f, -0.25f}, {1.f, 1.f,}},
			{{-0.25f, 0.25f, -0.25f}, {0.f, 1.f}},

			{{-0.25, -0.25f, -0.25f}, {0.f, 0.f}}, // -y
			{{0.25f, -0.25, -0.25f}, {1.f, 0.f}},
			{{0.25f, -0.25f, 0.25f}, {1.f, 1.f,}},
			{{-0.25f, -0.25, 0.25f}, {0.f, 1.f}}
		};
		const UINT vertexBufferSize = sizeof(triangleVertices);

		m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_VertexBuffer)
		);
		UINT8* vertexDateBegin;
		CD3DX12_RANGE readRange(0, 0);
		m_VertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&vertexDateBegin));
		memcpy(vertexDateBegin, triangleVertices, sizeof(triangleVertices));
		m_VertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view;
		m_VertexBufferView.BufferLocation = m_VertexBuffer->GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex);
		m_VertexBufferView.SizeInBytes = vertexBufferSize;
	}

	// Create the index buffer.
	{
		UINT indexList[] =
		{
			0, 1, 2, 0, 2, 3,
			4, 5, 6, 4, 6, 7,
			8, 9, 10, 8, 10, 11,
			12, 13, 14, 12, 14, 15,
			16, 17, 18, 16, 18, 19,
			20, 21, 22, 20, 22, 23
		};
		const UINT indexBufferSize = sizeof(indexList);
		
		m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_IndexBuffer)
		);
		UINT8* indexDataBegin;
		CD3DX12_RANGE readRange(0, 0);
		m_IndexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&indexDataBegin));
		memcpy(indexDataBegin, indexList, indexBufferSize);
		m_IndexBuffer->Unmap(0, nullptr);
		
		// Initialize the index buffer view.
		m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
		m_IndexBufferView.SizeInBytes = indexBufferSize;
		m_IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}

	// Creaate constant buffer.
	{
		m_ConstantBufferSize = (sizeof(Matrix) + 255) & ~255;
		CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_ConstantBufferSize);
		ThrowIfFailed(m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&constantBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_ConstantBuffer)));

		m_SRVCBVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_CPU_DESCRIPTOR_HANDLE cbvCpuHandle(m_SRVCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, m_SRVCBVDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		cbvDesc.SizeInBytes = m_ConstantBufferSize;
		m_Device->CreateConstantBufferView(&cbvDesc, cbvCpuHandle);

		CD3DX12_RANGE readRenge(0, 0);
		ThrowIfFailed(m_ConstantBuffer->Map(0, &readRenge, reinterpret_cast<void**>(&m_ConstantBufferBegin)));
		ZeroMemory(m_ConstantBufferBegin, m_ConstantBufferSize);
	}
	// Create the command list.
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));

	ComPtr<ID3D12Resource> textureUploadHeap;
	// Create the texture.
	{
		std::unique_ptr<uint8_t[]> ddsData;
		std::vector<D3D12_SUBRESOURCE_DATA> subresouceData;
		ThrowIfFailed(DirectX::LoadDDSTextureFromFile(m_Device.Get(),
			(assetsDir + L"directx12.dds"s).c_str(),
			&m_Texture,
			ddsData,
			subresouceData));
		D3D12_RESOURCE_DESC textureDesc = m_Texture->GetDesc();

		const UINT subresoucesize = static_cast<UINT>(subresouceData.size());
		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_Texture.Get(), 0, subresoucesize);

		// Create the GPU upload buffer.
		ThrowIfFailed(m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&textureUploadHeap)));

		UpdateSubresources(m_CommandList.Get(), m_Texture.Get(), textureUploadHeap.Get(), 0, 0, subresoucesize, &subresouceData[0]);
		m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_Texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		// Describe and create a SRV for the texture.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = subresoucesize;
		CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(m_SRVCBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 0, 0);
		m_Device->CreateShaderResourceView(m_Texture.Get(), &srvDesc, srvHandle);

		m_CommandList->DiscardResource(textureUploadHeap.Get(), nullptr);
	}
	ThrowIfFailed(m_CommandList->Close());
	ID3D12CommandList* ppCommandLists[] = {m_CommandList.Get()};
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Create sync objects.
	{
		m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		m_FenceValue = 1;

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (!m_FenceEvent)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
		WaitForPreviousFrame();
	}
}

void D3D12HelloWindow::InitializeMatrix()
{
	float fovAngleY = 70.0f * DirectX::XM_PI / 180.0f;

	if (m_aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(
		fovAngleY,
		m_aspectRatio,
		0.01f,
		100.0f
	);

	DirectX::XMFLOAT4X4 orientation = DirectX::XMFLOAT4X4(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
	DirectX::XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_CBMatrix.Projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
	);

	constexpr DirectX::XMVECTORF32 eye = {0.0f, 0.5f, -1.5f, 0.0f};
	constexpr DirectX::XMVECTORF32 at = {0.0f, 0.0f, 0.0f, 0.0f};
	constexpr DirectX::XMVECTORF32 up = {0.0f, 1.0f, 0.0f, 0.0f};

	XMStoreFloat4x4(&m_CBMatrix.View, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
}

void D3D12HelloWindow::PopulateCommandList()
{
	ThrowIfFailed(m_CommandAllocator->Reset());

	ThrowIfFailed(m_CommandList->Reset(m_CommandAllocator.Get(), m_PipelineState.Get()));

	// Set necessary state.
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());

	ID3D12DescriptorHeap* heaps[] = {m_SRVCBVDescriptorHeap.Get()};
	m_CommandList->SetDescriptorHeaps(_countof(heaps), heaps);


	CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(m_SRVCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 0, 0);
	CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(m_SRVCBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1, m_SRVCBVDescriptorSize);
	m_CommandList->SetGraphicsRootDescriptorTable(0, srvHandle);
	m_CommandList->SetGraphicsRootDescriptorTable(1, cbvHandle);

	m_CommandList->RSSetViewports(1, &m_Viewport);
	m_CommandList->RSSetScissorRects(1, &m_ScissorRect);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Set render target;
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RTVDescriptorSize);
	m_CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	constexpr float clearColor[] = {0.f, 0.2f, 0.4f, 1.f};
	m_CommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_CommandList->IASetVertexBuffers(0, 1, &m_VertexBufferView);
	m_CommandList->IASetIndexBuffer(&m_IndexBufferView);
	m_CommandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	ThrowIfFailed(m_CommandList->Close());
}

void D3D12HelloWindow::WaitForPreviousFrame()
{
	// Signal and increment the fence value.
	const UINT64 fence = m_FenceValue;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fence));
	m_FenceValue++;

	// Wait until the previous frame is finished.
	if (m_Fence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(m_Fence->SetEventOnCompletion(fence, m_FenceEvent));
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}
