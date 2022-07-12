#pragma once
#include "ComponentBase.h"

/// <summary>
/// �޼տ� ��ġ�ϴ� �� ������ ��ġ�� �����ϴ� ������Ʈ
/// �������� ������� ���� �޽� ������ On �ǰ� ��������� ���� ���� Off
/// �ۼ��� : �� �� ȯ
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
	// �÷��̾� �޽�
	MeshFilter* m_PlayerMesh;

	// �÷��̾� ��Ʈ�ѷ�
	class PlayerController* m_PlayerController;

	// �� ���� �޽� ������
	MeshRenderer* m_HealPotionRenderer;

	SimpleMath::Matrix* m_HealPotionParent;
	SimpleMath::Vector3 m_TargetScale;
	SimpleMath::Quaternion m_TargetQuaternion;
	SimpleMath::Vector3 m_TargetTranslation;

};

