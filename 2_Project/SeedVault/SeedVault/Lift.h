#pragma once
#include "CInteraction.h"

/// <summary>
/// ������ ���� ����Ʈ�� ���� ��Ű�� ������Ʈ
/// 
/// �ۼ��� : �� �� ȯ
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

