#include "helper.h"
#include "external/imgui/imgui.h"

extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11RenderTargetView* g_mainRenderTargetView;

static ID3D11ShaderResourceView* texSRV = nullptr;
static ID3D11Texture2D* tex             = nullptr;

static int tex_width;
static int tex_height;

void InitializeDisplayTexture(int width, int height, bool override)
{
    if (!override && tex != nullptr)
    {
        return;
    }
    if (texSRV != nullptr)
    {
        texSRV->Release();
        texSRV = nullptr;
    }
    if (tex != nullptr)
    {
        tex->Release();
        tex = nullptr;
    }

	tex_width = width;
	tex_height = height;

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width                = width;
    desc.Height               = height;
    desc.MipLevels            = 1;
    desc.ArraySize            = 1;
    desc.Format               = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count     = 1;
    desc.Usage                = D3D11_USAGE_DYNAMIC;
    desc.BindFlags            = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags       = D3D10_CPU_ACCESS_WRITE;

    HRESULT hr = g_pd3dDevice->CreateTexture2D(&desc, nullptr, &tex);

    if (SUCCEEDED(hr))
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
        SRVDesc.Format                          = DXGI_FORMAT_R8G8B8A8_UNORM;
        SRVDesc.ViewDimension                   = D3D11_SRV_DIMENSION_TEXTURE2D;
        SRVDesc.Texture2D.MipLevels             = 1;

        hr = g_pd3dDevice->CreateShaderResourceView(tex, &SRVDesc, &texSRV);
        if (SUCCEEDED(hr))
        {
            return;
        }
    }

    throw 0;
}

void UpdateDisplayTexture(const std::function<akane::Point3i(int x, int y)>& pixel_source)
{
    D3D11_MAPPED_SUBRESOURCE mapped_resource;
    ZeroMemory(&mapped_resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
    HRESULT hr = g_pd3dDeviceContext->Map(tex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);

    if (SUCCEEDED(hr))
    {
        auto data = reinterpret_cast<uint32_t*>(mapped_resource.pData);

        for (int y = 0; y < tex_height; ++y)
        {
            for (int x = 0; x < tex_width; ++x)
            {
                auto rgb       = pixel_source(x, y);
                uint32_t rr    = rgb[0];
                uint32_t gg    = rgb[1];
                uint32_t bb    = rgb[2];
                uint32_t pixel = (0xffu << 24) | (bb << 16) | (gg << 8) | rr;

                data[y * mapped_resource.RowPitch / 4 + x] = pixel;
            }
        }
        g_pd3dDeviceContext->Unmap(tex, 0);
    }
}

ImTextureID RetriveDisplayTexture()
{
	if (tex != nullptr && texSRV != nullptr)
	{
		return texSRV;
	}
	else
	{
		return nullptr;
	}
}