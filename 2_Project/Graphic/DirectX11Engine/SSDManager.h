#pragma once

#include "D3D11Define.h"
#include "SimpleMath.h"
#include <vector>

struct SSDInfo
{
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> texture;
	DirectX::XMMATRIX world;
};

class SSDManager
{
public:

	~SSDManager();
	void Initialize(std::shared_ptr<class SSScreen> pSSScreen);
	void AddSSD(Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> texture, const DirectX::XMMATRIX& world);
	void DrawSSD(
		Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pNormapDepth,
		class VertexShader* pVS,
		class PixelShader* pPS,
		const DirectX::XMMATRIX& view,
		const DirectX::XMMATRIX& proj);

private:
	std::shared_ptr<class SSScreen> m_pSSScreen;
	std::vector<std::unique_ptr<SSDInfo>> m_SSDQueue_V;
	unsigned int m_DecalCount;
};


