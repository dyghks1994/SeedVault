#pragma once
#include "ComponentBase.h"

/// <summary>
/// ��ȣ�ۿ� ������ ������Ʈ�� �۵�(���)�� �ϴ� ������Ʈ
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class CInteraction : public ComponentBase
{
public:
	CInteraction();
	~CInteraction();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void Run() abstract;

	bool isInterect = false;

protected:

	class Inventory* m_PlayerInventory;
};

