#include "pch.h"
#include "GameManager.h"
#include "MuzzleFlash.h"
#include "EquipmentController.h"
#include "Health.h"
#include "Gun.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "CameraController.h"
#include "CameraBehavior.h"
#include "OrbitCamera.h"
#include "Zombie_Runner_Move.h"
#include "Zombie_Runner_AI.h"
#include "Zombie_Clicker_Move.h"
#include "Zombie_Clicker_AI.h"
#include "NavMeshAgent.h"
#include "ObjectBuilder.h"
#include "CardKeyPad.h"
#include "Generator.h"
#include "Lift.h"
#include "ScriptTrigger.h"
#include "DialogueManager.h"
#include "QuestManager.h"

#include "CLTestSceneYH.h"


CLTestSceneYH::CLTestSceneYH()
	: m_pGameManager(nullptr)
	, m_pAxis(nullptr)

	, m_pNavMeshObject_1(nullptr)
	, m_pNavMeshObject_2(nullptr)

	, m_pGround(nullptr)

	, m_pPlayer(nullptr)
	, m_LeftHand(nullptr)
	, m_RightHand(nullptr)

	, m_FreeCamera(nullptr)

	, m_pPartner(nullptr)

	, m_pTestCube_1(nullptr)
	, m_pTestCube_2(nullptr)

	, m_pZombie_Runner_1(nullptr)
	, m_pZombie_Runner_2(nullptr)
	, m_pZombie_Runner_3(nullptr)
	, m_pZombie_Runner_4(nullptr)
	, m_pZombie_Runner_5(nullptr)

	, m_pZombie_Clicker_1(nullptr)
	, m_pZombie_Clicker_2(nullptr)

	/// UI
	, m_InGameUI(nullptr)
	, m_PauseUI(nullptr)

	, m_TestCube(nullptr)
	, m_bBGM_Playing(false)
	, m_IsSceneRun(true)
	, m_TriggerNum(0)
{
}

CLTestSceneYH::~CLTestSceneYH()
{
}

void CLTestSceneYH::Initialize()
{
	m_TriggerNum = 0;
	m_PrevTriggerNum = 0;

	m_bBGM_Playing = false;
	/// <summary>
	///  배치된 씬파일 로드
	/// </summary>
	Scene::Initialize();

	/// <summary>
	/// 처음 네비메쉬를 생성하는 부분
	/// </summary>
	DLLNavMesh::AddNavMeshByResourceName("navmesh_009", "navmesh_009.bin");
	DLLNavMesh::SetCurrentNavMeshByName("navmesh_009");

	m_sceneKind = eScene::Generator;

	/// GameManager
	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());
	m_pGameManager->AddComponent<Audio>(new Audio());
	////////////////////////////////////////////////////////////////////

	/// Ground Cube
	{
		m_pGround = DLLEngine::CreateObject(this);
		m_pGround->SetObjectName("Ground");

		m_pGround->m_Transform->SetPosition({ 0.0f, -10.0f, 0.0f });
		m_pGround->m_Transform->SetScale({ 10.0f, 0.1f, 10.0f });

		MeshFilter* m_TestCubeMesh = new MeshFilter();
		m_TestCubeMesh->SetMesh("box_size_1.bin");
		m_pGround->AddComponent<MeshFilter>(m_TestCubeMesh);

		//MeshRenderer
		//m_pGround->AddComponent<MeshRenderer>(new MeshRenderer);
		//m_pGround->GetComponent<MeshRenderer>()->SetGizmo(false);

		PhysicsActor* _GroundPhysActor = new PhysicsActor({ 50.0f, 1.0f, 50.0f }, RigidType::Dynamic);
		_GroundPhysActor->SetGravity(false);
		_GroundPhysActor->SetKinematic(true);
		m_pGround->AddComponent(_GroundPhysActor);
		//_GroundPhysActor->SetFreezePosition(false, true, false);
	}
	////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////

	/// 플레이어 오브젝트
	m_pPlayer = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePlayer, &m_IsSceneRun);
	m_pPlayer->m_Transform->SetPosition({ -14.673, 1.741f, 2.002f });

	Inventory* _Inventory = m_pPlayer->GetComponent<Inventory>();
	_Inventory->SetItemCount(IItem::Type::eLongGunAmmo, 6);
	_Inventory->SetItemCount(IItem::Type::eHandGunAmmo, 12);

	/// 플레이어 오른손 매쉬 손 위치에 부착할 오브젝트
	m_LeftHand = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLeftHand, &m_IsSceneRun);

	/// 플레이어 오른손 매쉬 손 위치에 부착할 오브젝트
	m_RightHand = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eRightHand, &m_IsSceneRun);

	/// 프리 컨트롤 카메라 (카메라 전환 테스트)
	m_FreeCamera = DLLEngine::CreateObject(this);
	m_FreeCamera->SetObjectName("FreeCamera");

	m_FreeCamera->AddComponent<Camera>(new Camera(m_FreeCamera->GetObjectName()));

	GameObject* _MainCamera = DLLEngine::FindGameObjectByName("MainCamera");
	m_FreeCamera->m_Transform->SetPosition(_MainCamera->m_Transform->GetWorldPosition());

	m_FreeCamera->AddComponent<CameraController>(new CameraController());
	///카메라에 오디오리스너를 붙여준다.
	_MainCamera->AddComponent<AudioListener>(new AudioListener());
	//DLLEngine::SetNowCamera("SubCamera");
	/////////////////////////////////////////////////////////////////////

	/// 파트너 오브젝트
	m_pPartner = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePartner, &m_IsSceneRun);
	m_pPartner->m_Transform->SetPosition({ -14.735f, 1.741f, 2.002f });
	/////////////////////////////////////////////////////////////////////


	/////////////////////////////////////////////////////////////////////

	/// UI
	m_InGameUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eInGameUI, &m_IsSceneRun);
	m_PauseUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePauseUI, &m_IsSceneRun);
	DLLEngine::FindGameObjectByName("Dialogue Manager")->GetComponent<DialogueManager>()->m_pNowTrigger = &m_TriggerNum;
	DLLEngine::FindGameObjectByName("Quest Text")->GetComponent<QuestManager>()->m_pNowTrigger = &m_TriggerNum;


	/// 좀비 배치
	SpawnZombie();

	/// 아이템 배치
	SpawnItem();

	/// 상호작용 오브젝트 배치
	SpawnInteractionObj();

	/// 트리거 오브젝트 배치
	SpawnScriptTrigger();

	SpawnDecal();

	/// 서류 스프라이트
	m_DeadScreen = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	m_DeadScreen->SetObjectName("DeadScreen");

	Sprite2D* _deadScreenSptire = new Sprite2D;
	_deadScreenSptire->SetType(eResourceType::eSingleImage);
	_deadScreenSptire->m_SpriteName_V.push_back("DeadScreen.png");
	_deadScreenSptire->SetProportion(1.0f, 1.0f);
	_deadScreenSptire->SetPivot(0.5f, 0.5f);
	_deadScreenSptire->SetUIAxis(eUIAxis::BackGround);
	_deadScreenSptire->OnDisable();

	m_DeadScreen->AddComponent<Sprite2D>(_deadScreenSptire);

	/////////////////////////////////////////////////////////////////////
	//m_TestCube = DLLEngine::CreateObject(this);
	//m_TestCube->AddComponent<PhysicsActor>(new PhysicsActor({ 1.0f, 1.0f, 1.0f }, RigidType::Dynamic));
	//m_TestCube->GetComponent<PhysicsActor>()->SetKinematic(true);

	//m_pEgen = DLLEngine::FindGameObjectByName("Gen_Egen");
	//m_pEgen->AddComponent<Audio>(new Audio());
}


void CLTestSceneYH::Update(float dTime)
{
	if (!m_bBGM_Playing)
	{
		m_pGameManager->GetComponent<Audio>()->PlayEvent("event:/AMB & BGM/BGM_Gen");
		m_bBGM_Playing = true;
	}


	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F11)))
	{
		//DLLNavMesh::SetCurrentNavMeshByIndex(0);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F12)))
	{
		//DLLNavMesh::SetCurrentNavMeshByIndex(1);
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_F8)))
	{
		/// 임시로 트리거 넘기는 역할
		++m_TriggerNum;
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)))
	{
		m_IsSceneRun = !m_IsSceneRun;

		/// 마우스만 움직이게 만들어야함
		//m_pGameManager->s_bTPSMode = !s_bTPSMode;
	}

	auto _pos = m_Decal->GetTransform()->GetWorldPosition();

	float _nowSpeed = 1.0f;
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_LSHIFT)))
	{
		_nowSpeed = 10.f;
	}

	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_I)))
	{
		_pos.z += _nowSpeed * dTime;
	}
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_K)))
	{
		_pos.z += _nowSpeed * -dTime;
	}
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_J)))
	{
		_pos.x += _nowSpeed * -dTime;
	}
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_L)))
	{
		_pos.x += _nowSpeed * dTime;
	}
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_9)))
	{
		_pos.y += _nowSpeed * dTime;
	}
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_0)))
	{
		_pos.y += _nowSpeed * -dTime;
	}




	//DirectX::SimpleMath::Matrix::CreateTranslation(_pos);

	m_Decal->GetTransform()->SetPosition(_pos);


	//트리거가 바뀌는 시점
	if (m_PrevTriggerNum!=m_TriggerNum)
	{
		std::string _triggerNum = std::to_string(m_TriggerNum);
		std::string _base = "00";

		for (int i = 0; i < _triggerNum.length(); i++)
		{
			_base[(int)(_base.length() - _triggerNum.length() + i)] = _triggerNum[i];
		}
	
		std::string _GT_M = "event:/Dialog/GT/GT_" + _base + "M";
		std::string _GT_A = "event:/Dialog/GT/GT_" + _base + "A";

		m_pPlayer->GetComponent<Audio>()->AllEventStop();
		m_pPartner->GetComponent<Audio>()->AllEventStop();

		m_pPlayer->GetComponent<Audio>()->PlayEvent(_GT_M.c_str(), false);
		m_pPartner->GetComponent<Audio>()->PlayEvent(_GT_A.c_str(), false);
	}

	m_PrevTriggerNum = m_TriggerNum;

	if (m_pPlayer->GetComponent<Health>()->IsDead() == true || m_pPartner->GetComponent<Health>()->IsDead() == true)
	{
		m_DieTime += dTime;
		m_DeadScreen->GetComponent<Sprite2D>()->OnEnable();
	}

	if (m_DieTime > 1.5f)
	{
		if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::KEY_TAB)))
		{
			m_DieTime = 0;
			m_DeadScreen->GetComponent<Sprite2D>()->OnDisable();
			DLLEngine::PickScene("GenRoom");
		}
	}
}

void CLTestSceneYH::Reset()
{
}	

void CLTestSceneYH::SpawnZombie()
{
	
	/// 러너 1
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ 10.513f, 1.1f, 5.519f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ 7.820f, 1.099f, 0.901f });

		m_pZombie_Runner_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner, &m_IsSceneRun);

		m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);


		m_pZombie_Runner_1->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
		//m_pZombie_Runner_1->m_Transform->SetRotationFromVec({ 0.0f, 127.649f, 0.0f });
	}
	/////////////////////////////////////////////////////////////////////

	/// 러너 2
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ 7.503f, 1.1f, 4.472f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ 8.871f, 1.1f, -1.063f });

		GameObject* _pWayPoint_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_2->m_Transform->SetPosition({ 9.82904f, 1.1f, 0.479f });

		m_pZombie_Runner_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner, &m_IsSceneRun);

		m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_2->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);

		m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);
		m_pZombie_Runner_2->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_2);

	}
	
	/// 러너 3
	{
		// SpawnPoint

		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ -2.681f, 0.41f, -4.445f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ 1.950f, 0.0f, -4.146f });

		m_pZombie_Runner_3 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner, &m_IsSceneRun);
		m_pZombie_Runner_3->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_3->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);

		m_pZombie_Runner_3->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);

	}

	
	/// 러너 4
	{
		// SpawnPoint

		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ -6.061f, 0.489f, -3.115f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ -7.028f, 0.489f, -5.562f });


		m_pZombie_Runner_4 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner, &m_IsSceneRun);
		m_pZombie_Runner_4->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_4->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);

		m_pZombie_Runner_4->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
	}

	/// 러너 5
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ -8.989f, 0.0f, -3.117f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ -7.881f, 0.489f, -5.349f });

		m_pZombie_Runner_5 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner, &m_IsSceneRun);

		m_pZombie_Runner_5->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Runner_5->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pWayPoint_1);

		m_pZombie_Runner_5->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
		m_pZombie_Runner_5->m_Transform->SetRotationFromVec({ 0.0f, 127.649f, 0.0f });
	}
	/////////////////////////////////////////////////////////////////////
	

	/// 클리커 1
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ 10.326f, 1.100f, -4.445f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ 10.21f, 1.1f, -1.918f });

		GameObject* _pWayPoint_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_2->m_Transform->SetPosition({ 7.869f, 1.1f, -1.238f });

		m_pZombie_Clicker_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Clicker, &m_IsSceneRun);

		m_pZombie_Clicker_1->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Clicker_1->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);

		m_pZombie_Clicker_1->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pWayPoint_1);
		m_pZombie_Clicker_1->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pWayPoint_2);

	}

	
	/// 클리커 2
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ -2.917f, 0.410f, 1.670f });

		GameObject* _pWayPoint_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_1->m_Transform->SetPosition({ 1.301f, 0.410f, -2.552f });

		GameObject* _pWayPoint_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pWayPoint_2->m_Transform->SetPosition({ 2.290f, 0.410f, 1.167f });

		m_pZombie_Clicker_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Clicker, &m_IsSceneRun);
		m_pZombie_Clicker_2->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pSpawnPoint);
		m_pZombie_Clicker_2->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);

		m_pZombie_Clicker_2->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pWayPoint_1);
		m_pZombie_Clicker_2->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pWayPoint_2);

	}
	
}

void CLTestSceneYH::SpawnItem()
{
	// LongGunAmmo_1
	GameObject* _LongGunAmmo_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo_1->m_Transform->SetPosition({ 10.763f, 0.6f, -6.436f });

	// LongGunAmmo_2
	GameObject* _LongGunAmmo_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo_2->m_Transform->SetPosition({ -8.445f, 0.05f, -3.016f });
	_LongGunAmmo_2->m_Transform->SetRotationFromVec({ 0.0f, 270.0f , 0.0f });

	// HandGunAmmo_1
	GameObject* _HandGunAmmo_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eHandGunAmmo, &m_IsSceneRun);
	_HandGunAmmo_1->m_Transform->SetPosition({ -2.180f, -0.1f , 3.784f });
	_HandGunAmmo_1->m_Transform->SetRotationFromVec({ 0.0f, 270.0f , 0.0f });

	// HandGunAmmo_2
	GameObject* _HandGunAmmo_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eHandGunAmmo, &m_IsSceneRun);
	_HandGunAmmo_2->m_Transform->SetPosition({ -9.459f, 0.05f, -3.270f });
	_HandGunAmmo_2->m_Transform->SetRotationFromVec({ 0.0f, 270.0f , 0.0f });

	// HealPotion
	GameObject* _HealPotion = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eHealPotion, &m_IsSceneRun);
	_HealPotion->m_Transform->SetPosition({ 2.0f, 0.0f, 2.0f });

	// CardKey
	GameObject* _CardKey = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eCardKey, &m_IsSceneRun);
	_CardKey->m_Transform->SetPosition({ -10.110, 0.44f, -2.494 });

	// File
	GameObject* _File = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eFile, &m_IsSceneRun);
	_File->m_Transform->SetPosition({ 7.032f, 1.1f, 5.915f });


	// 탄약 추가분
	// LongGunAmmo_
	GameObject* _LongGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo->m_Transform->SetPosition({ -13.329f, 1.241f, 0.432f });
	_LongGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 90.0f , 0.0f });

	// LongGunAmmo_
	_LongGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo->m_Transform->SetPosition({ -13.659f, 1.241f, 7.180f });
	_LongGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 270.0f , 0.0f });

	// LongGunAmmo_
	_LongGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo->m_Transform->SetPosition({ -9.061f, -0.1f, 6.683f });
	_LongGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 0.0f , 0.0f });

	// LongGunAmmo_
	_LongGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo->m_Transform->SetPosition({ -7.661f, -0.1f, 4.889f });
	_LongGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 0.0f , 0.0f });

	// LongGunAmmo_
	_LongGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo->m_Transform->SetPosition({ -3.354f, 0.05f, -5.127f });
	_LongGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 90.0f , 0.0f });

	// HandGunAmmo_
	GameObject* _HandGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eHandGunAmmo, &m_IsSceneRun);
	_HandGunAmmo->m_Transform->SetPosition({ -13.0f, 1.241f, 7.132f });
	_HandGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 270.0f , 0.0f });


}

void CLTestSceneYH::SpawnInteractionObj()
{
	/// 상호작용 오브젝트
	/// 중요!
	// 로드된 씬 파일(오브젝트)에서 문틀을 찾아서 트랜스폼 애니메이션 달자
	// Left Door
	GameObject* _LeftDoor = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_L");
	assert(_LeftDoor != nullptr);

	// Right Door
	GameObject* _RightDoor = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_R");
	assert(_RightDoor != nullptr);

	GameObject* _DoorFrame = DLLEngine::FindGameObjectByName("Gen_Ctrl_Door_01_Side");
	assert(_DoorFrame != nullptr);

	TransformAnimation* _TransformAnimation = new TransformAnimation();
	_DoorFrame->AddComponent<TransformAnimation>(_TransformAnimation);
	_TransformAnimation->SetPlayType(ePlayType::Once);
	_TransformAnimation->SetFrameRate(eFrameRate::FPS60);

		_TransformAnimation->AddActor(_LeftDoor);
		_TransformAnimation->GetActor("Gen_Ctrl_Door_01_L")->AddKeyFrame(5.0f,
			{ _LeftDoor->m_Transform->m_Position.x - 3.0f, _LeftDoor->m_Transform->m_Position.y, _LeftDoor->m_Transform->m_Position.z });

	_TransformAnimation->AddActor(_RightDoor);
	_TransformAnimation->GetActor("Gen_Ctrl_Door_01_R")->AddKeyFrame(5.0f,
		{ _RightDoor->m_Transform->m_Position.x + 3.6f, _RightDoor->m_Transform->m_Position.y, _RightDoor->m_Transform->m_Position.z });

	/// 카드키 패드(사무실)
	GameObject* _CardkeyPad = DLLEngine::CreateObject(this);
	_CardkeyPad->SetObjectName("CardKeyPad");
	DLLEngine::SetTag("Interaction", _CardkeyPad);

	_CardkeyPad->m_Transform->SetPosition({ 9.876f, 1.2f, -5.6f });
	_CardkeyPad->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	_CardkeyPad->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_CardkeyPad->AddComponent<CardKeyPad>(new CardKeyPad());

	//_CardkeyPad->AddComponent<MeshFilter>(new MeshFilter());
	//_CardkeyPad->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");
	//
	//_CardkeyPad->AddComponent<MeshRenderer>(new MeshRenderer());
	//_CardkeyPad->GetComponent<MeshRenderer>()->SetGizmo(true);

	/// 발전기(가동 후 라이트 온)
	{
		GameObject* _Generator = DLLEngine::CreateObject(this);
		_Generator->SetObjectName("Generator");
		DLLEngine::SetTag("Interaction", _Generator);

		_Generator->m_Transform->SetPosition({ -0.655f, 0.5f, 4.2f });
		_Generator->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
		_Generator->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

		_Generator->AddComponent<Generator>(new Generator());

		_Generator->AddComponent<MeshFilter>(new MeshFilter());
		_Generator->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

		//_Generator->AddComponent<MeshRenderer>(new MeshRenderer());
		//_Generator->GetComponent<MeshRenderer>()->SetGizmo(true);
	}


	/// 리프트 스위치(가동 후 게임 클리어)
	{
		GameObject* _Rift = DLLEngine::CreateObject(this);
		_Rift->SetObjectName("Lift");
		DLLEngine::SetTag("Interaction", _Rift);

		_Rift->m_Transform->SetPosition({ -7.5f, 0.5f, 7.2f });
		_Rift->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
		_Rift->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

		_Rift->AddComponent<Lift>(new Lift());

		_Rift->AddComponent<MeshFilter>(new MeshFilter());
		_Rift->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

		//_Rift->AddComponent<MeshRenderer>(new MeshRenderer());
		//_Rift->GetComponent<MeshRenderer>()->SetGizmo(true);
	}


}

void CLTestSceneYH::SpawnScriptTrigger()
{
	// 트리거 넘버 리셋
	m_TriggerNum = 0;

	/// 트리거 1
	GameObject* _ScriptTrigger_1 = DLLEngine::CreateObject(this);
	_ScriptTrigger_1->SetObjectName("1");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_1);

	_ScriptTrigger_1->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_1->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_1->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_1->m_Transform->SetPosition({ -13.545f, 1.74f, 4.179f });
	_ScriptTrigger_1->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_1->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_1->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	_ScriptTrigger_1->GetComponent<ScriptTrigger>()->m_bIsActivated = true;
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 2
	GameObject* _ScriptTrigger_2 = DLLEngine::CreateObject(this);
	_ScriptTrigger_2->SetObjectName("2");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_2);

	_ScriptTrigger_2->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_2->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_2->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_2->m_Transform->SetPosition({ -11.995f, 1.352f, 6.517f });
	_ScriptTrigger_2->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_2->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_2->GetComponent<ScriptTrigger>()->SetTriggerRange(1.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 3
	GameObject* _ScriptTrigger_3 = DLLEngine::CreateObject(this);
	_ScriptTrigger_3->SetObjectName("3");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_3);

	_ScriptTrigger_3->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_3->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_3->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_3->m_Transform->SetPosition({ -7.597f, 0.409f, 5.288f });
	_ScriptTrigger_3->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_3->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_3->GetComponent<ScriptTrigger>()->SetTriggerRange(3.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 4
	GameObject* _ScriptTrigger_4 = DLLEngine::CreateObject(this);
	_ScriptTrigger_4->SetObjectName("4");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_4);

	_ScriptTrigger_4->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_4->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_4->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_4->m_Transform->SetPosition({ -0.838f, 0.409f, 0.764f });
	_ScriptTrigger_4->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_4->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_4->GetComponent<ScriptTrigger>()->SetTriggerRange(4.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 5
	GameObject* _ScriptTrigger_5 = DLLEngine::CreateObject(this);
	_ScriptTrigger_5->SetObjectName("5");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_5);

	_ScriptTrigger_5->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_5->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_5->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_5->m_Transform->SetPosition({ 9.596f, 1.099f, -6.488f });
	_ScriptTrigger_5->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_5->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_5->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 6
	GameObject* _ScriptTrigger_6 = DLLEngine::CreateObject(this);
	_ScriptTrigger_6->SetObjectName("6");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_6);

	_ScriptTrigger_6->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_6->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_6->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_6->m_Transform->SetPosition({ -8.988f, 0.488f, -3.740f });
	_ScriptTrigger_6->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_6->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_6->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	//////////////////////////////////////////////////////////////////////////////

	
	/// 트리거 7
	GameObject* _ScriptTrigger_7 = DLLEngine::CreateObject(this);
	_ScriptTrigger_7->SetObjectName("7");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_7);

	_ScriptTrigger_7->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_7->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_7->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_7->m_Transform->SetPosition({ 9.596f, 0.488f, -6.488f });
	_ScriptTrigger_7->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_7->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_7->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 8
	GameObject* _ScriptTrigger_8 = DLLEngine::CreateObject(this);
	_ScriptTrigger_8->SetObjectName("8");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_8);

	_ScriptTrigger_8->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_8->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_8->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_8->m_Transform->SetPosition({ 9.082f, 1.009f, 1.522f });
	_ScriptTrigger_8->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_8->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_8->GetComponent<ScriptTrigger>()->SetTriggerRange(2.5f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 9
	GameObject* _ScriptTrigger_9 = DLLEngine::CreateObject(this);
	_ScriptTrigger_9->SetObjectName("9");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_9);

	_ScriptTrigger_9->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_9->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_9->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_9->m_Transform->SetPosition({ 7.567f, 1.099f, 5.330f });
	_ScriptTrigger_9->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_9->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_9->GetComponent<ScriptTrigger>()->SetTriggerRange(1.5f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 10
	GameObject* _ScriptTrigger_10 = DLLEngine::CreateObject(this);
	_ScriptTrigger_10->SetObjectName("10");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_10);

	_ScriptTrigger_10->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_10->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_10->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_10->m_Transform->SetPosition({ -0.715f, 0.409f, 3.440f });
	_ScriptTrigger_10->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_10->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_10->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 11
	GameObject* _ScriptTrigger_11 = DLLEngine::CreateObject(this);
	_ScriptTrigger_11->SetObjectName("11");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_11);

	_ScriptTrigger_11->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_11->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_11->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_11->m_Transform->SetPosition({ -5.231f, 0.409f, 5.232f });
	_ScriptTrigger_11->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_11->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_11->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 12
	GameObject* _ScriptTrigger_12 = DLLEngine::CreateObject(this);
	_ScriptTrigger_12->SetObjectName("12");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_12);

	_ScriptTrigger_12->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_12->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_12->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_12->m_Transform->SetPosition({ -7.407f, 0.409f, 6.519f });
	_ScriptTrigger_12->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_12->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_12->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);
	//////////////////////////////////////////////////////////////////////////////

	/// 넥스트 트리거 연결
	_ScriptTrigger_1->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_2);
	_ScriptTrigger_2->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_3);
	_ScriptTrigger_3->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_4);
	_ScriptTrigger_4->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_5);
	_ScriptTrigger_5->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_6);
	_ScriptTrigger_6->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_7);
	_ScriptTrigger_7->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_8);
	_ScriptTrigger_8->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_9);
	_ScriptTrigger_9->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_10);
	_ScriptTrigger_10->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_11);
	_ScriptTrigger_11->GetComponent<ScriptTrigger>()->SetNextTrigger(_ScriptTrigger_12);
	
}

void CLTestSceneYH::SpawnDecal()
{
	m_Decal = DLLEngine::CreateObject(this);
	m_Decal->SetObjectName("7777777Decal7777777");
	//m_Decal->GetTransform()->SetPosition(DirectX::SimpleMath::Vector3{ -7.5f, -0.361f, 5.7f });
	//m_Decal->GetTransform()->SetScale(DirectX::SimpleMath::Vector3{ 2.0f, 0.5f, 2.0f });
	DecalComponent* _newDecal = new DecalComponent();
	m_Decal->AddComponent<DecalComponent>(_newDecal);


	auto _decalFactory = [this](
		DirectX::SimpleMath::Vector3&& pos, 
		DirectX::SimpleMath::Vector3&& rot,
		DirectX::SimpleMath::Vector3&& scale, std::string&& name)
	{
		GameObject* _newDecalObj = DLLEngine::CreateObject(this);
		_newDecalObj->SetObjectName("_liftBlood");
		_newDecalObj->GetTransform()->SetPosition(pos);
		_newDecalObj->GetTransform()->SetRotationFromVec(rot),
		_newDecalObj->GetTransform()->SetScale(scale);
		DecalComponent* _newDecalPro = new DecalComponent();
		_newDecalPro->m_DecalTextureName = name;
		_newDecalObj->AddComponent<DecalComponent>(_newDecalPro);
	};

	_decalFactory(
		DirectX::SimpleMath::Vector3{ -7.5f, -0.361f, 5.7f },
		DirectX::SimpleMath::Vector3{},
		DirectX::SimpleMath::Vector3{ 2.0f, 0.5f, 2.0f },
		"BloodDecal01.png");

	_decalFactory(
		DirectX::SimpleMath::Vector3{ -9.0f, 1.0f, 8.45f },
		DirectX::SimpleMath::Vector3{ 270, 0, 0 },
		DirectX::SimpleMath::Vector3{ 2.0f, 2.0f, 2.0f },
		"BloodDecal02.png");

	_decalFactory(
		DirectX::SimpleMath::Vector3{ -3.9f, 1.3f, -1.9f },
		DirectX::SimpleMath::Vector3{ 270.0f, 0, 0 },
		DirectX::SimpleMath::Vector3{ 2.0f, 1.0f, 2.0f },
		"BloodDecal03.png");

}
