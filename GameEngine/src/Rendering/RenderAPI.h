#pragma once
#include <d3d11.h>
#include <wrl/client.h>

#include "FrameBuffer.h"

class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Shader;
class Window;

class RenderAPI
{
public:
	RenderAPI() = default;
	~RenderAPI() = default;
	RenderAPI operator=(const RenderAPI & other) = delete;
	RenderAPI(const RenderAPI & other) = delete;
	RenderAPI operator=(RenderAPI && other) = delete;
	RenderAPI(RenderAPI && other) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_device_context;
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3d_device;

	D3D_FEATURE_LEVEL m_feature_level;

	Microsoft::WRL::ComPtr<IDXGIDevice> m_dxgi_device;
	Microsoft::WRL::ComPtr<IDXGIAdapter> m_dxgi_adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory> m_dxgi_factory;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swap_chain;

	std::unique_ptr<FrameBuffer> m_frame_buffer;

public:
	void Init(const Window& window);

	void Clear() const;
	void Draw(const uint32_t& count, const uint32_t& offset, const D3D_PRIMITIVE_TOPOLOGY& topology) const;
	
	void SwapBuffer() const;

	void OnWindowResize(const uint32_t& width, const uint32_t& height);
	
public:
	const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() const { return m_d3d_device; }
	const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext() const { return m_device_context; }
	const Microsoft::WRL::ComPtr<IDXGIFactory>& GetFactory() const { return m_dxgi_factory; }
};
