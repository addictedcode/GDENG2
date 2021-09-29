#include "stdafx.h"
#include "Component.h"

#include "Entity.h"

BoxColliderComponent::BoxColliderComponent(Entity* owner)
{
	entity = owner;
	if (entity->HasComponent<TransformComponent>()) {
		auto& tc = entity->GetComponent<TransformComponent>();
		shape = Physics::Get().createBoxShape(rp3d::Vector3(tc.Scale.m128_f32[0] * size.x / 2.f, tc.Scale.m128_f32[1] * size.y / 2.f, tc.Scale.m128_f32[2] * size.z / 2.f));
	}
	else
	{
		shape = Physics::Get().createBoxShape(size / 2.f);
	}
	if (entity->HasComponent<RigidbodyComponent>()) {
		auto& rigidBodyComponent = entity->GetComponent<RigidbodyComponent>();
		collider = rigidBodyComponent.body->addCollider(shape, rp3d::Transform(position, rp3d::Quaternion::identity()));
		rigidbody = rigidBodyComponent.body;
	}
}

BoxColliderComponent::BoxColliderComponent(Entity* owner, rp3d::Vector3 pos, rp3d::Vector3 new_size)
{
	if (pos.x <= 0)
		pos.x = 0.01f;
	if (pos.y <= 0)
		pos.y = 0.01f;
	if (pos.z <= 0)
		pos.z = 0.0f;
	position = pos;
	size = new_size;
	entity = owner;
	if (entity->HasComponent<TransformComponent>()) {
		auto& tc = entity->GetComponent<TransformComponent>();
		shape = Physics::Get().createBoxShape(rp3d::Vector3(tc.Scale.m128_f32[0] * size.x / 2.f, tc.Scale.m128_f32[1] * size.y / 2.f, tc.Scale.m128_f32[2] * size.z / 2.f));
	}
	else
	{
		shape = Physics::Get().createBoxShape(size / 2.f);
	}
	if (entity->HasComponent<RigidbodyComponent>()) {
		auto& rigidBodyComponent = entity->GetComponent<RigidbodyComponent>();
		collider = rigidBodyComponent.body->addCollider(shape, rp3d::Transform(position, rp3d::Quaternion::identity()));
		rigidbody = rigidBodyComponent.body;
	}
}

BoxColliderComponent::~BoxColliderComponent()
{
	if (!rigidbody.expired())
		rigidbody.lock()->removeCollider(collider);
	Physics::Get().destroyBoxShape(shape);
}

RigidbodyComponent::RigidbodyComponent(Entity* owner, std::shared_ptr<rp3d::PhysicsWorld>& new_world):
	world(new_world)
{
	entity = owner;
	auto& transformComponent = entity->GetComponent<TransformComponent>();
	rp3d::Vector3 position(transformComponent.Translation.m128_f32[0], transformComponent.Translation.m128_f32[1], transformComponent.Translation.m128_f32[2]);
	auto normalized_rotation = DirectX::XMQuaternionNormalize(transformComponent.Rotation);
	rp3d::Quaternion rotation(normalized_rotation.m128_f32[0],
							normalized_rotation.m128_f32[1],
							normalized_rotation.m128_f32[2],
							normalized_rotation.m128_f32[3]);
	rp3d::Transform transform(position, rotation);
	body.reset(world->createRigidBody(transform), [](rp3d::RigidBody* p){});
	body->setType(rp3d::BodyType::STATIC);
	body->enableGravity(false);
	body->setIsAllowedToSleep(false);

	if (entity->HasComponent<BoxColliderComponent>())
	{
		auto& boxColliderComponent = entity->GetComponent<BoxColliderComponent>();
		boxColliderComponent.collider = body->addCollider(boxColliderComponent.shape, rp3d::Transform(boxColliderComponent.position, rp3d::Quaternion::identity()));
		boxColliderComponent.rigidbody = body;
	}
}
