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
	// 부모 트랜스폼(오른손) 위치 연결
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_HealPotionParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// 플레이어 컨트롤러 연결
	m_PlayerController = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>();
	assert(m_PlayerController != nullptr);

	m_HealPotionRenderer = DLLEngine::FindGameObjectByName("HealPotionMesh")->GetComponent<MeshRenderer>();
	assert(m_HealPotionRenderer != nullptr);
}

void LeftHandController::Update(float dTime)
{
	// 플레이어가 죽어있으면 갱신하지 않음.
	if (m_PlayerController->m_bIsDead == true)
	{
		return;
	}

	// 부모 트랜스폼(오른손) 위치 연결
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_HealPotionParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("LeftHand"));

	// 타겟 오브젝트의 트랜스폼 성분d을 구한다
	m_HealPotionParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// 타겟의 성분으로 myObject의 트랜스폼 구성
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
