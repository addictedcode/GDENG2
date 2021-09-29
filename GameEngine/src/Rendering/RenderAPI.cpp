#include "stdafx.h"
#include "RenderAPI.h"

#include "Window.h"

void RenderAPI::Init(const Window& window)
{
	#pragma region Device
	D3D_DRIVER_TYPE driver_types[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	const uint32_t num_driver_types = ARRAYSIZE(driver_types);

	D3D_FEATURE_LEVEL feature_levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};
	const uint32_t num_feature_levels = ARRAYSIZE(feature_levels);

	HRESULT res = 0;
	for (UINT driver_type_index = 0; driver_type_index < num_driver_types;)
	{
		res = D3D11CreateDevice(
			NULL, driver_types[driver_type_index],
			NULL, NULL,
			feature_levels, num_feature_levels,
			D3D11_SDK_VERSION,
			&m_d3d_device, &m_feature_level, &m_device_context);

		if (SUCCEEDED(res))
			break;

		++driver_type_index;
	}
	
	assert(SUCCEEDED(res));

	m_d3d_device->QueryInterface(__uuidof(IDXGIDevice), (void**)&m_dxgi_device);
	m_dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&m_dxgi_adapter);
	m_dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&m_dxgi_factory);
	#pragma endregion 
	
	#pragma region SwapChain
	DXGI_SWAP_CHAIN_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BufferCount = 1;
	desc.BufferDesc.Width = window.m_client_size.width;
	desc.BufferDesc.Height = window.m_client_size.height;
	desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.RefreshRate.Numerator = 60;
	desc.BufferDesc.RefreshRate.Denominator = 1;
	desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow = window.GetHWND();
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Windowed = TRUE;
	desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//Create the swap chain for the window indicated by HWND parameter
	HRESULT hr = m_dxgi_factory->CreateSwapChain(m_d3d_device.Get(), &desc, &m_swap_chain);

	assert(SUCCEEDED(hr));

	#pragma endregion

	//Frame Buffer (Render Target View, Depth Stencil View, Viewport)
	m_frame_buffer = std::make_unique<FrameBuffer>(1, window.m_client_size.width, window.m_client_size.height);
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_frame_buffer->AddSwapChainBuffer(color);
	
	ID3D11Texture2D* buffer;
	m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&buffer));
	m_frame_buffer->CreateSwapChainRTV(buffer);

	m_frame_buffer->CreateDepthStencilView();
}

void RenderAPI::Clear() const
{
	m_frame_buffer->Begin();
}

void RenderAPI::Draw(const uint32_t& count, const uint32_t& offset, const D3D_PRIMITIVE_TOPOLOGY& topology) const
{
	m_device_context->IASetPrimitiveTopology(topology);
	m_device_context->DrawIndexed(count, offset, 0);
}

void RenderAPI::SwapBuffer() const
{
	m_swap_chain->Present(true, NULL);
	m_frame_buffer->End();
}

void RenderAPI::OnWindowResize(const uint32_t& width, const uint32_t& height)
{
	if (!m_swap_chain) return;

	m_frame_buffer->Release();

	HRESULT hr = m_swap_chain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	assert(!FAILED(hr));

	m_frame_buffer->Resize(width, height);
	
	ID3D11Texture2D* buffer;
	m_swap_chain->GetBuffer(0, IID_PPV_ARGS(&buffer));
	m_frame_buffer->CreateSwapChainRTV(buffer);

	m_frame_buffer->CreateDepthStencilView();
}
