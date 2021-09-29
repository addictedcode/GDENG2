#pragma once
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	~Camera() = default;
	Camera operator=(const Camera & other) = delete;
	Camera(const Camera & other) = delete;
	Camera operator=(Camera && other) = delete;
	Camera(Camera && other) = delete;

public:
	DirectX::XMVECTOR m_position;
	DirectX::XMVECTOR m_rotation;
	
	DirectX::XMMATRIX m_view;
	DirectX::XMMATRIX* m_projection;

	bool m_is_perspective;
private:
	DirectX::XMMATRIX m_perspective_proj;
	DirectX::XMMATRIX m_orthographic_proj;

	//TEMP

	float m_ratio;
	
	struct perspectiveproj
	{
		float FOV;
		float nearZ;
		float farZ;
	};

	perspectiveproj persdata;

	struct orthoproj
	{
		float view_scale;
		float nearZ;
		float farZ;
	};

	orthoproj orthodata;
public:
	void OnUpdate();
	void OnImGuiRender();
	void UpdateRatio(float ratio);

	DirectX::XMMATRIX GetView() { return m_view = DirectX::XMMatrixInverse(nullptr, DirectX::XMMatrixRotationRollPitchYawFromVector(m_rotation) * DirectX::XMMatrixTranslationFromVector(m_position)); }
	DirectX::XMMATRIX GetViewProjection() { return GetView() * *m_projection; }
};