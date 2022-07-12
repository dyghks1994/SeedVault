#pragma once

#include "D3D11Define.h"

class SSScreen
{
public:
	void Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice);

	DirectX::XMFLOAT4 m_FrustumFarCorner[4];
	Microsoft::WRL::ComPtr <ID3D11Buffer> m_ScreenQuadVB;
	Microsoft::WRL::ComPtr <ID3D11Buffer> m_ScreenQuadIB;
};

