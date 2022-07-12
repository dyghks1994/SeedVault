#include "SSDManager.h"

#include "VertexHelper.h"
#include "Shaders.h"
#include "SSScreen.h"

#include "ConstantBufferManager.h"
#include "RasterizerState.h"

SSDManager::~SSDManager()
{
	m_pSSScreen.reset();
	m_SSDQueue_V.clear();
}

void SSDManager::Initialize(std::shared_ptr<SSScreen> pSSScreen)
{
	m_pSSScreen = pSSScreen;

	for (int Index = 0; Index < 20; Index++)
	{
		m_SSDQueue_V.emplace_back(std::make_unique<SSDInfo>());
	}
}

void SSDManager::AddSSD(Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> texture, const DirectX::XMMATRIX& world)
{
	m_SSDQueue_V[m_DecalCount]->texture = texture;
	m_SSDQueue_V[m_DecalCount]->world = world;
	m_DecalCount++;
}

void SSDManager::DrawSSD(
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> pDeviceContext,
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> pNormapDepth,
	class VertexShader* pVS,
	class PixelShader* pPS,
	const DirectX::XMMATRIX& view,
	const DirectX::XMMATRIX& proj)
{
	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetSSAOSamplerStateAddressOf());
	ID3D11BlendState* _bs;
	pDeviceContext->OMGetBlendState(&_bs, nullptr, nullptr);
	pDeviceContext->OMSetBlendState(RasterizerState::GetAlphaBlenderState(), nullptr, 0xffffffff);

	for (unsigned int i = 0; i < m_DecalCount; i++)
	{
		
		{	// 픽셀 셰이더 상수 버퍼

			pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSAmbientBuffer()->GetAddressOf());

			DirectX::SimpleMath::Matrix _invView = view;
			_invView = _invView.Invert();
			DirectX::SimpleMath::Matrix _world = m_SSDQueue_V[i]->world;

			ConstantBufferManager::GetVertexBuffer()->data.wvpMatrix = _invView;
			ConstantBufferManager::GetVertexBuffer()->data.worldMatrix = _world.Invert();
			ConstantBufferManager::GetVertexBuffer()->ApplyChanges();
			pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetVertexBuffer()->GetAddressOf());
		}

		ID3D11ShaderResourceView* _srv[2] =
		{
			m_SSDQueue_V[i]->texture.Get(),
			pNormapDepth.Get(),
		};

		pDeviceContext->PSSetShaderResources(0, 2, _srv);

		// 데칼의 버텍스 셰이더를 먼저 실행 시킨다
		UINT stride = sizeof(Vertex::VertexPosNormalTex);
		UINT offset = 0;

		pDeviceContext->IASetInputLayout(pVS->GetInputLayout());
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_pSSScreen->m_ScreenQuadVB.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pSSScreen->m_ScreenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

		pDeviceContext->VSSetShader(pVS->GetVertexShader(), nullptr, 0);
		pDeviceContext->PSSetShader(pPS->GetPixelShader(), nullptr, 0);

		pDeviceContext->DrawIndexed(6, 0, 0);
	}

	m_DecalCount = 0;

	pDeviceContext->OMSetBlendState(_bs, nullptr, 0xffffffff);
}
