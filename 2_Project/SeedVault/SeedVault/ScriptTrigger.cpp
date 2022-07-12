#include "pch.h"
#include "CLTutorial.h"
#include "CLTestSceneYH.h"
#include "ScriptTrigger.h"

ScriptTrigger::ScriptTrigger()
	: ComponentBase(ComponentType::GameLogic)
	, m_bIsActivated(false)
	, m_Player(nullptr)
	, m_TriggerRange(1.0f)
	, m_bIsTriggered(false)
	, m_NextTrigger(nullptr)
{

}

ScriptTrigger::~ScriptTrigger()
{
}

void ScriptTrigger::Start()
{
	m_Player = DLLEngine::FindGameObjectByName("Player");
	assert(m_Player != nullptr);

}

void ScriptTrigger::Update(float dTime)
{
	// ��Ȱ��ȭ �Ǿ� ������ ������Ʈ ���� ����
	if (m_bIsActivated == false)
	{
		return;
	}

	/// Ȱ��ȭ �Ǿ� �ִ� ��� Ʈ���� üũ
	// �÷��̾���� �Ÿ��� ���ؼ� ��������� Ʈ���� �ߵ�(��ũ��Ʈ ���)
	float _dist = SimpleMath::Vector3::Distance(m_Transform->m_WorldPosition, m_Player->m_Transform->m_WorldPosition);
	if (_dist < m_TriggerRange)
	{
		CA_TRACE("[ScriptTrigger] Ʈ���� �̸�! -> %s", m_pMyObject->GetObjectName().c_str());

		// ���� Ʈ���� ��ȣ ����
		CLTutorial* _TutorialScene = dynamic_cast<CLTutorial*>(DLLEngine::GetNowScene());
		if (_TutorialScene != nullptr)
		{
			if (m_bIsTriggered == false)	
			{
				_TutorialScene->m_TriggerNum++;

				CA_TRACE("[ScriptTrigger] ! -> TriggerNum %u", _TutorialScene->m_TriggerNum);

				// nextTrigger Ȱ��ȭ
				if (m_NextTrigger != nullptr)
				{
					m_NextTrigger->m_bIsActivated = true;
				}

				if (_TutorialScene->m_TriggerNum == 7)
				{
					GameObject* _barrier = DLLEngine::FindGameObjectByName("Barrier");
					_barrier->GetComponent<PhysicsActor>()->OnDisable();
					CA_TRACE("[Ʃ�丮���] �踮�� ����");
				}

			}
			
		}
		
		CLTestSceneYH* _GenScene = dynamic_cast<CLTestSceneYH*>(DLLEngine::GetNowScene());
		if (_GenScene != nullptr)
		{
			if (m_bIsTriggered == false)
			{
				_GenScene->m_TriggerNum++;

				CA_TRACE("[ScriptTrigger] ! -> TriggerNum %u", _GenScene->m_TriggerNum);

				// nextTrigger Ȱ��ȭ
				if (m_NextTrigger != nullptr)
				{
					m_NextTrigger->m_bIsActivated = true;
				}
			}
		}

		m_bIsTriggered = true;
		m_bIsActivated = false;
	}
}

void ScriptTrigger::OnRender()
{
}

/// <summary>
/// Ʈ���� �ߵ� üũ �Ÿ�
/// </summary>
/// <param name="range">������ �Ÿ� ��</param>
void ScriptTrigger::SetTriggerRange(const float range)
{
	m_TriggerRange = range;
}

void ScriptTrigger::SetNextTrigger(GameObject* nextTriggerObj)
{
	m_NextTrigger = nextTriggerObj->GetComponent<ScriptTrigger>();
}
