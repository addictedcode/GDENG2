#pragma once
#include <DirectXMath.h>
#include <Mesh/Mesh.h>
#include <Rendering/Texture.h>
#include <Rendering/Shader.h>
#include "Physics/Physics.h"

class Entity;

//Abstract Component class
struct Component
{
	Component() = default;
	virtual ~Component() = 0;
	
	Entity& GetEntity() { return *entity; }
protected:
	Entity* entity = nullptr;
	friend class Entity;
};
inline Component::~Component() {}

struct TransformComponent: Component
{
	DirectX::XMVECTOR Translation = { 0.f, 0.f, 0.f, 1.f };
	DirectX::XMVECTOR Rotation = { 0.f, 0.f, 0.f, 1.f };
	DirectX::XMVECTOR Scale = { 1.f, 1.f, 1.f, 1.f };

	DirectX::XMMATRIX GetTransform() const {
		return
			DirectX::XMMatrixScalingFromVector(Scale) *
			DirectX::XMMatrixRotationQuaternion(Rotation) *
			DirectX::XMMatrixTranslationFromVector(Translation);
	};

	TransformComponent() = default;
};

struct RendererComponent: Component
{
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Texture> texture;
	std::shared_ptr<Mesh> mesh;

	RendererComponent() = default;
};

struct BoxColliderComponent: Component
{
	rp3d::Vector3 position = rp3d::Vector3(0.f, 0.f, 0.f);
	rp3d::Vector3 size = rp3d::Vector3(1.f, 1.f, 1.f);
	rp3d::BoxShape* shape;
	rp3d::Collider* collider;
	std::weak_ptr<rp3d::RigidBody> rigidbody;

	BoxColliderComponent(Entity* owner);
	BoxColliderComponent(Entity* owner, rp3d::Vector3 pos, rp3d::Vector3 new_size);

	~BoxColliderComponent();
};

struct RigidbodyComponent: Component
{
 	std::shared_ptr<rp3d::RigidBody> body;
	std::shared_ptr<rp3d::PhysicsWorld> world;

	RigidbodyComponent(Entity* owner, std::shared_ptr<rp3d::PhysicsWorld>& new_world);

	~RigidbodyComponent()
	{
		world->destroyRigidBody(body.get());
	}
};