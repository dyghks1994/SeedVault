#pragma once
#include "ComponentBase.h"

/// <summary>
/// ��ũ��Ʈ ��¿� Ʈ����
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
	GameObject* m_Player;	// �÷��̾� ������Ʈ

	bool m_bIsActivated;
	float m_TriggerRange;	// Ʈ���� üũ ����
	bool m_bIsTriggered;	// �̹� Ʈ���� �ߵ� �Ǿ���?

	ScriptTrigger* m_NextTrigger;	// Ȱ��ȭ��ų ���� Ʈ���� ������Ʈ

};

