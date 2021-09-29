#include "stdafx.h"
#include "Math.h"

DirectX::XMVECTOR Math::QuaternionToEuler(const DirectX::XMVECTOR& quat)
{
	DirectX::XMMATRIX matrix = DirectX::XMMatrixRotationQuaternion(quat);

	float Row[3][3];

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			Row[i][j] = matrix.r[i].m128_f32[j];
	
	float x, y, z;
	
	y = asin(-Row[0][2]);
	if (cos(y) != 0) {
		x = atan2(Row[1][2], Row[2][2]);
		z = atan2(Row[0][1], Row[0][0]);
	}
	else {
		x = atan2(-Row[2][0], Row[1][1]);
		z = 0;
	}

	DirectX::XMVECTOR euler = { x, y, z };
	return euler;
}