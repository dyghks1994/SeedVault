#pragma once
#include "CInteraction.h"

/// <summary>
/// 발전실 씬의 사무실 문을 열기 위한 카드키를 대는 패드
/// 
/// 작성자 : 최 요 환
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
	bool m_bDoorOpen;		// 문 개폐 여부
	bool m_bPrevDoorOpen;	// 문 개폐 여부
};

