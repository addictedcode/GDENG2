#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class IndexBuffer
{
public:
	IndexBuffer(const void* data, const uint32_t& count);
	~IndexBuffer() = default;
	IndexBuffer operator=(const IndexBuffer & other) = delete;
	IndexBuffer(const IndexBuffer & other) = delete;
	IndexBuffer operator=(IndexBuffer && other) = delete;
	IndexBuffer(IndexBuffer && other) = delete;

private:
	uint32_t m_count;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
public:
	void Bind() const;
	
	const uint32_t& GetCount() const { return m_count; }
};
