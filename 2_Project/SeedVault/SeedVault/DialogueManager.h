#pragma once
#include "ComponentBase.h"

class DialogueManager : public ComponentBase
{
public:
	DialogueManager();
	virtual ~DialogueManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void SetTriggerPass();


	unsigned int* m_pNowTrigger;		// 현재 트리거
	std::vector<struct DialogueBlock> m_DialogueText_V;

private:
	Text* m_pDialogueText;

	unsigned int m_CurTriggerIdx = 0;
	unsigned int m_CurStringIdx = 0;

	float m_time = 0;
	const float m_recessTime = 0.3f;

	std::vector<bool> m_isTriggerPass_V;

};

