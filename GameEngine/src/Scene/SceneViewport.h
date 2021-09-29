#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include "Entity.h"
#include "Scene.h"
#include "Rendering/Camera.h"
#include "Rendering/FrameBuffer.h"

class SceneViewport
{
public:
	SceneViewport();
	~SceneViewport() = default;
	SceneViewport operator=(const SceneViewport & other) = delete;
	SceneViewport(const SceneViewport & other) = delete;
	SceneViewport operator=(SceneViewport && other) = delete;
	SceneViewport(SceneViewport && other) = delete;

public:
	std::shared_ptr<Camera> m_Active_Camera;
	std::shared_ptr<Scene> m_Active_Scene;
	std::shared_ptr<std::weak_ptr<Entity>> m_Selected_Entity;

	bool m_camera_need_update = false;
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shader_resource_view;

	std::unique_ptr<FrameBuffer> m_frame_buffer;
	
public:
	void Init(const uint32_t width, const uint32_t height);

	void BeginScene();
	void EndScene();

	void OnImGuiRender();
private:
	void CreateShaderResourceView();

	void OnWindowResize(const uint32_t width, const uint32_t height);
};
