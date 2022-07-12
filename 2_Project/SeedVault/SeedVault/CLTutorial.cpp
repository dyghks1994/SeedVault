#include "pch.h"
#include "GameManager.h"
#include "MuzzleFlash.h"
#include "EquipmentController.h"
#include "Health.h"
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
#include "ScriptTrigger.h"
#include "Note.h"
#include "Door.h"
#include "DialogueManager.h"
#include "CLTutorial.h"
#include "QuestManager.h"

CLTutorial::CLTutorial()
	: m_pGameManager(nullptr)
	, m_pAxis(nullptr)

	, m_pNavMeshObject_1(nullptr)
	, m_pNavMeshObject_2(nullptr)

	, m_pGround(nullptr)

	, m_pPlayer(nullptr)
	, m_RightHand(nullptr)

	, m_FreeCamera(nullptr)

	, m_pPartner(nullptr)

	, m_pTestCube_1(nullptr)
	, m_pTestCube_2(nullptr)

	, m_pZombie_Runner_1(nullptr)
	, m_pZombie_Runner_2(nullptr)

	, m_pZombie_Clicker_1(nullptr)

	/// UI
	, m_InGameUI(nullptr)
	, m_PauseUI(nullptr)

	, m_TestCube(nullptr)
	, m_BGM_Playing(false)
	, m_IsSceneRun(true)
{
}

CLTutorial::~CLTutorial()
{
}

void CLTutorial::Initialize()
{
	m_IsSceneRun = true;
	m_TriggerNum = 0;
	m_PrevTriggerNum =0;

	/// <summary>
	///  배치된 씬파일 로드
	/// </summary>
	Scene::Initialize();

	/// <summary>
	/// 처음 네비메쉬를 생성하는 부분
	/// </summary>

	DLLNavMesh::AddNavMeshByResourceName("TR_navmesh_002fbx.bin", "TR_navmesh_002fbx.bin");
	DLLNavMesh::SetCurrentNavMeshByName("TR_navmesh_002fbx.bin");

	/// GameManager
	m_pGameManager = DLLEngine::CreateObject(this);
	m_pGameManager->SetObjectName("GameManager");
	m_pGameManager->AddComponent<GameManager>(new GameManager());
	m_pGameManager->AddComponent<Audio>(new Audio());
	////////////////////////////////////////////////////////////////////

	/// 플레이어 오브젝트
	m_pPlayer = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePlayer, &m_IsSceneRun);
	m_pPlayer->m_Transform->SetPosition({ -24.669f, 1.741f, 27.149f });
	m_pPlayer->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });

	/// 플레이어 매쉬 손 위치에 부착할 오브젝트
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
	m_pPartner->m_Transform->SetPosition({ -26.925f, 1.741f, 27.761f });
	m_pPartner->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	/////////////////////////////////////////////////////////////////////

	/// UI
	m_InGameUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eInGameUI, &m_IsSceneRun);
	m_PauseUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePauseUI, &m_IsSceneRun);
	DLLEngine::FindGameObjectByName("Dialogue Manager")->GetComponent<DialogueManager>()->m_pNowTrigger = &m_TriggerNum;
	DLLEngine::FindGameObjectByName("Quest Text")->GetComponent<QuestManager>()->m_pNowTrigger = &m_TriggerNum;


	// 좀비 배치
	SpawnZombie();

	// 아이템 배치
	SpawnItem();

	// 상호작용 오브젝트 배치
	SpawnInteractionObj();

	// 스크림트 트리거 배치
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

}

void CLTutorial::Update(float dTime)
{
	if (!m_BGM_Playing)
	{
		m_pGameManager->GetComponent<Audio>()->PlayEvent("event:/BGM_Tutorial");
		m_BGM_Playing = true;
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)))
	{
		m_IsSceneRun = !m_IsSceneRun;

		//마우스만 움직이게 만들어야함
		//m_pGameManager->s_bTPSMode = !s_bTPSMode;
	}

	//트리거가 바뀌는 시점
	if (m_PrevTriggerNum != m_TriggerNum)
	{
		std::string _triggerNum = std::to_string(m_TriggerNum);
		std::string _base = "00";

		for (int i = 0; i < _triggerNum.length(); i++)
		{
			_base[(int)(_base.length() - _triggerNum.length() + i)] = _triggerNum[i];
		}

		std::string _GT_M = "event:/Dialog/TT/TT_" + _base + "M";
		std::string _GT_A = "event:/Dialog/TT/TT_" + _base + "A";

		m_pPlayer->GetComponent<Audio>()->AllEventStop();
		m_pPartner->GetComponent<Audio>()->AllEventStop();

		m_pPlayer->GetComponent<Audio>()->PlayEvent(_GT_M.c_str(), false);
		m_pPartner->GetComponent<Audio>()->PlayEvent(_GT_A.c_str(), false);
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

	m_Decal->GetTransform()->SetPosition(_pos);

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
			DLLEngine::PickScene("Tutorial_SH");
		}
	}

}



void CLTutorial::Reset()
{	
}

void CLTutorial::SpawnZombie()
{
	/// 러너 1
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ -17.283f, 0.0f, 3.512f });

		m_pZombie_Runner_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Runner, &m_IsSceneRun);
		//m_pZombie_Runner_1->GetComponent<Zombie_Runner_Move>()->AddWayPoint(_pSpawnPoint);

		m_pZombie_Runner_1->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
		m_pZombie_Runner_1->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	}


	/// 클리커 1
	{
		// SpawnPoint
		GameObject* _pSpawnPoint = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eWayPoint, &m_IsSceneRun);
		_pSpawnPoint->m_Transform->SetPosition({ -26.342f, 0.0f, 5.507f });

		m_pZombie_Clicker_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eZombie_Clicker, &m_IsSceneRun);
		//m_pZombie_Clicker_1->GetComponent<Zombie_Clicker_Move>()->AddWayPoint(_pSpawnPoint);

		m_pZombie_Clicker_1->m_Transform->SetPosition(_pSpawnPoint->m_Transform->m_Position);
		m_pZombie_Clicker_1->m_Transform->SetRotationFromVec({ 0.0f, 193.f, 0.0f });
	}

}

void CLTutorial::SpawnItem()
{
	// LongGunAmmo
	GameObject* _LongGunAmmo_1 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo_1->m_Transform->SetPosition({ -30.0f, 1.2f, 2.05f });
	_LongGunAmmo_1->m_Transform->SetRotationFromVec({ 0.0f, 200.0f, 0.0f });

	// LongGunAmmo
	GameObject* _LongGunAmmo_2 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo_2->m_Transform->SetPosition({ -26.5f, 1.2f, 0.35f });
	_LongGunAmmo_2->m_Transform->SetRotationFromVec({ 0.0f, 90.0f, 0.0f });

	// LongGunAmmo
	GameObject* _LongGunAmmo_3 = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eLongGunAmmo, &m_IsSceneRun);
	_LongGunAmmo_3->m_Transform->SetPosition({ -16.956f, 1.2f, 3.533f });
	_LongGunAmmo_3->m_Transform->SetRotationFromVec({ 0.0f, 270.0f, 0.0f });

	// HandGunAmmo
	GameObject* _HandGunAmmo = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eHandGunAmmo, &m_IsSceneRun);
	_HandGunAmmo->m_Transform->SetPosition({ -29.65f, 1.2f, 2.855f });
	_HandGunAmmo->m_Transform->SetRotationFromVec({ 0.0f, 200.0f, 0.0f });
}

void CLTutorial::SpawnInteractionObj()
{
	GameObject* _Note = DLLEngine::CreateObject(this);
	_Note->SetObjectName("Note");
	DLLEngine::SetTag("Interaction", _Note);

	_Note->m_Transform->SetPosition({ -31.608f, 1.2f, 1.0f });
	_Note->m_Transform->SetRotationFromVec({ 0.0f, 90.0f, 0.0f });
	_Note->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });

	_Note->AddComponent<MeshFilter>(new MeshFilter());
	_Note->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_FILE);

	_Note->AddComponent<MeshRenderer>(new MeshRenderer());

	_Note->AddComponent<Note>(new Note());

	GameObject* _Door = DLLEngine::CreateObject(this);
	_Door->SetObjectName("Door");
	DLLEngine::SetTag("Interaction", _Door);

	GameObject* _leftDoor = DLLEngine::FindGameObjectByName("Gen_Gate_L");
	assert(_leftDoor != nullptr);

	GameObject* _rightDoor = DLLEngine::FindGameObjectByName("Gen_Gate_R");
	assert(_rightDoor != nullptr);


	TransformAnimation* _TransformAnimation = new TransformAnimation();
	_Door->AddComponent<TransformAnimation>(_TransformAnimation);
	_TransformAnimation->SetPlayType(ePlayType::Once);
	_TransformAnimation->SetFrameRate(eFrameRate::FPS60);

	_TransformAnimation->AddActor(_leftDoor);
	_TransformAnimation->GetActor("Gen_Gate_L")->AddKeyFrame(30.0f,
		{ _leftDoor->m_Transform->m_Position.x, _leftDoor->m_Transform->m_Position.y, _leftDoor->m_Transform->m_Position.z - 9.6f });

	_TransformAnimation->AddActor(_rightDoor);
	_TransformAnimation->GetActor("Gen_Gate_R")->AddKeyFrame(30.0f,
		{ _rightDoor->m_Transform->m_Position.x, _rightDoor->m_Transform->m_Position.y, _rightDoor->m_Transform->m_Position.z  + 9.6f});



	_Door->m_Transform->SetPosition({ -14.7f, 1.670f, 2.1f });
	_Door->m_Transform->SetRotationFromVec({ 0.0f, 270.0f, 0.0f });
	_Door->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_Door->AddComponent<MeshFilter>(new MeshFilter());
	_Door->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_Door->AddComponent<MeshRenderer>(new MeshRenderer());

	_Door->AddComponent<Door>(new Door());
}

void CLTutorial::SpawnScriptTrigger()
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

	_ScriptTrigger_1->m_Transform->SetPosition({ -25.208f, 1.670f, 23.0f });
	_ScriptTrigger_1->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	_ScriptTrigger_1->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_1->GetComponent<ScriptTrigger>()->SetTriggerRange(4.0f);
	_ScriptTrigger_1->GetComponent<ScriptTrigger>()->m_bIsActivated = true;
	//////////////////////////////////////////////////////////////////////////////

	/// 트리거 2
	GameObject* _ScriptTrigger_2 = DLLEngine::CreateObject(this);
	_ScriptTrigger_2->SetObjectName("2");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_2);

	_ScriptTrigger_2->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_2->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_2->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_2->m_Transform->SetPosition({ -24.508f, 1.670f, 18.8f });
	_ScriptTrigger_2->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_2->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_2->GetComponent<ScriptTrigger>()->SetTriggerRange(1.5f);

	/// 트리거 3
	GameObject* _ScriptTrigger_3 = DLLEngine::CreateObject(this);
	_ScriptTrigger_3->SetObjectName("3");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_3);

	_ScriptTrigger_3->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_3->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_3->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_3->m_Transform->SetPosition({ -25.208f, 1.670f, 13.0f });
	_ScriptTrigger_3->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_3->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_3->GetComponent<ScriptTrigger>()->SetTriggerRange(2.5f);

	/// 트리거 4
	GameObject* _ScriptTrigger_4 = DLLEngine::CreateObject(this);
	_ScriptTrigger_4->SetObjectName("4");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_4);

	_ScriptTrigger_4->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_4->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_4->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_4->m_Transform->SetPosition({ -26.408f, 1.670f, 4.7f });
	_ScriptTrigger_4->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_4->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_4->GetComponent<ScriptTrigger>()->SetTriggerRange(1.2f);

	/// 트리거 5
	GameObject* _ScriptTrigger_5 = DLLEngine::CreateObject(this);
	_ScriptTrigger_5->SetObjectName("5");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_5);

	_ScriptTrigger_5->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_5->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_5->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_5->m_Transform->SetPosition({ -26.408f, 1.670f, 3.1f });
	_ScriptTrigger_5->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_5->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_5->GetComponent<ScriptTrigger>()->SetTriggerRange(1.0f);

	/// 트리거 6
	GameObject* _ScriptTrigger_6 = DLLEngine::CreateObject(this);
	_ScriptTrigger_6->SetObjectName("6");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_6);

	_ScriptTrigger_6->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_6->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_6->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_6->m_Transform->SetPosition({ -31.108f, 1.670f, 1.0f });
	_ScriptTrigger_6->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_6->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_6->GetComponent<ScriptTrigger>()->SetTriggerRange(1.0f);

	/// 트리거 7
	GameObject* _ScriptTrigger_7 = DLLEngine::CreateObject(this);
	_ScriptTrigger_7->SetObjectName("7");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_7);

	_ScriptTrigger_7->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_7->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_7->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_7->m_Transform->SetPosition({ -27.7f, 1.670f, 1.8f });
	_ScriptTrigger_7->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_7->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_7->GetComponent<ScriptTrigger>()->SetTriggerRange(2.0f);

	/// 트리거 8
	GameObject* _ScriptTrigger_8 = DLLEngine::CreateObject(this);
	_ScriptTrigger_8->SetObjectName("8");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_8);

	_ScriptTrigger_8->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_8->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_8->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_8->m_Transform->SetPosition({ -20.708f, 1.670f, 1.1f });
	_ScriptTrigger_8->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_8->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_8->GetComponent<ScriptTrigger>()->SetTriggerRange(1.0f);

	/// 트리거 9
	GameObject* _ScriptTrigger_9 = DLLEngine::CreateObject(this);
	_ScriptTrigger_9->SetObjectName("9");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_9);

	_ScriptTrigger_9->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_9->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_9->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_9->m_Transform->SetPosition({ -17.308f, 1.670f, 1.1f });
	_ScriptTrigger_9->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_9->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_9->GetComponent<ScriptTrigger>()->SetTriggerRange(1.2f);

	/// 트리거 10
	GameObject* _ScriptTrigger_10 = DLLEngine::CreateObject(this);
	_ScriptTrigger_10->SetObjectName("10");
	DLLEngine::SetTag("ScriptTrigger", _ScriptTrigger_10);

	_ScriptTrigger_10->AddComponent<MeshFilter>(new MeshFilter());
	_ScriptTrigger_10->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//_ScriptTrigger_10->AddComponent<MeshRenderer>(new MeshRenderer());

	_ScriptTrigger_10->m_Transform->SetPosition({ -14.7f, 1.670f, 2.1f });
	_ScriptTrigger_10->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });

	_ScriptTrigger_10->AddComponent<ScriptTrigger>(new ScriptTrigger());
	_ScriptTrigger_10->GetComponent<ScriptTrigger>()->SetTriggerRange(1.5f);
	///////////////////////////////////////////////////////////////////////////////////

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
}

void CLTutorial::SpawnDecal()
{
	m_Decal = DLLEngine::CreateObject(this);
	m_Decal->SetObjectName("7777777Decal7777777");
	DecalComponent* _newDecal = new DecalComponent();
	_newDecal->m_DecalTextureName = "leakage.dds";
	m_Decal->AddComponent<DecalComponent>(_newDecal);

	GameObject* _pilllar = DLLEngine::CreateObject(this);
	_pilllar->GetTransform()->SetPosition(DirectX::SimpleMath::Vector3{-24.578f, 2.495f, 9.502f});
	_pilllar->GetTransform()->SetScale(DirectX::SimpleMath::Vector3{ 0.5f, 0.5f, 0.5f });
	MeshFilter* _newMesh = new MeshFilter();
	_newMesh->SetMesh("BG_TR_BD_Pillar_01.bin");
	_pilllar->AddComponent<MeshFilter>(_newMesh);
	_pilllar->AddComponent<MeshRenderer>(new MeshRenderer);

}
