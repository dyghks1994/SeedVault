#pragma once
#include "CInteraction.h"

/// <summary>
/// Ʃ�丮�� ���� ������ ���� ���� ��ȣ�ۿ� ������Ʈ
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class Door : public CInteraction
{
public:
	Door();
	~Door();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void Run() override;

public:
	bool m_bDoorOpen;		// �� ���� ����
	bool m_bTutorialDone;
	float m_TimeCheck;
};

