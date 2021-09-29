#include "stdafx.h"
#include "Scene.h"

#include "Physics/Physics.h"

#include "Rendering/Renderer.h"

Scene::Scene() :
	m_Scene_Camera(std::make_shared<Camera>()),
	m_physics_world(Physics::CreatePhysicsWorld(), [](rp3d::PhysicsWorld* p) { Physics::Get().destroyPhysicsWorld(p); })
{	
}

std::shared_ptr<Entity> Scene::CreateEntity()
{
	auto entity = std::make_shared<Entity>();
	entity->m_tag = "Empty Entity";
	m_Registry.emplace(entity);
	return entity;
}

std::shared_ptr<Entity> Scene::CreateEntity(const uint32_t& id, const std::string& tag)
{
	auto entity = std::make_shared<Entity>(id, tag);
	m_Registry.emplace(entity);
	return entity;
}

void Scene::AddEntity(Entity&& entity)
{
	auto _entity = std::make_shared<Entity>(std::move(entity));
	m_Registry.emplace(std::move(_entity));
}

void Scene::DestroyEntity(const std::shared_ptr<Entity>& entity)
{
	m_Registry.erase(entity);
}

void Scene::OnUpdate(float delta_time)
{
	//Update Transform of Entities with Rigidbodies
	for (const auto& entity : m_Registry)
	{
		if (entity->HasComponent<RigidbodyComponent>() && entity->HasComponent<TransformComponent>())
		{
			auto& rigidbody = entity->GetComponent<RigidbodyComponent>();
			auto& transformComponent = entity->GetComponent<TransformComponent>();
			const auto& transform = rigidbody.body->getTransform();
			auto& position = transform.getPosition();
			auto& rotation = transform.getOrientation();
			transformComponent.Translation.m128_f32[0] = position.x;
			transformComponent.Translation.m128_f32[1] = position.y;
			transformComponent.Translation.m128_f32[2] = position.z;
			transformComponent.Rotation.m128_f32[0] = rotation.x;
			transformComponent.Rotation.m128_f32[1] = rotation.y;
			transformComponent.Rotation.m128_f32[2] = rotation.z;
			transformComponent.Rotation.m128_f32[3] = rotation.w;
		}
	}
	
	static float m_angle = 0.0f;
	m_angle += 3.14f / 2.0f * delta_time;

	DirectX::XMMATRIX lightrot = DirectX::XMMatrixRotationX(3.14f / 2.0f);
	
	Renderer::BeginScene(m_Scene_Camera,
		{ lightrot.r[0].m128_f32[2], lightrot.r[1].m128_f32[2], lightrot.r[2].m128_f32[2], lightrot.r[3].m128_f32[2] },
		m_angle);

	for (const auto& entity : m_Registry)
	{
		Renderer::Submit(entity);
	}

	Renderer::EndScene();
}

void Scene::OnFixedUpdate(float timestep)
{
	//Update Transform of Rigidbodies of entities with rigid bodies
	for (const auto& entity : m_Registry)
	{
		if (entity->HasComponent<RigidbodyComponent>() && entity->HasComponent<TransformComponent>())
		{
			auto& rigidbody = entity->GetComponent<RigidbodyComponent>();
			auto& transformComponent = entity->GetComponent<TransformComponent>();
			rp3d::Vector3 position(transformComponent.Translation.m128_f32[0], transformComponent.Translation.m128_f32[1], transformComponent.Translation.m128_f32[2]);
			auto normalized_rotation = DirectX::XMQuaternionNormalize(transformComponent.Rotation);
			rp3d::Quaternion rotation(normalized_rotation.m128_f32[0],
									normalized_rotation.m128_f32[1],
									normalized_rotation.m128_f32[2],
									normalized_rotation.m128_f32[3]);
			rigidbody.body->setTransform(rp3d::Transform(position, rotation));
		}
	}

	if (timestep > 0.f)
		m_physics_world->update(timestep);
}
