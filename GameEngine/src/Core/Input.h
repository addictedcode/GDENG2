#pragma once
#include <DirectXMath.h>

class Input
{
public:
	static bool IsKeyPressed(const uint16_t& key);
	static bool IsKeyDown(const uint16_t& key);

	static DirectX::XMFLOAT2 GetMousePosition();
};
