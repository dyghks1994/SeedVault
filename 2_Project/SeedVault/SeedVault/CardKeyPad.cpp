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
/// 플레이어가 카드키를 가진 상태에서 작동시키면 옆에 사무실 물을 연다.
/// </summary>
void CardKeyPad::Run()
{
	// 문이 닫혀 있는 경우에 
	if (m_bDoorOpen == false)
	{
		// 플레이어가 키를 가지고 있는지 확인
		if (m_PlayerInventory->GetItemCount(IItem::Type::eCardKey) <= 0)
		{
			CA_TRACE("[CardKeyPad] 카드키가 없다..");

			return;
		}

		// 문 메쉬는 열리도록
		GameObject* _DoorFrame = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_Side");
		assert(_DoorFrame != nullptr);

		_DoorFrame->GetComponent<TransformAnimation>()->Play();
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_Card");

		// 물리 적으로 막고 있는 콜라이더는 해제
		GameObject* _DoorLeft = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_L");
		PhysicsActor* _DoorLeftPhysicsActor = _DoorLeft->GetComponent<PhysicsActor>();
		if (_DoorLeftPhysicsActor != nullptr)
		{
			_DoorLeftPhysicsActor->OnDisable();
		}

		// 스크립트 트리거 6,7번은 강제 비활성화, 8번은 강제 활성화
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

		// 열고 나서 상태 전환
		m_bDoorOpen = true;
	}
	if (m_bPrevDoorOpen == false&&m_bDoorOpen==true)
	{
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_DoorOpen");
		m_bPrevDoorOpen = true;
	}
	CA_TRACE("[CardKeyPad] already Door Opened...");
	
}
