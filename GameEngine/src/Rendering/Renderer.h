#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class Shader;
class VertexBuffer;
class IndexBuffer;
class ConstantBuffer;
class Mesh;
class Entity;
class Camera;
class RenderAPI;
class Window;

class Renderer
{
public:
	static void Initialize(const Window& window);

	static void BeginScene(const std::shared_ptr<Camera>& camera, const DirectX::XMFLOAT4& light_direction, float m_angle);
	static void EndScene();
	
	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertex_buffer, const std::shared_ptr<IndexBuffer>& index_buffer);
	static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh);
	static void Submit(const std::shared_ptr<Entity>& entity);

	static void Clear();
	static void SwapBuffer();

	static void OnWindowResize(const uint32_t& width, const uint32_t& height);

	static const Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice();
	static const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext();
	static const std::unique_ptr<RenderAPI>& GetRenderAPI() { return m_renderAPI; }
private:
	static std::unique_ptr<RenderAPI> m_renderAPI;
	
	__declspec(align(16))
	struct SceneData
	{
		float m_angle;
		DirectX::XMMATRIX m_viewprojection;
		DirectX::XMFLOAT4 m_light_direction;
	};

	static SceneData m_scene_data;
};