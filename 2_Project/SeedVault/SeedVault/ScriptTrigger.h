#pragma once
#include "ComponentBase.h"

/// <summary>
/// 스크립트 출력용 트리거
/// </summary>
class ScriptTrigger : public ComponentBase
{
public:
	ScriptTrigger();
	~ScriptTrigger();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void SetTriggerRange(const float range);
	void SetNextTrigger(GameObject* nextTriggerObj);

public:
	GameObject* m_Player;	// 플레이어 오브젝트

	bool m_bIsActivated;
	float m_TriggerRange;	// 트리거 체크 범위
	bool m_bIsTriggered;	// 이미 트리거 발동 되었나?

	ScriptTrigger* m_NextTrigger;	// 활성화시킬 다음 트리거 오브젝트

};

