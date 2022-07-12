#pragma once
#include "CInteraction.h"

/// <summary>
/// ������ ���� �繫�� ���� ���� ���� ī��Ű�� ��� �е�
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class CardKeyPad : public CInteraction
{
public:
	CardKeyPad();
	~CardKeyPad();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void Run() override;

public:
	bool m_bDoorOpen;		// �� ���� ����
	bool m_bPrevDoorOpen;	// �� ���� ����
};

