#pragma once
#include "CInteraction.h"

/// <summary>
/// 튜토리얼 씬의 노트
/// 상호작용하면 노트(UI 이미지)가 보이게 된다.
/// 
/// 작성자 : 최 요 환
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

