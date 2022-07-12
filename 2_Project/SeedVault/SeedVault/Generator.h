#pragma once
#include "CInteraction.h"

/// <summary>
/// 발전실 씬의 발전기를 가동 시키는 오브젝트
/// 
/// 작성자 : 최 요 환
/// </summary>
/// 
class SoundEvent;
class Generator : public CInteraction
{
public:
	Generator();
	virtual ~Generator();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	virtual void Run() override;

	void TurnOnGenRoomLight();
	void TurnOffGenRoomLight();

public:
	bool m_bGeneratorPowerOn;		// 발전기 가동 여부
private:
	SoundEvent* m_EgenSound;
};

