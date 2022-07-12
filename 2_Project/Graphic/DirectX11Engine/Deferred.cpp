#include "Deferred.h"

#include "DepthStencilView.h"
#include "RenderTargetView.h"
#include "ConstantBufferManager.h"
#include "RasterizerState.h"
#include "VertexBuffer.h"
#include "Shaders.h"
#include "Quad.h"

#define DEFERRED_DEBUG

Deferred::Deferred()
{

}

Deferred::~Deferred()
{
}

void Deferred::Initialize(ID3D11Device* pDevice, UINT width, UINT height)
{
	// ���� ���Ľ� ������Ʈ�� ������ ������ �ʿ����
	// �̹� �ѹ��� �����θ� �ȴ�
	CreateDepthStencilState(pDevice);

	m_pDepthBuffer = std::make_unique<DepthStencilView>();
	m_pDepthBuffer->Initialize(pDevice, width, height,
		D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,	// �׳� BIND_DEPTH_STENCIL�� �ϸ� ������..
		DXGI_FORMAT_R24G8_TYPELESS,								// �ؽ�ó ����
		DXGI_FORMAT_R24_UNORM_X8_TYPELESS,						// ���� Ÿ�� ����(���� ���ٽ� ����)
		DXGI_FORMAT_D24_UNORM_S8_UINT);							// ���̴� ���ҽ� �� ����

	for (unsigned int i = 0; i < DRAW_LAYER; i++)
	{
		m_pDeferredOpaqueRTV_V.emplace_back(std::make_unique<RenderTargetView>());
		m_pDeferredTransparentRTV_V.emplace_back(std::make_unique<RenderTargetView>());
	}

	UINT _bindFlag = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	m_pDeferredOpaqueRTV_V[ALBEDO_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredOpaqueRTV_V[EMISSIVE_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredOpaqueRTV_V[MATERIAL_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pDeferredOpaqueRTV_V[NORMALDEPTH_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredOpaqueRTV_V[AMBIENT_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredOpaqueRTV_V[SHADOW_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredOpaqueRTV_V[ID_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R32G32B32A32_UINT);
	
	m_pDeferredTransparentRTV_V[ALBEDO_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredTransparentRTV_V[EMISSIVE_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredTransparentRTV_V[MATERIAL_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R8G8B8A8_UNORM);
	m_pDeferredTransparentRTV_V[NORMALDEPTH_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredTransparentRTV_V[AMBIENT_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredTransparentRTV_V[SHADOW_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);
	m_pDeferredTransparentRTV_V[ID_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R32G32B32A32_UINT);

	m_pOpaqueFrameBuffer = std::make_unique<RenderTargetView>();
	m_pOpaqueFrameBuffer->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pAccumulation = std::make_unique<RenderTargetView>();
	m_pAccumulation->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);

	m_pRevealage = std::make_unique<RenderTargetView>();
	m_pRevealage->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R16G16B16A16_FLOAT);

	// �븻�� �������� ���� Ȱ���� ���� Bind�� �߰��ߴ�
	_bindFlag |= D3D11_BIND_UNORDERED_ACCESS;

	m_pDeferredOpaqueRTV_V[NORMAL_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R8G8B8A8_UNORM);

	m_pDeferredTransparentRTV_V[NORMAL_MAP]->Initialize(pDevice, width, height, _bindFlag, DXGI_FORMAT_R8G8B8A8_UNORM);

	// �ܺο��� ���� �־��� ���� ������ ���ο��� ó���ϴ°� �ξ� �� �����ϴٰ� �����ߴ�
	m_pScreenQaud = std::make_shared<Quad>();
	m_pScreenQaud->BuildHelper(pDevice);


	m_VertexShader = std::make_shared<VertexShader>();
	m_VertexShader->Initialize(pDevice, L"../Data/Shader/ps_deferredTotal.hlsl", "FullScreenQuadVS", "vs_5_0");



	{

		m_ScreenQuadVB.Reset();
		m_ScreenQuadIB.Reset();

		Vertex::VertexPosNormalTex _vertex[4];

		_vertex[0].pos = XMFLOAT3(-1.0f, -1.0f, 0.0f);
		_vertex[1].pos = XMFLOAT3(-1.0f, +1.0f, 0.0f);
		_vertex[2].pos = XMFLOAT3(+1.0f, +1.0f, 0.0f);
		_vertex[3].pos = XMFLOAT3(+1.0f, -1.0f, 0.0f);

		// Store far plane frustum corner indices in Normal.x slot.
		_vertex[0].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		_vertex[1].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
		_vertex[2].normal = XMFLOAT3(2.0f, 0.0f, 0.0f);
		_vertex[3].normal = XMFLOAT3(3.0f, 0.0f, 0.0f);

		_vertex[0].tex = XMFLOAT2(0.0f, 1.0f);
		_vertex[1].tex = XMFLOAT2(0.0f, 0.0f);
		_vertex[2].tex = XMFLOAT2(1.0f, 0.0f);
		_vertex[3].tex = XMFLOAT2(1.0f, 1.0f);

		D3D11_BUFFER_DESC _vbd;
		_vbd.Usage = D3D11_USAGE_IMMUTABLE;
		_vbd.ByteWidth = sizeof(Vertex::VertexPosNormalTex) * 4;
		_vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		_vbd.CPUAccessFlags = 0;
		_vbd.MiscFlags = 0;
		_vbd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA _vinitData;
		_vinitData.pSysMem = _vertex;

		D3D11_BUFFER_DESC _ibd;
		_ibd.Usage = D3D11_USAGE_IMMUTABLE;
		_ibd.ByteWidth = sizeof(USHORT) * 6;
		_ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		_ibd.CPUAccessFlags = 0;
		_ibd.StructureByteStride = 0;
		_ibd.MiscFlags = 0;

		USHORT _indices[6] =
		{
			0, 1, 2,
			0, 2, 3
		};

		D3D11_SUBRESOURCE_DATA _iinitData;
		_iinitData.pSysMem = _indices;

		try
		{
			HRESULT _hr;

			_hr = pDevice->CreateBuffer(&_vbd, &_vinitData, m_ScreenQuadVB.GetAddressOf());
			COM_ERROR_IF_FAILED(_hr, "CreateBuffer Fail");

			_hr = pDevice->CreateBuffer(&_ibd, &_iinitData, m_ScreenQuadIB.GetAddressOf());
			COM_ERROR_IF_FAILED(_hr, "CreateBuffer Fail");

		}
		catch (COMException& e)
		{
			ErrorLogger::Log(e);
		}

	}
}

void Deferred::OnResize(ID3D11Device* pDevice, UINT width, UINT height)
{
	m_pDeferredOpaqueRTV_V.clear();
	m_pDepthBuffer.reset();

	// unique_ptr�̹Ƿ� clear�� �ذ�
	m_pDeferredOpaqueRTV_V.clear();
	m_pDeferredTransparentRTV_V.clear();

	m_pOpaqueFrameBuffer.reset();
	m_pAccumulation.reset();
	m_pRevealage.reset();

	Initialize(pDevice, width, height);
}

void Deferred::OpaqueBeginRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	pDeviceContext->RSSetState(0);
	pDeviceContext->RSSetViewports(1, &m_pDeferredOpaqueRTV_V[ALBEDO_MAP]->GetViewPort());


	/// ���� ������, �̹̽ú�� �߰� ���� ���´�
	// ���� �����ϴ� ���߿� �ٱ� ������ ���̴°� �ƴ϶�� ���� �� ������ �� �ʿ� ����
	// ������ �ȳ��� ���̱� ������

	// �ٱ��� �ִ� ����׿� ���� Ÿ���� �����ͼ� ������
	DeferredRenderTargetClear(pDeviceContext);

	// Bind all the render targets togther
	ID3D11RenderTargetView* rt[DRAW_LAYER];
	for (int i = 0; i < DRAW_LAYER; i++)
	{
		rt[i] = m_pDeferredOpaqueRTV_V[i]->GetRenderTargerViewRawptr();
	}

	// ���� ���ٽ� ���۴� Ŭ�������� �ʴ´�
	pDeviceContext->OMSetRenderTargets(DRAW_LAYER, rt, m_pDepthBuffer->GetDepthSetncilView());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
}

void Deferred::TransparentBeginRender(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	pDeviceContext->RSSetState(0);
	pDeviceContext->RSSetViewports(1, &m_pDeferredOpaqueRTV_V[ALBEDO_MAP]->GetViewPort());

	// �ٱ��� �ִ� ����׿� ���� Ÿ���� �����ͼ� ������
	DeferredRenderTransparentTargetClear(pDeviceContext);

	// Bind all the render targets togther
	ID3D11RenderTargetView* rt[DRAW_LAYER];
	for (int i = 0; i < DRAW_LAYER; i++)
	{
		rt[i] = m_pDeferredTransparentRTV_V[i]->GetRenderTargerViewRawptr();
	}

	// ���� ���ٽ� ���۴� Ŭ�������� �ʴ´�
	pDeviceContext->OMSetRenderTargets(DRAW_LAYER, rt, m_pDepthBuffer->GetDepthSetncilView());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetDepthStencilState(), 1);
}

void Deferred::DeferredRenderTargetClear(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	for (unsigned int i = 0; i < DRAW_LAYER; i++)
	{
		pDeviceContext->ClearRenderTargetView(m_pDeferredOpaqueRTV_V[i]->GetRenderTargerViewRawptr(), null);
	}

	pDeviceContext->ClearRenderTargetView(m_pDeferredOpaqueRTV_V[NORMALDEPTH_MAP]->GetRenderTargerViewRawptr(), normalDepthColor);
}

void Deferred::DeferredRenderTransparentTargetClear(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	for (unsigned int i = 0; i < DRAW_LAYER; i++)
	{
		pDeviceContext->ClearRenderTargetView(m_pDeferredTransparentRTV_V[i]->GetRenderTargerViewRawptr(), null);
	}

	pDeviceContext->ClearRenderTargetView(m_pDeferredTransparentRTV_V[NORMALDEPTH_MAP]->GetRenderTargerViewRawptr(), normalDepthColor);
}

void Deferred::BeginRenderWithOutDepth(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	pDeviceContext->RSSetState(0);
	pDeviceContext->RSSetViewports(1, &m_pDeferredOpaqueRTV_V[ALBEDO_MAP]->GetViewPort());


	/// ���� ������, �̹̽ú�� �߰� ���� ���´�
	// ���� �����ϴ� ���߿� �ٱ� ������ ���̴°� �ƴ϶�� ���� �� ������ �� �ʿ� ����
	// ������ �ȳ��� ���̱� ������

	// �ٱ��� �ִ� ����׿� ���� Ÿ���� �����ͼ� ������
	pDeviceContext->ClearRenderTargetView(m_pDeferredOpaqueRTV_V[ALBEDO_MAP]->GetRenderTargerViewRawptr(), null);

	// ���� ���ٽ� ���۴� Ŭ�������� �ʴ´�
	pDeviceContext->OMSetRenderTargets(1, m_pDeferredOpaqueRTV_V[ALBEDO_MAP]->GetRenderTargetViewAddressOf(), m_pDepthBuffer->GetDepthSetncilView());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetOITInitDS(), 1);
}

void Deferred::BindAccumReveal(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	ID3D11RenderTargetView* _rt[2] =
	{
		m_pAccumulation->GetRenderTargerViewRawptr(),
		m_pRevealage->GetRenderTargerViewRawptr(),
	};
	pDeviceContext->OMSetRenderTargets(2, _rt, m_pDepthBuffer->GetDepthSetncilView());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetOITInitDS(), 1);

}

void Deferred::UnbindRenderTargets(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	ID3D11RenderTargetView* rt[DRAW_LAYER] =
	{
		nullptr,
	};
	pDeviceContext->OMSetRenderTargets(DRAW_LAYER, rt, nullptr);
}

void Deferred::RenderTargetClear(Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	// Queue�� �־�߸� �ʱ�ȭ�� �ǵ��� �����ϰ� �־��� ������
	// ������ ���ư����� ������ �����Ѵ�
	float _darkGray[4] = { 0.3f, 0.3f,  0.3f, 1.0f };
	pDeviceContext->ClearRenderTargetView(m_pOpaqueFrameBuffer->GetRenderTargerViewRawptr(), _darkGray);

	float nullColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float nullButRed[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	pDeviceContext->ClearRenderTargetView(m_pAccumulation->GetRenderTargerViewRawptr(), nullColor);
	pDeviceContext->ClearRenderTargetView(m_pRevealage->GetRenderTargerViewRawptr(), nullButRed);
	pDeviceContext->ClearDepthStencilView(m_pDepthBuffer->GetDepthSetncilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

}

void Deferred::CombineRenderTargetOpaque(
	bool debug,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader,
	ID3D11ShaderResourceView* pLightTexture,
	ID3D11ShaderResourceView* pAO,
	const DirectX::SimpleMath::Matrix& view)
{
	UINT stride = sizeof(Vertex::Vertex2D);
	UINT offset = 0;


#ifdef DEFERRED_DEBUG
	// ������� ���� ��ũ�� ���忡 �׸�
	{
		//UINT stride = sizeof(Vertex::Vertex2D);
		UINT stride = sizeof(Vertex::VertexPosNormalTex);
		UINT offset = 0;

		/*
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_pScreenQaud->GetAddressOfVertexBuffer(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pScreenQaud->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.05f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		*/

		
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_ScreenQuadVB.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_ScreenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

		ConstantBufferManager::GetPSAmbientBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSAmbientBuffer()->GetAddressOf());

	}
#else
	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
#endif 

	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());
	pDeviceContext->OMSetBlendState(RasterizerState::GetBlenderState(), nullptr, 0xffffffff);

	ConstantBufferManager::GetPSDirLightBuffer()->ApplyChanges();
	pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSDirLightBuffer()->GetAddressOf());

	ConstantBufferManager::GetLightCountBuffer()->ApplyChanges();
	pDeviceContext->PSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightCountBuffer()->GetAddressOf());

	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetPointSamplerStateAddressOf());
	pDeviceContext->PSSetSamplers(1, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	
	DirectX::SimpleMath::Matrix _invView = view.Invert();

	float _invViewFloat[16] =
	{
		_invView._11, _invView._12, _invView._13, _invView._14,
		_invView._21, _invView._22, _invView._23, _invView._24,
		_invView._31, _invView._32, _invView._33, _invView._34,
		_invView._41, _invView._42, _invView._43, _invView._44,
	};

	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::PS, CBufferSize::SIZE_16, 3, _invViewFloat);

	// ����� ����
	for (int i = 0; i < DRAW_LAYER - 1; i++)
	{
		pDeviceContext->PSSetShaderResources(i, 1, m_pDeferredOpaqueRTV_V[i]->GetShaderResourceViewAddressOf());
	}

	ID3D11ShaderResourceView* _srv[2] =
	{
		pLightTexture, pAO,
	};
	pDeviceContext->PSSetShaderResources(DRAW_LAYER - 1, 2, _srv);


#ifdef DEFERRED_DEBUG
	pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
	pDeviceContext->DrawIndexed(6, 0, 0);
	//pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);

#else
	// ���� ���� �׸���
	pDeviceContext->VSSetShader(m_VertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
	pDeviceContext->Draw(4, 0);

#endif

	// Cleanup
	ID3D11ShaderResourceView* arrViews[DRAW_LAYER] = { nullptr };
	ZeroMemory(arrViews, sizeof(arrViews));
	///pDeviceContext->PSSetShaderResources(0, 1, arrViews);
	/// �������� �̷������� arrViews�� �־�����, ��� �Ű������� 1�� �δ� �Ǽ��� ���ߴ�
	pDeviceContext->PSSetShaderResources(0, DRAW_LAYER, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}


void Deferred::CombineRenderTargetTransparent(
	bool debug,
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader,
	ID3D11ShaderResourceView* pTextrueLightMap,
	const DirectX::SimpleMath::Matrix& view)

{
	// ���⼭ �� ���� ���´�
	pDeviceContext->RSSetState(0);
	pDeviceContext->RSSetViewports(1, &m_pAccumulation->GetViewPort());

	ID3D11RenderTargetView* _rt[2] =
	{
		m_pAccumulation->GetRenderTargerViewRawptr(),
		m_pRevealage->GetRenderTargerViewRawptr(),
	};
	pDeviceContext->OMSetRenderTargets(2, _rt, m_pDepthBuffer->GetDepthSetncilView());
	pDeviceContext->OMSetDepthStencilState(RasterizerState::GetOITInitDS(), 1);

#ifdef DEFERRED_DEBUG
	// ������� ���� ��ũ�� ���忡 �׸�
	{
		/*
		UINT stride = sizeof(Vertex::Vertex2D);
		UINT offset = 0;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_pScreenQaud->GetAddressOfVertexBuffer(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pScreenQaud->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.05f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		*/

		UINT stride = sizeof(Vertex::VertexPosNormalTex);
		UINT offset = 0;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_ScreenQuadVB.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_ScreenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

		ConstantBufferManager::GetPSAmbientBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSAmbientBuffer()->GetAddressOf());

	}
#else
	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
#endif 

	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());
	pDeviceContext->OMSetBlendState(RasterizerState::GetTransparentSetState(), nullptr, 0xffffffff);


	DirectX::SimpleMath::Matrix _invView = view.Invert();

	float _invViewFloat[16] =
	{
		_invView._11, _invView._12, _invView._13, _invView._14,
		_invView._21, _invView._22, _invView._23, _invView._24,
		_invView._31, _invView._32, _invView._33, _invView._34,
		_invView._41, _invView._42, _invView._43, _invView._44,
	};

	ConstantBufferManager::SetGeneralCBuffer(pDeviceContext, CBufferType::PS, CBufferSize::SIZE_16, 3, _invViewFloat);

	ConstantBufferManager::GetPSDirLightBuffer()->ApplyChanges();
	pDeviceContext->PSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSDirLightBuffer()->GetAddressOf());

	ConstantBufferManager::GetLightCountBuffer()->ApplyChanges();
	pDeviceContext->PSSetConstantBuffers(1, 1, ConstantBufferManager::GetLightCountBuffer()->GetAddressOf());

	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetPointSamplerStateAddressOf());
	pDeviceContext->PSSetSamplers(1, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	// ����� ����
	for (int i = 0; i < DRAW_LAYER - 1; i++)
	{
		pDeviceContext->PSSetShaderResources(i, 1, m_pDeferredTransparentRTV_V[i]->GetShaderResourceViewAddressOf());
	}
	pDeviceContext->PSSetShaderResources(DRAW_LAYER - 1, 1, &pTextrueLightMap);

#ifdef DEFERRED_DEBUG
	pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
	pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);

#else
	// ���� ���� �׸���
	pDeviceContext->VSSetShader(m_VertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
	pDeviceContext->Draw(4, 0);

#endif
	// Cleanup
	ID3D11ShaderResourceView* arrViews[DRAW_LAYER] = { nullptr };
	ZeroMemory(arrViews, sizeof(arrViews));
	pDeviceContext->PSSetShaderResources(0, DRAW_LAYER, arrViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);
}

void Deferred::CompositeFrameBuffers(
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext,
	std::shared_ptr<RenderTargetView> pSwapChainRenderTarget,
	class VertexShader* pVertexShader,
	class PixelShader* pPixelShader)
{
	pDeviceContext->OMSetRenderTargets(1, pSwapChainRenderTarget->GetRenderTargetViewAddressOf(), nullptr);

	UINT stride = sizeof(Vertex::Vertex2D);
	UINT offset = 0;

#ifdef DEFERRED_DEBUG
	// ������� ���� ��ũ�� ���忡 �׸�
	{
		/*
		UINT stride = sizeof(Vertex::Vertex2D);
		UINT offset = 0;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_pScreenQaud->GetAddressOfVertexBuffer(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pScreenQaud->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

		XMMATRIX world(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.05f, 1.0f);

		ConstantBufferManager::GetVSPerObjectBuffer()->data.World = world;
		ConstantBufferManager::GetVSPerObjectBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetVSPerObjectBuffer()->GetAddressOf());
		*/

		UINT stride = sizeof(Vertex::VertexPosNormalTex);
		UINT offset = 0;

		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pDeviceContext->IASetVertexBuffers(0, 1, m_ScreenQuadVB.GetAddressOf(), &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_ScreenQuadIB.Get(), DXGI_FORMAT_R16_UINT, 0);

		ConstantBufferManager::GetPSAmbientBuffer()->ApplyChanges();
		pDeviceContext->VSSetConstantBuffers(0, 1, ConstantBufferManager::GetPSAmbientBuffer()->GetAddressOf());

	}
#else
	pDeviceContext->IASetInputLayout(NULL);
	pDeviceContext->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
	pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_UNKNOWN, 0);
#endif 

	pDeviceContext->RSSetState(RasterizerState::GetNoCullingRS());
	float _blend[4] = { 0.0f, 0.0f,  0.0f,  0.0f };
	pDeviceContext->OMSetBlendState(RasterizerState::GetTransparentState(), nullptr, 0xffffffff);

	pDeviceContext->PSSetSamplers(0, 1, RasterizerState::GetPointSamplerStateAddressOf());
	pDeviceContext->PSSetSamplers(1, 1, RasterizerState::GetLinearSamplerStateAddressOf());

	ID3D11ShaderResourceView* arrViews[2] =
	{
		m_pAccumulation->GetShaderResourceViewRawptr(),
		m_pRevealage->GetShaderResourceViewRawptr()
	};

	pDeviceContext->PSSetShaderResources(0, 2, arrViews);

#ifdef DEFERRED_DEBUG
	pDeviceContext->IASetInputLayout(pVertexShader->inputLayout.Get());
	pDeviceContext->VSSetShader(pVertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
	pDeviceContext->DrawIndexed(m_pScreenQaud->GetIndexCount(), 0, 0);

#else
	// ���� ���� �׸���
	pDeviceContext->VSSetShader(m_VertexShader->GetVertexShader(), NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader->GetPixelShader(), NULL, 0);
	pDeviceContext->Draw(4, 0);
#endif

	// Cleanup
	ID3D11ShaderResourceView* nullViews[2] = { nullptr };
	ZeroMemory(nullViews, sizeof(nullViews));
	pDeviceContext->PSSetShaderResources(0, 2, nullViews);
	pDeviceContext->VSSetShader(NULL, NULL, 0);
	pDeviceContext->PSSetShader(NULL, NULL, 0);

}

std::unique_ptr<DepthStencilView>& Deferred::GetDepthBuffer()
{
	return m_pDepthBuffer;
}

const std::vector< std::unique_ptr<RenderTargetView>>& Deferred::GetDrawLayers()
{
	return m_pDeferredOpaqueRTV_V;
}

const std::vector< std::unique_ptr<RenderTargetView>>& Deferred::GetTransparentDrawLayers()
{
	return m_pDeferredTransparentRTV_V;
}

void Deferred::CreateDepthStencilState(ID3D11Device* pDevice)
{
	D3D11_DEPTH_STENCIL_DESC descDepth;
	descDepth.DepthEnable = TRUE;
	descDepth.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	descDepth.DepthFunc = D3D11_COMPARISON_LESS;
	descDepth.StencilEnable = TRUE;
	descDepth.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	descDepth.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	const D3D11_DEPTH_STENCILOP_DESC stencilMarkOp = { D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_REPLACE, D3D11_COMPARISON_ALWAYS };
	descDepth.FrontFace = stencilMarkOp;
	descDepth.BackFace = stencilMarkOp;

	pDevice->CreateDepthStencilState(&descDepth, m_pDepthStencilState.GetAddressOf());
}

