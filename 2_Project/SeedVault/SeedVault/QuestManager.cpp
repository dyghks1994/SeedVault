#include "pch.h"
#include "QuestManager.h"

QuestManager::QuestManager()
	: ComponentBase(ComponentType::GameLogic)
{
}

QuestManager::~QuestManager()
{
}

void QuestManager::Start()
{
	m_MainText = m_pMyObject->GetComponent<Text>();
	assert(m_MainText != nullptr);

	m_SubText = DLLEngine::FindGameObjectByName("Quest Text Sub")->GetComponent<Text>();
	assert(m_SubText != nullptr);
}

void QuestManager::Update(float dTime)
{
	/*
	// �����ε��� + 1 �� �ε����� ����� �ʴ��� �˻�
	if (m_CurMainTriggerIdx + 1 >= m_QuestText_V.size())
	{

	}
	else if (*m_pNowTrigger == m_QuestText_V[m_CurMainTriggerIdx + 1].triggerNum)
	{
		++m_CurMainTriggerIdx;
		m_CurSubTriggerIdx = 0;
	}

	if (m_CurSubTriggerIdx + 1 >= m_QuestText_V.size())
	{

	}
	else if (*m_pNowTrigger == m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V[m_CurSubTriggerIdx + 1].triggerNum)
	{
		++m_CurSubTriggerIdx;
	}
	*/

	// �����ε��� + 1 �� �ε����� ����� �ʴ��� �˻�
	if (m_CurMainTriggerIdx + 1 < m_QuestText_V.size())
	{
		if (*m_pNowTrigger == m_QuestText_V[m_CurMainTriggerIdx + 1].triggerNum)
		{
			++m_CurMainTriggerIdx;
			m_CurSubTriggerIdx = 0;
		}
	}

	if (m_CurSubTriggerIdx + 1 < m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V.size())
	{
		if (*m_pNowTrigger == m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V[m_CurSubTriggerIdx + 1].triggerNum)
		{
			++m_CurSubTriggerIdx;
		}
	}

	m_MainText->PrintSpriteText((TCHAR*)m_QuestText_V[m_CurMainTriggerIdx].MainQuest.c_str());
	m_SubText->PrintSpriteText((TCHAR*)m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V[m_CurSubTriggerIdx].SubQuest.c_str());

	//////////////////////////////////////////////////////////////////

	//if (m_CurMainTriggerIdx + 1 == m_QuestText_V.size())
	//{
	//	m_MainText->PrintSpriteText((TCHAR*)m_QuestText_V[m_CurMainTriggerIdx].MainQuest.c_str());
	//
	//}
	//else if (*m_pNowTrigger < m_QuestText_V[m_CurMainTriggerIdx + 1].triggerNum)
	//{
	//	//�۾��� ����.
	//	m_MainText->PrintSpriteText((TCHAR*)m_QuestText_V[m_CurMainTriggerIdx].MainQuest.c_str());
	//	
	//	if (*m_pNowTrigger < m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V[m_CurSubTriggerIdx + 1].triggerNum)
	//	{
	//		m_SubText->PrintSpriteText((TCHAR*)m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V[m_CurSubTriggerIdx].SubQuest.c_str());
	//	}
	//
	//
	//}
	//
	//if (*m_pNowTrigger > m_QuestText_V[m_CurMainTriggerIdx].triggerNum)
	//{
	//	++m_CurMainTriggerIdx; 
	//	m_CurSubTriggerIdx = 0;
	//}
	//
	//if (*m_pNowTrigger > m_QuestText_V[m_CurMainTriggerIdx].SubQuest_V[m_CurSubTriggerIdx].triggerNum)
	//{
	//	++m_CurSubTriggerIdx;
	//}

	// ������ ���
	//if (*m_pNowTrigger > m_QuestText_V[m_CurTriggerIdx].triggerNum)
	//{
	//	m_isTriggerPass_V[m_CurTriggerIdx] == true;
	//	++m_CurTriggerIdx;
	//	m_CurStringIdx = 0;
	//
	//	// �ε����� ��纤�ͺ��� Ŀ���� 0���� �ٲ������
	//	if (m_CurTriggerIdx >= m_QuestText_V.size())
	//	{
	//		m_CurTriggerIdx = 0;
	//	}
	//}

}

void QuestManager::SetTriggerPass()
{
	m_isTriggerPass_V.resize(m_QuestText_V.size(), false);
}
