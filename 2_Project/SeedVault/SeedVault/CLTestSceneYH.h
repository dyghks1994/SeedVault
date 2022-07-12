#pragma once

/// <summary>
/// 테스트 씬
/// 엔진의 IScene 를 상속 받아 만들자
/// 
/// 일단 오브젝트 하나 생성하고 렌더링하고 움직이는 내용을 구현해야 한다.
/// 
/// 작성자 : 최 요 환
/// </summary>

#include "Scene.h"

class CLTestSceneYH final : public Scene
{
public:
	CLTestSceneYH();
	virtual ~CLTestSceneYH();
	CLTestSceneYH(const CLTestSceneYH& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;
	//virtual void Render() override;

public:
	void SpawnZombie();
	void SpawnItem();
	void SpawnInteractionObj();
	void SpawnScriptTrigger();
	void SpawnDecal();

private:

	GameObject* m_pGameManager;

	GameObject* m_pAxis;
	
	GameObject* m_pNavMeshObject_1;
	GameObject* m_pNavMeshObject_2;

	GameObject* m_pGround;

	GameObject* m_pPlayer;
	GameObject* m_LeftHand;
	GameObject* m_RightHand;

	GameObject* m_FreeCamera;

	GameObject* m_pPartner;

	GameObject* m_pTestCube_1;
	GameObject* m_pTestCube_2;

	GameObject* m_pZombie_Runner_1;
	GameObject* m_pZombie_Runner_2;
	GameObject* m_pZombie_Runner_3;
	GameObject* m_pZombie_Runner_4;
	GameObject* m_pZombie_Runner_5;

	GameObject* m_pZombie_Clicker_1;
	GameObject* m_pZombie_Clicker_2;

	/// UI Object
	GameObject* m_InGameUI;
	GameObject* m_PauseUI;

	// Test
	GameObject* m_TestCube;
	GameObject* m_Decal;
	GameObject* m_DeadScreen;


	bool m_bBGM_Playing;
	GameObject* m_pEgen;

	bool m_IsSceneRun;
	float m_DieTime;
public:
	unsigned int m_TriggerNum = 0;
	unsigned int m_PrevTriggerNum = 0;

};

