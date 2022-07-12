#pragma once
#include "ComponentBase.h"

/// <summary>
/// 플레이어에 붙어서 전방을 밝히는 라이트의 트랜스폼 업데이트용 클래스
/// 
/// 작성자 : 최 요 환
/// </summary>

class HeadLightController : public ComponentBase
{
public:
	HeadLightController();
	virtual ~HeadLightController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

private:
	GameObject* m_HeadLightObj;
	Light* m_Light;

	MeshFilter* m_PlayerMeshFilter;
	

	SimpleMath::Matrix* m_HeadLightParent;
	SimpleMath::Vector3 m_TargetScale;
	SimpleMath::Quaternion m_TargetQuaternion;
	SimpleMath::Vector3 m_TargetTranslation;
};

