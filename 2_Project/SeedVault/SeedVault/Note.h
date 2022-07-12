#pragma once
#include "CInteraction.h"

/// <summary>
/// Ʃ�丮�� ���� ��Ʈ
/// ��ȣ�ۿ��ϸ� ��Ʈ(UI �̹���)�� ���̰� �ȴ�.
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class Note : public CInteraction 
{
public:
	Note();
	virtual ~Note();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void Run() override;

public:
	GameObject* m_Document;
};

