#include "stdafx.h"
#include "Texture.h"

#include "Rendering/Renderer.h"

#include "stb_image/stb_image.h"

Texture::Texture(const char* filename)
{
    // Load from disk into a raw RGBA buffer
    int width, height, channel;
    unsigned char* image_data = stbi_load(filename, &width, &height, &channel, 4);
    if (image_data == NULL)
        assert(false);

    m_width = width;
    m_height = height;
	
    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = m_width;
    desc.Height = m_height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    HRESULT hr;
	
    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    hr =Renderer::GetDevice()->CreateTexture2D(&desc, &subResource, &pTexture);
    assert(SUCCEEDED(hr));
	
    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    hr = Renderer::GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, m_srv.GetAddressOf());
    assert(SUCCEEDED(hr));
    pTexture->Release();

    stbi_image_free(image_data);

    CreateSamplerState();
}

void Texture::CreateSamplerState()
{
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = Renderer::GetDevice()->CreateSamplerState(&sampDesc, m_ss.GetAddressOf());
    assert(SUCCEEDED(hr));
}

void Texture::Bind()
{
    Renderer::GetContext()->PSSetShaderResources(0u, 1u, m_srv.GetAddressOf());
    Renderer::GetContext()->PSSetSamplers(0u, 1u, m_ss.GetAddressOf());
}
