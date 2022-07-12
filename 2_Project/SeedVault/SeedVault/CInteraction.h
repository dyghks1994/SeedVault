#pragma once
#include "ComponentBase.h"

/// <summary>
/// 상호작용 가능한 오브젝트의 작동(사용)을 하는 컴포넌트
/// 
/// 작성자 : 최 요 환
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

