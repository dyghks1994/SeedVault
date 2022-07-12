#pragma once
#include <vector>
#include <string>

#include <wrl/client.h>
#include <d3d11.h>
#include <SimpleMath.h>

struct ImageInfo
{
};

class LoadingScene
{
public:
	LoadingScene();
	~LoadingScene();
public:
	void SetImageAndFps(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<std::string>& fileName_V, const std::string& vertexShaderName, const std::string& pixelShaderName, float width, float height);
	void Draw(class Renderer* renderer);

private:
	class VertexShader* m_VertexShader;
	class PixelShader* m_pixelShader;

	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_ImageInfo_V;

	DirectX::SimpleMath::Matrix m_worldTM;

	int m_curIdx = 0;

	//float m_fps;
	int m_FrameCount = 0;
};

