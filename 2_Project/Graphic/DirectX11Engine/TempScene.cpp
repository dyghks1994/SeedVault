//-----------------------------------------------------------------------------
// File: Scene.cpp
//
// Desc: Holds a description for a simple scene usend in the Order Independent
//       Transparency sample.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//-----------------------------------------------------------------------------
#include "TempScene.h"

//using namespace DirectX;

struct SCENE_VERTEX
{
	XMFLOAT4 pos;
	XMFLOAT4 color;
};

//--------------------------------------------------------------------------------------
// CScene constructor
//--------------------------------------------------------------------------------------
TempScene::TempScene()
	: m_pVertexShader(nullptr)
	, m_pVertexLayout(nullptr)
	, m_pVS_CB(nullptr)
	, m_pVB(nullptr)
{
}


//--------------------------------------------------------------------------------------
// Allocate device resources
//--------------------------------------------------------------------------------------
void TempScene::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	// Create the vertex shader
	m_pVertexShader = std::make_shared<VertexShader>();
	m_pVertexShader->Initialize(pDevice, L"../Data/Shader/vs_OIT.hlsl", "SceneVS", "vs_5_0");


	// Set up constant buffer

	m_pVS_CB = new ConstantBuffer<VS_CB>();
	m_pVS_CB->Initialize(pDevice.Get(), pDeviceContext.Get());

	// Set up vertex buffer
	float fRight = -10.0f;
	float fTop = -10.0f;
	float fLeft = 10.0f;
	float fLowH = -5.0f;

	// Fill the vertex buffer
	SCENE_VERTEX pVertex[12];
	pVertex[0].pos = XMFLOAT4(fLeft, fLowH, 50.0f, 1.0f);
	pVertex[1].pos = XMFLOAT4(fLeft, fTop, 50.0f, 1.0f);
	pVertex[2].pos = XMFLOAT4(fRight, fLowH, 50.0f, 1.0f);
	pVertex[3].pos = XMFLOAT4(fRight, fTop, 50.0f, 1.0f);

	pVertex[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);
	pVertex[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);
	pVertex[2].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);
	pVertex[3].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.5f);

	pVertex[4].pos = XMFLOAT4(fLeft, fLowH, 60.0f, 1.0f);
	pVertex[5].pos = XMFLOAT4(fLeft, fTop, 60.0f, 1.0f);
	pVertex[6].pos = XMFLOAT4(fRight, fLowH, 40.0f, 1.0f);
	pVertex[7].pos = XMFLOAT4(fRight, fTop, 40.0f, 1.0f);

	pVertex[4].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	pVertex[5].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	pVertex[6].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);
	pVertex[7].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.5f);

	pVertex[8].pos = XMFLOAT4(fLeft, fLowH, 40.0f, 1.0f);
	pVertex[9].pos = XMFLOAT4(fLeft, fTop, 40.0f, 1.0f);
	pVertex[10].pos = XMFLOAT4(fRight, fLowH, 60.0f, 1.0f);
	pVertex[11].pos = XMFLOAT4(fRight, fTop, 60.0f, 1.0f);

	pVertex[8].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.5f);
	pVertex[9].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.5f);
	pVertex[10].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.5f);
	pVertex[11].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.5f);

	D3D11_BUFFER_DESC vbdesc;
	vbdesc.ByteWidth = 12 * sizeof(SCENE_VERTEX);
	vbdesc.Usage = D3D11_USAGE_IMMUTABLE;
	vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbdesc.CPUAccessFlags = 0;
	vbdesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pVertex;
	pDevice->CreateBuffer(&vbdesc, &InitData, &m_pVB);
}

//--------------------------------------------------------------------------------------
// Render the scene
//--------------------------------------------------------------------------------------
void TempScene::D3D11Render(CXMMATRIX mWVP, Microsoft::WRL::ComPtr<ID3D11DeviceContext> pDeviceContext)
{
	pDeviceContext->IASetInputLayout(m_pVertexShader->GetInputLayout());

	UINT uStrides = sizeof(SCENE_VERTEX);
	UINT uOffsets = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &uStrides, &uOffsets);
	pDeviceContext->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	pDeviceContext->VSSetShader(m_pVertexShader->GetVertexShader(), nullptr, 0);

	// Update the constant buffer
	XMFLOAT4X4 matWVP;
	XMStoreFloat4x4(&matWVP, mWVP);
	m_pVS_CB->data.mWorldViewProj = matWVP;
	m_pVS_CB->ApplyChanges();

	pDeviceContext->VSSetConstantBuffers(0, 1, m_pVS_CB->GetAddressOf());

	pDeviceContext->Draw(4, 0);
	//pDeviceContext->DrawIndexed(6, 0, 0);

	uOffsets = 4 * sizeof(SCENE_VERTEX);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &uStrides, &uOffsets);
	pDeviceContext->Draw(4, 0);
	//pDeviceContext->DrawIndexed(6, 0, 0);

	uOffsets = 8 * sizeof(SCENE_VERTEX);
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &uStrides, &uOffsets);
	pDeviceContext->Draw(4, 0);
	//pDeviceContext->DrawIndexed(6, 0, 0);
}


//--------------------------------------------------------------------------------------
// Release device resources
//--------------------------------------------------------------------------------------
void TempScene::OnD3D11DestroyDevice()
{
	SAFE_RELEASE(m_pVB);
}
