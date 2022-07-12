#pragma once
#include "CInteraction.h"

/// <summary>
/// 튜토리얼 씬의 마지막 문을 여는 상호작용 오브젝트
/// 
/// 작성자 : 최 요 환
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
	bool m_bDoorOpen;		// 문 개폐 여부
	bool m_bTutorialDone;
	float m_TimeCheck;
};

