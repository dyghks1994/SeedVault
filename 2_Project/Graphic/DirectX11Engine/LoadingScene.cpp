#include "LoadingScene.h"
#include <WICTextureLoader.h>
#include <DirectXMath.h>
#include <random>

#include "Renderer.h"

#include "Shaders.h"
#include "InputLayout.h"

LoadingScene::LoadingScene()
	: m_curIdx(0), m_FrameCount(0)
{
}

LoadingScene::~LoadingScene()
{
	delete m_VertexShader;
	m_VertexShader = nullptr;

	delete m_pixelShader;
	m_pixelShader = nullptr;
}

void LoadingScene::SetImageAndFps(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<std::string>& fileName_V, const std::string& vertexShaderName, const std::string& pixelShaderName, float width, float height)
{
	for (int i = 0; i < fileName_V.size(); ++i)
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _tempBitmap;
		Microsoft::WRL::ComPtr<ID3D11Resource> _texResource;

		std::wstring _FileName;
		_FileName.assign(fileName_V[i].begin(), fileName_V[i].end());

		HRESULT _hr = DirectX::CreateWICTextureFromFile(pDevice.Get(), _FileName.c_str(), &_texResource, &_tempBitmap);
		
		m_ImageInfo_V.push_back(_tempBitmap);
	}

	std::wstring _vertexShaderName;
	_vertexShaderName.assign(vertexShaderName.begin(), vertexShaderName.end());

	std::wstring _pixelShaderName;
	_pixelShaderName.assign(pixelShaderName.begin(), pixelShaderName.end());

	m_VertexShader = new VertexShader();
	m_pixelShader = new PixelShader();

	m_VertexShader->Initialize(pDevice, _vertexShaderName);

	InputLayout::CrateInputLayoutFromVertexShader(pDevice.Get(), m_VertexShader->GetVertexShaderBuffer(), &m_VertexShader->inputLayout);

	m_pixelShader->Initialize(pDevice, _pixelShaderName);


	m_worldTM =
		DirectX::XMMatrixScaling(width, height, 1.0f) *
		DirectX::XMMatrixRotationRollPitchYaw(0.f, 0.f + DirectX::XM_PI, 0.f + DirectX::XM_PI) *
		DirectX::XMMatrixTranslation(width / 2, height / 2, 0.f);

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_int_distribution<int> distr(0, m_ImageInfo_V.size() - 1);

	m_curIdx = distr(eng);
}

void LoadingScene::Draw(Renderer* renderer)
{
	++m_FrameCount;

	if (m_FrameCount > 20000)
	{
		std::random_device rd;
		std::default_random_engine eng(rd());
		std::uniform_int_distribution<int> distr(0, m_ImageInfo_V.size() - 1);

		m_curIdx = distr(eng);

		m_FrameCount = 0;
	}

	renderer->DrawSprite(m_ImageInfo_V[m_curIdx], m_worldTM, m_VertexShader, m_pixelShader);
}
