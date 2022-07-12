#include "SSScreen.h"

#include "VertexHelper.h"

void SSScreen::Initialize(Microsoft::WRL::ComPtr<ID3D11Device> pDevice)
{
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
