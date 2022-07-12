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
	// �ڽ��� ������Ʈ���� Text�� �����´�
	m_pDialogueText = m_pMyObject->GetComponent<Text>();
	assert(m_pDialogueText != nullptr);
}

void DialogueManager::Update(float dTime)
{
	// Ʈ���Ű� ������ && ���� ����簡 ��������� ������
	if (*m_pNowTrigger == m_DialogueText_V[m_CurTriggerIdx].triggerNum && m_isTriggerPass_V[m_CurTriggerIdx] == false)
	{
		//�۾��� ����.
		m_pDialogueText->PrintSpriteText((TCHAR*)m_DialogueText_V[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].dialogue.c_str());

		// �ð��� ����.
		m_time += dTime;
	
		// �ð��� �������ͺ��� �������� ���� �ε����� �ѱ��		
		if (m_time > m_DialogueText_V[m_CurTriggerIdx].dialogueString_V[m_CurStringIdx].time + m_recessTime)
		{
			// ��Ʈ�� �ε����� �ø��µ�, 
			++m_CurStringIdx;
			unsigned int _Idx = m_DialogueText_V[m_CurTriggerIdx].dialogueString_V.size();

			//��Ʈ�� �ε����� �������ִ� ��Ʈ������ �������� 
			if (m_CurStringIdx >= _Idx)
			{
				// �� Ʈ������ ��Ʈ������ �����Ŵ�(Ʈ���� ������)
				m_isTriggerPass_V[m_CurTriggerIdx] = true;
				++m_CurTriggerIdx;
				m_CurStringIdx = 0;
				
				// �ε����� ��纤�ͺ��� Ŀ���� 0���� �ٲ������
				if (m_CurTriggerIdx >= m_DialogueText_V.size())
				{
					m_CurTriggerIdx = 0;
				}
			}

			m_time = 0;
		}
	}

	// ������ ���
	if (*m_pNowTrigger > m_DialogueText_V[m_CurTriggerIdx].triggerNum)
	{
		//unsigned int a = m_CurTriggerIdx;
		//unsigned int b = m_CurTriggerIdx + 1;
		//CA_TRACE("����!! �ε��� %d���� %d �� �Ѿ, Ʈ���� : %d,���� �ۿ��� �Ѿ Ʈ���� : %d", a, b, m_DialogueText_V[b].triggerNum, *m_pNowTrigger);

		m_isTriggerPass_V[m_CurTriggerIdx] == true;
		++m_CurTriggerIdx;
		m_CurStringIdx = 0;

		// �ε����� ��纤�ͺ��� Ŀ���� 0���� �ٲ������
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
