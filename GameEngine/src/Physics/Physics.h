#pragma once
#include <reactphysics3d/reactphysics3d.h>

class Physics
{
public:
	static rp3d::PhysicsCommon& Get()
	{
		static rp3d::PhysicsCommon common;
		return common;
	}

	static rp3d::PhysicsWorld* CreatePhysicsWorld()
	{
		rp3d::PhysicsWorld::WorldSettings settings;
		//settings.defaultVelocitySolverNbIterations = 25;
		settings.gravity = rp3d::Vector3(0.f, -9.81f, 0.f);
		return Get().createPhysicsWorld(settings);
	}
};
