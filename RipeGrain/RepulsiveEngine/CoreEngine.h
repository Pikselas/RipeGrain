#pragma once

#include<D3DCompiler.h>
#pragma comment(lib,"D3DCompiler.lib")

#include"CustomWindow.h"
#include"StencilBuffer.h"
#include"MemoryRenderer.h"
#include"WindowRenderer.h"
#include"AnimatedSprite.h"

#include "RenderCommandEngine.h"

class CoreEngine : public RenderCommandEngine
{
private:
	template<typename ObjectT>
	using ObjectManager = Microsoft::WRL::ComPtr<ObjectT>;
private:
	struct VertexType
	{
		float x, y;
	};
private:
	ObjectManager<ID3D11Device>				graphics_device;
private:
	ObjectManager<ID3D11InputLayout>		input_layout;
private:
	ObjectManager<ID3D11VertexShader>		vertex_shader;
	ObjectManager<ID3D11PixelShader>		pixel_shader;
private:
	ObjectManager<ID3D11Buffer>				vertex_shader_transform_buffer;
	ObjectManager<ID3D11Buffer>				vertex_shader_texture_coord_buffer;
	ObjectManager<ID3D11Buffer>				index_buffer;
public:
	CoreEngine();
private:
	ObjectManager<ID3D11SamplerState>		SAMPLER_STATE;
	ObjectManager<ID3D11RasterizerState>	raster_state;
private:
	ObjectManager<ID3D11DepthStencilState> STENCIL_PASS_STATE;
	ObjectManager<ID3D11DepthStencilState> STENCIL_CLIP_STATE;
public:
	void SetComponent(const DirectX::XMMATRIX transformation) override;
	void SetComponent(ID3D11ShaderResourceView* texture_view, std::pair<float, float> coord, std::pair<float, float> size) override;
	void SetComponent(ID3D11Buffer* vertices) override;
public:
	void SetStencilBuffer(StencilBuffer& buffer);
	void ClearStencilBuffer(StencilBuffer& buffer);
public:
	void RemoveStencilBuffer();
	void EndStencilClipping(unsigned int ref_value);
	void BeginStencilClipping(unsigned int ref_value);
public:
	StencilBuffer CreateStencilBuffer(unsigned int width, unsigned int height);
public:
	Texture CreateTexture(const Image& image);
public:
	ImageSprite CreateSprite(const Image& image);
	ImageSprite CreateSprite(Texture texture, unsigned int width , unsigned int height);
	AnimatedSprite CreateSprite(const std::vector<Image>& frames , std::chrono::milliseconds duration , std::optional<unsigned int> repeat_count = std::nullopt);
public:
	MemoryRenderer CreateRenderer(Image& image);
	WindowRenderer CreateRenderer(CustomWindow& window);
public:
	void Draw() override;
};