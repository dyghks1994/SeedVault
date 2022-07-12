#include "pch.h"
#include "PlayerController.h"
#include "LeftHandController.h"

LeftHandController::LeftHandController()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerMesh(nullptr)
	, m_PlayerController(nullptr)
	, m_HealPotionRenderer(nullptr)
	, m_HealPotionParent()
{
}

LeftHandController::~LeftHandController()
{
}

void LeftHandController::Start()
{
	// �θ� Ʈ������(������) ��ġ ����
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_HealPotionParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// �÷��̾� ��Ʈ�ѷ� ����
	m_PlayerController = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>();
	assert(m_PlayerController != nullptr);

	m_HealPotionRenderer = DLLEngine::FindGameObjectByName("HealPotionMesh")->GetComponent<MeshRenderer>();
	assert(m_HealPotionRenderer != nullptr);
}

void LeftHandController::Update(float dTime)
{
	// �÷��̾ �׾������� �������� ����.
	if (m_PlayerController->m_bIsDead == true)
	{
		return;
	}

	// �θ� Ʈ������(������) ��ġ ����
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_HealPotionParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("LeftHand"));

	// Ÿ�� ������Ʈ�� Ʈ������ ����d�� ���Ѵ�
	m_HealPotionParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// Ÿ���� �������� myObject�� Ʈ������ ����
	m_Transform->SetPosition(m_TargetTranslation);
	m_Transform->SetRotation(m_TargetQuaternion);
	m_Transform->SetScale(m_TargetScale);

	if (m_PlayerController->m_bIsHeal == true)
	{
		m_HealPotionRenderer->SetEnable(true);
	}
	else
	{
		m_HealPotionRenderer->SetEnable(false);
	}

}

void LeftHandController::OnRender()
{
}
