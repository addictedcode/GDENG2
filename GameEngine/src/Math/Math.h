#pragma once

#include "DirectXMath.h"

namespace Math
{

	DirectX::XMVECTOR QuaternionToEuler(const DirectX::XMVECTOR& quat);
	
}