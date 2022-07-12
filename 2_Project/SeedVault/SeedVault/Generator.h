#pragma once
#include "CInteraction.h"

/// <summary>
/// ������ ���� �����⸦ ���� ��Ű�� ������Ʈ
/// 
/// �ۼ��� : �� �� ȯ
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
	bool m_bGeneratorPowerOn;		// ������ ���� ����
private:
	SoundEvent* m_EgenSound;
};

