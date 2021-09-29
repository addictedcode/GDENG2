#pragma once

class Layer
{
public:
	Layer() = default;
	virtual ~Layer() = default;
	Layer operator=(const Layer & other) = delete;
	Layer(const Layer & other) = delete;
	Layer operator=(Layer && other) = delete;
	Layer(Layer && other) = delete;

	virtual void OnAttach() {}
	virtual void OnDetach() {}
	virtual void OnUpdate(float delta_time) {}
	virtual void OnFixedUpdate(float timestep) {}
	virtual void OnImGuiRender() {}

};