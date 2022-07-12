#pragma once
#include "Scene.h"

class CLTutorial : public Scene
{
public:
	CLTutorial();
	virtual ~CLTutorial();
	CLTutorial(const CLTutorial& other) = default;

	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

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
	GameObject* m_RightHand;

	GameObject* m_FreeCamera;

	GameObject* m_pPartner;

	GameObject* m_pTestCube_1;
	GameObject* m_pTestCube_2;

	GameObject* m_pZombie_Runner_1;
	GameObject* m_pZombie_Runner_2;

	GameObject* m_pZombie_Clicker_1;

	/// UI Object
	GameObject* m_InGameUI;
	GameObject* m_PauseUI;

	// Test
	GameObject* m_TestCube;
	GameObject* m_Decal;
	GameObject* m_DeadScreen;

	bool m_BGM_Playing;

	bool m_IsSceneRun;
	float m_DieTime;

public:
	unsigned int m_TriggerNum = 0;
	unsigned int m_PrevTriggerNum = 0;

};

