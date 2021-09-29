#include "stdafx.h"
#include "FrameBuffer.h"

#include "Renderer.h"

uint8_t FrameBuffer::active_numBuffers = 0;

FrameBuffer::FrameBuffer(uint8_t numBuffers, uint32_t client_width, uint32_t client_height):
	FrameBuffer(client_width, client_height)
{
	m_buffers.reserve(numBuffers);
	m_rtv.reserve(numBuffers);
}

FrameBuffer::FrameBuffer(uint32_t client_width, uint32_t client_height):
	m_client_width(client_width),
	m_client_height(client_height),
	m_buffers()
{
	m_viewport.Width = (float)m_client_width;
	m_viewport.Height = (float)m_client_height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
}


void FrameBuffer::AddBuffer(DXGI_FORMAT format, D3D11_USAGE usage, uint32_t bindFlags, float clearColor[4])
{
	Buffer buffer;
	ZeroMemory(&(buffer.bufferDesc), sizeof(buffer.bufferDesc));

	//Texture 2D Description
	buffer.bufferDesc.Width = m_client_width;
	buffer.bufferDesc.Height = m_client_height;
	buffer.bufferDesc.MipLevels = 1;
	buffer.bufferDesc.ArraySize = 1;
	buffer.bufferDesc.Format = format;
	buffer.bufferDesc.SampleDesc.Count = 1;
	buffer.bufferDesc.Usage = usage;
	buffer.bufferDesc.BindFlags = bindFlags;
	buffer.bufferDesc.CPUAccessFlags = 0;
	buffer.bufferDesc.MiscFlags = 0;
	buffer.bufferDesc.SampleDesc.Count = 1;
	buffer.bufferDesc.SampleDesc.Quality = 0;

	//Render Target View Description
	buffer.rtvDesc.Format = buffer.bufferDesc.Format;
	buffer.rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	buffer.rtvDesc.Texture2D.MipSlice = 0;

	//Clear Color
	if (clearColor)
		for (int i = 0; i < 4; ++i)
			buffer.clearColor[i] = clearColor[i];
	
	m_buffers.push_back(buffer);
	m_rtv.push_back(nullptr);
	++numBuffers;
}

void FrameBuffer::AddSwapChainBuffer(const float clearColor[4])
{
	Buffer buffer;

	//Clear Color
	if (clearColor)
		for (int i = 0; i < 4; ++i)
			buffer.clearColor[i] = clearColor[i];
	
	m_buffers.push_back(buffer);
	m_rtv.push_back(nullptr);
	++numBuffers;
}

void FrameBuffer::CreateRenderTargetViews()
{
	for (auto i = 0 ; i < m_buffers.size(); ++i)
	{
		HRESULT hr;
		
		hr = Renderer::GetDevice()->CreateTexture2D(&m_buffers[i].bufferDesc, nullptr, m_buffers[i].buffer.GetAddressOf());
		assert(SUCCEEDED(hr));

		hr = Renderer::GetDevice()->CreateRenderTargetView(m_buffers[i].buffer.Get(), &m_buffers[i].rtvDesc, m_rtv[i].GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void FrameBuffer::CreateSwapChainRTV(ID3D11Texture2D* buffer)
{
	HRESULT hr = Renderer::GetDevice()->CreateRenderTargetView(buffer, nullptr, m_rtv[0].GetAddressOf());
	buffer->Release();
	assert(SUCCEEDED(hr));
}

void FrameBuffer::CreateDepthStencilView()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil = nullptr;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = static_cast<uint32_t>(m_client_width);
	descDepth.Height = static_cast<uint32_t>(m_client_height);
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	HRESULT hr = Renderer::GetDevice()->CreateTexture2D(&descDepth, nullptr, depthStencil.GetAddressOf());
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DSState;
	Renderer::GetDevice()->CreateDepthStencilState(&dsDesc, &DSState);

	// Bind depth stencil state
	Renderer::GetContext()->OMSetDepthStencilState(DSState.Get(), 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;

	// Create the depth stencil view
	hr = Renderer::GetDevice()->CreateDepthStencilView(depthStencil.Get(), &descDSV, m_dsv.GetAddressOf());
	assert(SUCCEEDED(hr));
}

void FrameBuffer::Release()
{
	for (auto i = 0; i < m_buffers.size(); ++i)
	{
		if (m_buffers[i].buffer) m_buffers[i].buffer->Release();
		m_rtv[i]->Release();
	}
	m_dsv->Release();
}

void FrameBuffer::Resize(uint32_t client_width, uint32_t client_height)
{
	m_client_width = client_width;
	m_client_height = client_height;

	m_viewport.Width = (float)m_client_width;
	m_viewport.Height = (float)m_client_height;
	
	for (auto& buffer : m_buffers)
	{
		buffer.bufferDesc.Width = m_client_width;
		buffer.bufferDesc.Height = m_client_height;
	}
}

void FrameBuffer::Begin()
{
	old.numBuffers = active_numBuffers;
	Renderer::GetContext()->OMGetRenderTargets(active_numBuffers, &old.rtv, &old.dsv);
	Renderer::GetContext()->RSGetViewports(&old.viewportCount, &old.viewport);

	active_numBuffers = numBuffers;
	Renderer::GetContext()->OMSetRenderTargets(numBuffers, m_rtv[0].GetAddressOf(), m_dsv.Get());
	Renderer::GetContext()->RSSetViewports(1, &m_viewport);
	for (auto i = 0; i < m_buffers.size(); ++i)
		Renderer::GetContext()->ClearRenderTargetView(m_rtv[i].Get(), m_buffers[i].clearColor);
	Renderer::GetContext()->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void FrameBuffer::End()
{
	Renderer::GetContext()->OMSetRenderTargets(old.numBuffers, &old.rtv, old.dsv);
	Renderer::GetContext()->RSSetViewports(old.viewportCount, &old.viewport);
	active_numBuffers = old.numBuffers;
	
	if (old.rtv) old.rtv->Release();
	if (old.dsv) old.dsv->Release();
}

void FrameBuffer::ReadPixel(const uint8_t buffer_index, const int x, const int y, DXGI_FORMAT format, void* data)
{
	//Initialize 1x1 Texture
	static D3D11_TEXTURE2D_DESC pixelTextureDesc = {
		1,//UINT Width;
		1,//UINT Height;
		1,//UINT MipLevels;
		1,//UINT ArraySize;
		format,//DXGI_rotationQ.m128_f32ORMAT rotationQ.m128_f32ormat;
		1, 0,//DXGI_SAMPLE_DESC SampleDesc;
		D3D11_USAGE_STAGING,//D3D11_USAGE Usage;
		0,//UINT BindrotationQ.m128_f32lags;
		D3D11_CPU_ACCESS_READ,//UINT CPUAccessrotationQ.m128_f32lags;
		0//UINT MiscrotationQ.m128_f32lags;
	};
	static Microsoft::WRL::ComPtr<ID3D11Texture2D> pixel_data = nullptr;

	if (!pixel_data) Renderer::GetDevice()->CreateTexture2D(&pixelTextureDesc, NULL, pixel_data.GetAddressOf());
	
	if (pixelTextureDesc.Format != format ) {
		pixelTextureDesc.Format = format;
		pixel_data->Release();
		Renderer::GetDevice()->CreateTexture2D(&pixelTextureDesc, NULL, pixel_data.GetAddressOf());
	}
	//------End-------

	//Check if format is the same with buffer
	if (m_buffers[buffer_index].bufferDesc.Format != format) {
		data = NULL;
		return;
	}
	
	D3D11_BOX SrcBox;
	SrcBox.left = x; // Minimum X
	SrcBox.right = x + 1; // Maximum X
	SrcBox.top = y; // Minimum Y
	SrcBox.bottom = y + 1; // Maximum Y
	SrcBox.front = 0; // Always 0 for 2D textures
	SrcBox.back = 1; // Always 1 for 2D textures

	//copy data
	Renderer::GetContext()->CopySubresourceRegion(pixel_data.Get(), 0, 0, 0, 0, m_buffers[buffer_index].buffer.Get(), 0, &SrcBox);
	D3D11_MAPPED_SUBRESOURCE MapResource;
	HRESULT hr = Renderer::GetContext()->Map(pixel_data.Get(), 0, D3D11_MAP_READ, 0, &MapResource);
	assert(SUCCEEDED(hr));

	// Get the data
	switch (format) {
	case DXGI_FORMAT_R32_UINT: *(uint32_t*)data = *(uint32_t*)MapResource.pData; break;
	}

	// Unlock the memory
	Renderer::GetContext()->Unmap(pixel_data.Get(), 0);
}
