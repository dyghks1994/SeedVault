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
	// 비활성화 되어 있으면 업데이트 하지 않음
	if (m_bIsActivated == false)
	{
		return;
	}

	/// 활성화 되어 있는 경우 트리거 체크
	// 플레이어와의 거리를 구해서 가까워지면 트리거 발동(스크립트 출력)
	float _dist = SimpleMath::Vector3::Distance(m_Transform->m_WorldPosition, m_Player->m_Transform->m_WorldPosition);
	if (_dist < m_TriggerRange)
	{
		CA_TRACE("[ScriptTrigger] 트리거 이름! -> %s", m_pMyObject->GetObjectName().c_str());

		// 씬의 트리거 번호 증가
		CLTutorial* _TutorialScene = dynamic_cast<CLTutorial*>(DLLEngine::GetNowScene());
		if (_TutorialScene != nullptr)
		{
			if (m_bIsTriggered == false)	
			{
				_TutorialScene->m_TriggerNum++;

				CA_TRACE("[ScriptTrigger] ! -> TriggerNum %u", _TutorialScene->m_TriggerNum);

				// nextTrigger 활성화
				if (m_NextTrigger != nullptr)
				{
					m_NextTrigger->m_bIsActivated = true;
				}

				if (_TutorialScene->m_TriggerNum == 7)
				{
					GameObject* _barrier = DLLEngine::FindGameObjectByName("Barrier");
					_barrier->GetComponent<PhysicsActor>()->OnDisable();
					CA_TRACE("[튜토리얼씬] 배리어 해제");
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

				// nextTrigger 활성화
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
/// 트리거 발동 체크 거리
/// </summary>
/// <param name="range">설정할 거리 값</param>
void ScriptTrigger::SetTriggerRange(const float range)
{
	m_TriggerRange = range;
}

void ScriptTrigger::SetNextTrigger(GameObject* nextTriggerObj)
{
	m_NextTrigger = nextTriggerObj->GetComponent<ScriptTrigger>();
}
