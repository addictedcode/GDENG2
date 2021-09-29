#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class ConstantBuffer
{
public:
	ConstantBuffer(const uint32_t& size, const uint32_t& binding);
	~ConstantBuffer() = default;
	ConstantBuffer operator=(const ConstantBuffer & other) = delete;
	ConstantBuffer(const ConstantBuffer & other) = delete;
	ConstantBuffer operator=(ConstantBuffer && other) = delete;
	ConstantBuffer(ConstantBuffer && other) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	uint32_t m_binding;

public:
	void Bind() const;
	void SetData(const void* data) const;
};
