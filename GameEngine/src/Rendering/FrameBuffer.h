#pragma once
#include <d3d11.h>
#include <wrl/client.h>


class FrameBuffer
{
public:
	FrameBuffer(uint8_t numBuffers, uint32_t client_width, uint32_t client_height);
	FrameBuffer(uint32_t client_width, uint32_t client_height);
	~FrameBuffer() = default;
	FrameBuffer operator=(const FrameBuffer & other) = delete;
	FrameBuffer(const FrameBuffer & other) = delete;
	FrameBuffer operator=(FrameBuffer && other) = delete;
	FrameBuffer(FrameBuffer && other) = delete;

private:
	static uint8_t active_numBuffers;

	uint8_t numBuffers = 0;
	uint32_t m_client_width, m_client_height;

	struct Buffer
	{
		D3D11_TEXTURE2D_DESC bufferDesc;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer;
		
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		float clearColor[4] = {0.f, 0.f, 0.f, 1.f};
	};

	std::vector<Buffer> m_buffers;

	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_rtv;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
	D3D11_VIEWPORT m_viewport = {};

	struct DX11_State
	{
		uint8_t numBuffers = 0;
		ID3D11RenderTargetView* rtv = nullptr;
		ID3D11DepthStencilView* dsv = nullptr;
		UINT viewportCount;
		D3D11_VIEWPORT viewport;
	};

	DX11_State old = {};
	
public:
	void AddBuffer(DXGI_FORMAT format, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, uint32_t bindFlags = D3D11_BIND_RENDER_TARGET, float clearColor[4] = nullptr);
	void AddSwapChainBuffer(const float clearColor[4] = nullptr);
	void CreateRenderTargetViews();
	void CreateSwapChainRTV(ID3D11Texture2D* buffer);
	void CreateDepthStencilView();
	void Release();

	void Resize(uint32_t client_width, uint32_t client_height);
	
	void Begin();
	void End();

	void ReadPixel(const uint8_t buffer_index, const int x, const int y, DXGI_FORMAT format, void* data);
	ID3D11Texture2D* GetBuffer(const uint8_t index) { return m_buffers[index].buffer.Get(); }
};
