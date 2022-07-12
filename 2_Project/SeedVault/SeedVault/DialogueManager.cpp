#include "pch.h"
#include "DialogueManager.h"
#include <string>

#include "EffectUIEnum.h"

DialogueManager::DialogueManager()
	: ComponentBase(ComponentType::GameLogic)
{
}

DialogueManager::~DialogueManager()
{
}

void DialogueManager::Start()
{
	// 자신의 오브젝트에서 Text를 가져온다
	m_pDialogueText = m_pMyObject->GetComponent<Text>();
	assert(m_pDialogueText != nullptr);
}

void DialogueManager::Update(float dTime)
{
	// 트리거가 같을떄 && 아직 씬대사가 진행된적이 없으면
	if (*m_pNowTrigger == m_DialogueText_V[m_CurTriggerIdx].triggerNum && m_isTriggerPass_V[m_CurTriggerIdx] == false)
	{
		//글씨를 띄운다.
		m_pDialogueText->PrintSpriteText((TCHAR*)m_DialogueText_V[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].dialogue.c_str());

		// 시간이 간다.
		m_time += dTime;
	
		// 시간이 정해진것보다 많아지면 다음 인덱스로 넘긴다		
		if (m_time > m_DialogueText_V[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].time + m_recessTime)
		{
			// 스트링 인덱스를 늘리는데, 
			++m_CurStringIdx;
			unsigned int _Idx = m_DialogueText_V[m_CurTriggerIdx].dialogueString_V.size();

			//스트링 인덱스가 가지고있는 스트링보다 많아지면 
			if (m_CurStringIdx >= _Idx)
			{
				// 그 트리거의 스트링들은 끝난거다(트리거 지나감)
				m_isTriggerPass_V[m_CurTriggerIdx] = true;
				++m_CurTriggerIdx;
				m_CurStringIdx = 0;
				
				// 인덱스가 대사벡터보다 커지면 0으로 바꿔버린다
				if (m_CurTriggerIdx >= m_DialogueText_V.size())
				{
					m_CurTriggerIdx = 0;
				}
			}

			m_time = 0;
		}
	}

	// 씹혔을 경우
	if (*m_pNowTrigger > m_DialogueText_V[m_CurTriggerIdx].triggerNum)
	{
		//unsigned int a = m_CurTriggerIdx;
		//unsigned int b = m_CurTriggerIdx + 1;
		//CA_TRACE("씹힘!! 인덱스 %d에서 %d 로 넘어감, 트리거 : %d,현재 밖에서 넘어간 트리거 : %d", a, b, m_DialogueText_V[b].triggerNum, *m_pNowTrigger);

		m_isTriggerPass_V[m_CurTriggerIdx] == true;
		++m_CurTriggerIdx;
		m_CurStringIdx = 0;

		// 인덱스가 대사벡터보다 커지면 0으로 바꿔버린다
		if (m_CurTriggerIdx >= m_DialogueText_V.size())
		{
			m_CurTriggerIdx = 0;
		}
	}
}

void DialogueManager::OnRender()
{

}

void DialogueManager::SetTriggerPass()
{
	m_isTriggerPass_V.resize(m_DialogueText_V.size(), false);
}
