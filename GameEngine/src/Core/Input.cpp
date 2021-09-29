#include "stdafx.h"
#include "Input.h"

#include <Windows.h>
bool Input::IsKeyPressed(const uint16_t& key)
{
	bool is_Key_Pressed = false;
	short key_state = ::GetKeyState(key);

	if (key_state & 0x8000) {
		is_Key_Pressed = true;
	}
	
	return is_Key_Pressed;
}

bool Input::IsKeyDown(const uint16_t& key)
{
	bool is_Key_Pressed = false;
	static std::unordered_map<uint16_t, short> prev_state;
	short key_state = ::GetKeyState(key);

	if (!(prev_state[key] & 0x8000) && (key_state & 0x8000)) {
		is_Key_Pressed = true;
	}

	prev_state[key] = key_state;
	return is_Key_Pressed;
}

DirectX::XMFLOAT2 Input::GetMousePosition()
{
	POINT pos;
	::GetCursorPos(&pos);
	return {static_cast<float>(pos.x), static_cast<float>(pos.y)};
}
