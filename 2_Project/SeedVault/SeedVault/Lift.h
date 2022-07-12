#pragma once
#include "CInteraction.h"

/// <summary>
/// 발전실 씬의 리프트를 가동 시키는 오브젝트
/// 
/// 작성자 : 최 요 환
/// </summary>
class Lift : public CInteraction
{
public:
	Lift();
	virtual ~Lift();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void Run() override;

public:
	class Generator* m_Generator;
	float m_time = 0.f;
};

