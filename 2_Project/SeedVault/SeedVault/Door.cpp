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
/// �÷��̾ ���� �۵���Ű�� Ʃ�丮�� �� Ŭ���� �� ������ �Ƿ� �̵�
/// </summary>
void Door::Run()
{
	// �� ������
	if (m_bDoorOpen == false)
	{
	
		// �� �޽��� ��������
		GameObject* _DoorFrame = DLLEngine::FindGameObjectByName("Door");
		assert(_DoorFrame != nullptr);

		_DoorFrame->GetComponent<TransformAnimation>()->Play();
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_DoorOpen");

		m_bTutorialDone = true;
	}
}
