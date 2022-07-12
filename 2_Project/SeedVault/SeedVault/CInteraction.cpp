#include "pch.h"
#include "Inventory.h"
#include "CInteraction.h"

CInteraction::CInteraction()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerInventory(nullptr)
	, isInterect(false)
{
}

CInteraction::~CInteraction()
{
}

void CInteraction::Start()
{
	// �÷��̾� ��Ʈ�ѷ� ����
	m_PlayerInventory = DLLEngine::FindGameObjectByName("Player")->GetComponent<Inventory>();
	assert(m_PlayerInventory != nullptr);
}

void CInteraction::Update(float dTime)
{
}

void CInteraction::OnRender()
{
}
