#pragma once
#include "ComponentBase.h"

/// <summary>
/// �÷��̾ �پ ������ ������ ����Ʈ�� Ʈ������ ������Ʈ�� Ŭ����
/// 
/// �ۼ��� : �� �� ȯ
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

