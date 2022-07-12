#include "pch.h"
#include "Inventory.h"
#include "ScriptTrigger.h"
#include "CLTestSceneYH.h"
#include "CardKeyPad.h"
#include "Audio.h"

CardKeyPad::CardKeyPad()
	: m_bDoorOpen(false)
{
}

CardKeyPad::~CardKeyPad()
{
}

void CardKeyPad::Start()
{
	CInteraction::Start();

	m_bDoorOpen = false;
	m_bPrevDoorOpen = false;
	m_pMyObject->AddComponent<Audio>(new Audio());
}

void CardKeyPad::Update(float dTime)
{
}

void CardKeyPad::OnRender()
{
}

/// <summary>
/// �÷��̾ ī��Ű�� ���� ���¿��� �۵���Ű�� ���� �繫�� ���� ����.
/// </summary>
void CardKeyPad::Run()
{
	// ���� ���� �ִ� ��쿡 
	if (m_bDoorOpen == false)
	{
		// �÷��̾ Ű�� ������ �ִ��� Ȯ��
		if (m_PlayerInventory->GetItemCount(IItem::Type::eCardKey) <= 0)
		{
			CA_TRACE("[CardKeyPad] ī��Ű�� ����..");

			return;
		}

		// �� �޽��� ��������
		GameObject* _DoorFrame = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_Side");
		assert(_DoorFrame != nullptr);

		_DoorFrame->GetComponent<TransformAnimation>()->Play();
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_Card");

		// ���� ������ ���� �ִ� �ݶ��̴��� ����
		GameObject* _DoorLeft = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_L");
		PhysicsActor* _DoorLeftPhysicsActor = _DoorLeft->GetComponent<PhysicsActor>();
		if (_DoorLeftPhysicsActor != nullptr)
		{
			_DoorLeftPhysicsActor->OnDisable();
		}

		// ��ũ��Ʈ Ʈ���� 6,7���� ���� ��Ȱ��ȭ, 8���� ���� Ȱ��ȭ
		ScriptTrigger* _scriptTrigger = DLLEngine::FindGameObjectByName("6")->GetComponent<ScriptTrigger>();
		if (_scriptTrigger != nullptr)
		{
			_scriptTrigger->m_bIsActivated = false;
		}

		_scriptTrigger = DLLEngine::FindGameObjectByName("7")->GetComponent<ScriptTrigger>();
		if (_scriptTrigger != nullptr)
		{
			_scriptTrigger->m_bIsActivated = false;
		}

		_scriptTrigger = DLLEngine::FindGameObjectByName("8")->GetComponent<ScriptTrigger>();
		if (_scriptTrigger != nullptr)
		{
			_scriptTrigger->m_bIsActivated = true;
		}

		CLTestSceneYH* _GenScene = dynamic_cast<CLTestSceneYH*>(DLLEngine::GetNowScene());
		if (_GenScene != nullptr)
		{
			_GenScene->m_TriggerNum = 7;
		}

		CA_TRACE("[CardKeyPad] Door Open!!");

		// ���� ���� ���� ��ȯ
		m_bDoorOpen = true;
	}
	if (m_bPrevDoorOpen == false&&m_bDoorOpen==true)
	{
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_DoorOpen");
		m_bPrevDoorOpen = true;
	}
	CA_TRACE("[CardKeyPad] already Door Opened...");
	
}
