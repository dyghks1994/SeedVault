#include "pch.h"
#include "Door.h"

Door::Door()
	: m_bDoorOpen(false)
{
}

Door::~Door()
{
}

void Door::Start()
{
	CInteraction::Start();
	m_bDoorOpen = false;

	m_pMyObject->AddComponent<Audio>(new Audio);
}

void Door::Update(float dTime)
{
	if (m_bTutorialDone == true)
	{
		m_TimeCheck += dTime;

		if (m_TimeCheck >= 4.0f)
		{
			DLLEngine::PickScene("GenRoom");
		}
	}
}

void Door::OnRender()
{
}

/// <summary>
/// 플레이어가 문을 작동시키면 튜토리얼 씬 클리어 후 발전기 실로 이동
/// </summary>
void Door::Run()
{
	// 문 열기전
	if (m_bDoorOpen == false)
	{
	
		// 문 메쉬는 열리도록
		GameObject* _DoorFrame = DLLEngine::FindGameObjectByName("Door");
		assert(_DoorFrame != nullptr);

		_DoorFrame->GetComponent<TransformAnimation>()->Play();
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_DoorOpen");

		m_bTutorialDone = true;
	}
}
