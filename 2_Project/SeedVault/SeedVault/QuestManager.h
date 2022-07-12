#pragma once
#include "ComponentBase.h"

class QuestManager : public ComponentBase
{
public:
	QuestManager();
	~QuestManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override {};

	void SetTriggerPass();

	unsigned int* m_pNowTrigger;		// 현재 트리거
	std::vector<struct QuestBlock> m_QuestText_V;

private:
	Text* m_MainText;
	Text* m_SubText;

	unsigned int m_CurMainTriggerIdx = 0;
	unsigned int m_CurSubTriggerIdx = 0;

	//unsigned int m_CurStringIdx = 0;

	std::vector<bool> m_isTriggerPass_V;

};

