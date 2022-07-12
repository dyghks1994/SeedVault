#pragma once
#include "ComponentBase.h"

/// <summary>
/// 왼손에 위치하는 힐 포션의 위치를 조정하는 컴포넌트
/// 힐포션은 사용중일 때만 메쉬 렌더가 On 되고 사용중이지 않을 떄는 Off
/// 작성자 : 최 요 환
/// </summary>

class LeftHandController : public ComponentBase
{
public:
	LeftHandController();
	virtual ~LeftHandController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

protected:

private:
	// 플레이어 메쉬
	MeshFilter* m_PlayerMesh;

	// 플레이어 컨트롤러
	class PlayerController* m_PlayerController;

	// 힐 포션 메쉬 렌더러
	MeshRenderer* m_HealPotionRenderer;

	SimpleMath::Matrix* m_HealPotionParent;
	SimpleMath::Vector3 m_TargetScale;
	SimpleMath::Quaternion m_TargetQuaternion;
	SimpleMath::Vector3 m_TargetTranslation;

};

