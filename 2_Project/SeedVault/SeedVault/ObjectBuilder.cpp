#include "pch.h"
#include "CameraController.h"
#include "CameraBehavior.h"
#include "OrbitCamera.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "Partner_Move.h"
#include "Partner_AI.h"
#include "Zombie_Runner_Move.h"
#include "Zombie_Runner_AI.h"
#include "Zombie_Clicker_Move.h"
#include "Zombie_Clicker_AI.h"
#include "Health.h"
#include "HitPoint.h"
#include "MuzzleFlash.h"
#include "NavMeshAgent.h"
#include "HeadLightController.h"

#include "EquipmentController.h"
#include "LeftHandController.h"
#include "DialogueManager.h"
#include "QuestManager.h"

#include "Audio.h"
#include "AudioPercentageDefine.h"
#include "ObjectBuilder.h"


/// <summary>
/// ObjectType에 해당하는 오브젝트를 만들어서 리턴
/// </summary>
/// <param name="type">만들 오브젝트 타입</param>
/// <returns>만들어진 오브젝트</returns>

GameObject* ObjectBuilder::MakeObject(ObjectType type, bool* isSceneRun)
{
	GameObject* obj = nullptr;
	switch (type)
	{

	case ObjectBuilder::ObjectType::ePlayer:
		obj = MakePlayer(obj);
		break;

	case ObjectBuilder::ObjectType::eLeftHand:
		obj = MakeLeftHand(obj);
		break;

	case ObjectBuilder::ObjectType::eRightHand:
		obj = MakeRightHand(obj);
		break;


	case ObjectBuilder::ObjectType::ePartner:
		obj = MakePartner(obj);
		break;


	case ObjectBuilder::ObjectType::eZombie_Runner:
		obj = MakeZombie_Runner(obj);
		break;

	case ObjectBuilder::ObjectType::eZombie_Clicker:
		obj = MakeZombie_Clicker(obj);
		break;


	case ObjectBuilder::ObjectType::eWayPoint:
		obj = MakeWayPoint(obj);
		break;


	case ObjectBuilder::ObjectType::eLongGunAmmo:
		obj = MakeLongGunAmmo(obj);
		break;


	case ObjectBuilder::ObjectType::eHandGunAmmo:
		obj = MakeHandGunAmmo(obj);
		break;

	case ObjectBuilder::ObjectType::eHealPotion:
		obj = MakeHealPotion(obj);
		break;

	case ObjectBuilder::ObjectType::eCardKey:
		obj = MakeCardKey(obj);
		break;

	case ObjectBuilder::ObjectType::eFile:
		obj = MakeFile(obj);
		break;

	case ObjectBuilder::ObjectType::eInGameUI:
		obj = MakeIngameUI(obj, isSceneRun);
		break;

	case ObjectBuilder::ObjectType::ePauseUI:
		obj = MakePauseUI(obj, isSceneRun);
		break;

	default:
		break;
	}

	return obj;
}

GameObject* ObjectBuilder::MakePlayer(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Player");
	obj->SetTagName("Player");

	/// <summary>
	/// Audio
	/// </summary>
	Audio* _pAudio = new Audio();
	obj->AddComponent<Audio>(_pAudio);


	PhysicsActor* _PlayerPhysActor = new PhysicsActor({ 0.15f, 0.62f, 0.15f }, RigidType::Dynamic);
	//_PlayerPhysActor->SetGravity(false);
	//_PlayerPhysActor->SetKinematic(true);
	obj->AddComponent(_PlayerPhysActor);
	_PlayerPhysActor->SetFreezeRotation(true, false, true);
	_PlayerPhysActor->SetFreezePosition(false, true, false);

	// [PlayerController]
	PlayerController* _playerController = new PlayerController();
	obj->AddComponent<PlayerController>(_playerController);

	// [Health]
	Health* _playerHealth = new Health();
	obj->AddComponent<Health>(_playerHealth);

	// [Inventory]
	Inventory* _playerInventory = new Inventory();
	obj->AddComponent<Inventory>(_playerInventory);

	// [ParticleSpawner]
	ParticleSpawner* _particleSpawner = new ParticleSpawner();
	_particleSpawner->AddSpriteName("dia.png");
	_particleSpawner->AddSpriteName("08.png");
	obj->AddComponent<ParticleSpawner>(_particleSpawner);

	// [HitPoint]
	HitPoint* _hitPoint = new HitPoint();
	obj->AddComponent<HitPoint>(_hitPoint);

	// [NavMeshAgent]
	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/// 플레이어 메쉬 오브젝트(플레이어이 자식으로 들어감)
	{
		GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_meshObj->SetObjectName("Player_Mesh");

		DLLEngine::SetTag("PlayerMesh", _meshObj);

		_meshObj->m_Transform->SetPosition({ 0.0f, -0.62f, 0.0f });
		_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
		_meshObj->m_Transform->SetScale({ 1.0f, 1.0f, 1.0f });	// 모델이 너무 작아서 스케일 조정..

		MeshFilter* _Mesh = new MeshFilter();
		_Mesh->SetMesh(CL::ResourcePath::MESH_PLAYER_RIFLE);
		_meshObj->AddComponent<MeshFilter>(_Mesh);

		//MeshRenderer
		_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
		_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);

		// [Animator]
		// 애니메이터 생성
		Animator* _playerAnimator = new Animator();

		/// Layer 추가
		// Rifle_Movement
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		auto _animLayer_Rifle = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		_animLayer_Rifle->m_MaskingType = eMASKING_TYPE::NONE;
		{
			/// State 추가
			// Stand
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_IDLE);
			auto _animState_Rifle_Stand_Idle = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_FORWARD);
			auto _animState_Rifle_Stand_Walk_Forward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);

			/// <summary>
			/// 걷는 애니메이션 Sound삽입
			/// </summary>
			_animState_Rifle_Stand_Walk_Forward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Forward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_BACKWARD);
			auto _animState_Rifle_Stand_Walk_Backward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
			_animState_Rifle_Stand_Walk_Backward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Backward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_LEFT);
			auto _animState_Rifle_Stand_Walk_Left = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
			_animState_Rifle_Stand_Walk_Left->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Left->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_WALK_RIGHT);
			auto _animState_Rifle_Stand_Walk_Right = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
			_animState_Rifle_Stand_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Rifle_Stand_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_SPRINT);
			auto _animState_Rifle_Stand_Walk_Sprint = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
			_animState_Rifle_Stand_Walk_Sprint->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 2);
				},
				PLAYER_FOOTSTEP_RUN_L
					);
			_animState_Rifle_Stand_Walk_Sprint->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 2);
				},
				PLAYER_FOOTSTEP_RUN_R
					);

			// Crouch
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_IDLE);
			auto _animState_Rifle_Crouch_Idle = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
			auto _animState_Rifle_Crouch_Walk_Forward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
			_animState_Rifle_Crouch_Walk_Forward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Forward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
			auto _animState_Rifle_Crouch_Walk_Backward = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
			_animState_Rifle_Crouch_Walk_Backward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Backward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_LEFT);
			auto _animState_Rifle_Crouch_Walk_Left = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
			_animState_Rifle_Crouch_Walk_Left->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Left->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
			auto _animState_Rifle_Crouch_Walk_Right = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
			_animState_Rifle_Crouch_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Rifle_Crouch_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;



					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 1);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);
			// Rifle_Die
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DIE);
			auto _animState_Rifle_Die = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);

			// Rifle_Dead
			_animLayer_Rifle->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD, CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_DEAD);
			auto _animState_Rifle_Dead = _animLayer_Rifle->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD);

			// end of State

			/// Transition 설정
			{
				//Rifle_Dead
				{
					_animState_Rifle_Die->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DEAD);
					_animState_Rifle_Die->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDead, true);
				}

				// Stand
				{
					// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Stand_Die
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(0)->m_FadingPeriod = 8;

						// Stand_Die -> Rifle_Dead
						_animState_Rifle_Die->AddEvent(
							[obj]() {
								obj->GetComponent<PlayerController>()->PostDie();
							}, 0.99f
						);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Walking
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(1)->m_FadingPeriod = 00;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(2)->m_FadingPeriod = 00;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(4)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Sprint
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(5)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> "Crouch_Idle"
						_animState_Rifle_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Idle->m_Transition_V->at(6)->m_FadingPeriod = 5;
					}

					// Stand_Walk_Forward
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_FORWARD -> Stand_Die
						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 8;
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 8;
					}

					// Stand_Walk_Backward
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_BACKWARD -> Stand_Die
						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Rifle_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 8;
					}

					// Stand_Walk_Left
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_LEFT -> Stand_Die
						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Rifle_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Left->m_Transition_V->at(6)->m_FadingPeriod = 8;
					}

					// Stand_Walk_Right
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_RIGHT -> Stand_Die
						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 20;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Rifle_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Rifle_Stand_Walk_Right->m_Transition_V->at(6)->m_FadingPeriod = 8;
					}

					// Stand_Sprint
					{
						// ANIMLAYER_PLAYER_RIFLE_STAND_SPRINT -> Stand_Die
						_animState_Rifle_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Stand_Walk_Sprint->m_Transition_V->at(1)->AddParameter(&_playerController->m_bSprint, false);

					}
				} // end of Stand


				// Crouch
				{
					// "Crouch_Idle"
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_IDLE -> Crouch_Die
						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(0)->m_FadingPeriod = 8;

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_IDLE);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(1)->AddParameter(&_playerController->m_bCrouch, false);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(4)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(5)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(6)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);
					}

					// Crouch_Walk_Forward
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_FORWARD -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 20;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 30;

						_animState_Rifle_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_SPRINT);
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);

					}

					// Crouch_Walk_Backward
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_BACKWARD -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_VForAnim, true);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Rifle_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Left
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_LEFT -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Rifle_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Right
					{
						// ANIMLAYER_PLAYER_RIFLE_CROUCH_RIGHT -> Crouch_Die
						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_DIE);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_IDLE);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_FORWARD);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_BACKWARD);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_CROUCH_WALK_LEFT);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 5;
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Rifle_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_RIFLE_STAND_WALK_RIGHT);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Rifle_Crouch_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 30;

					}
				} // end of Crouch

			} // end of Transition

		} // end of Layer[Rifle_Movement]

		// Rifle_Assassinate_Begin
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_BEGIN);
		auto _animLayer_Rifle_Assassinate_Begin = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_BEGIN);
		_animLayer_Rifle_Assassinate_Begin->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Rifle_Assassinate_Begin->SetEnabled(false);
		_animLayer_Rifle_Assassinate_Begin->AddStateMap("Assassinate_Begin", CL::ResourcePath::ANIM_PLAYER_RIFLE_ASSASSINATE_BEGIN);

		// Rifle_Assassinate_End
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END);
		auto _animLayer_Rifle_Assassinate_End = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END);
		_animLayer_Rifle_Assassinate_End->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Rifle_Assassinate_End->SetEnabled(false);
		_animLayer_Rifle_Assassinate_End->AddStateMap("Assassinate_End", CL::ResourcePath::ANIM_PLAYER_RIFLE_ASSASSINATE_END);

		// Event
		{
			// Assassinate_Begin -> End
			auto _animState_Rifle_Assassinate_Begin = _animLayer_Rifle_Assassinate_Begin->GetState("Assassinate_Begin");
			_animState_Rifle_Assassinate_Begin->AddEvent(
				[_meshObj]() {
					_meshObj->GetComponent<Animator>()->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_END);
					CA_TRACE("[Player] Assassinate Begin End");
				},
				0.99f
					);

			// Assassinate_End -> 해제
			auto _animState_Rifle_Assassinate_End = _animLayer_Rifle_Assassinate_End->GetState("Assassinate_End");
			_animState_Rifle_Assassinate_End->AddEvent(
				[obj]() {
					obj->GetComponent<PlayerController>()->PostAssassinate();
				},
				0.99f
					);
		}
		// // end of Layer[Rifle_Assassinate]

		// Rifle_Bited_Die
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_BITED_DIE);
		auto _animLayer_Rifle_Bited_Die = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_BITED_DIE);
		_animLayer_Rifle_Bited_Die->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Rifle_Bited_Die->SetEnabled(false);
		_animLayer_Rifle_Bited_Die->AddStateMap("Bited_Die", CL::ResourcePath::ANIM_PLAYER_RIFLE_BITED_DIE);
		_animLayer_Rifle_Bited_Die->GetState("Bited_Die")->AddEvent(
			[_meshObj, obj]() {
				_meshObj->GetComponent<Animator>()->SetNoneAnimLayer(CL::ResourcePath::ANIM_PLAYER_RIFLE_BITED_DEAD);
				obj->GetComponent<PlayerController>()->m_bIsDie = false;
				obj->GetComponent<PlayerController>()->m_bIsDead = true;
			},
			0.99f
				);

		// Rifle_Bited_Dead
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_BITED_DEAD);
		auto _animLayer_Rifle_Bited_Dead = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_BITED_DEAD);
		_animLayer_Rifle_Bited_Dead->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Rifle_Bited_Dead->SetEnabled(false);
		_animLayer_Rifle_Bited_Dead->AddStateMap("Bited_Dead", CL::ResourcePath::ANIM_PLAYER_RIFLE_BITED_DEAD);


		/// Layer 추가
		// Pistol
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
		auto _animLayer_Pistol = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
		{
			/// State 추가
			_animLayer_Pistol->m_MaskingType = eMASKING_TYPE::NONE;

			// Stand
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_IDLE);
			auto _animState_Pistol_Stand_Idle = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_FORWARD);
			auto _animState_Pistol_Stand_Walk_Forward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
			_animState_Pistol_Stand_Walk_Forward->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Pistol_Stand_Walk_Forward->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_BACKWARD);
			auto _animState_Pistol_Stand_Walk_Backward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
			_animState_Pistol_Stand_Walk_Backward->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Pistol_Stand_Walk_Backward->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_LEFT);
			auto _animState_Pistol_Stand_Walk_Left = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
			_animState_Pistol_Stand_Walk_Left->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);
			_animState_Pistol_Stand_Walk_Left->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_WALK_RIGHT);
			auto _animState_Pistol_Stand_Walk_Right = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
			_animState_Pistol_Stand_Walk_Right->AddEvent(

				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_L
					);

			_animState_Pistol_Stand_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_WALK_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_SPRINT);
			auto _animState_Pistol_Stand_Walk_Sprint = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
			_animState_Pistol_Stand_Walk_Sprint->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_RUN_L
					);

			_animState_Pistol_Stand_Walk_Sprint->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_RUN_R
					);

			// Crouch
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_IDLE);
			auto _animState_Pistol_Crouch_Idle = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
			auto _animState_Pistol_Crouch_Walk_Forward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
			_animState_Pistol_Crouch_Walk_Forward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Forward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
			auto _animState_Pistol_Crouch_Walk_Backward = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
			_animState_Pistol_Crouch_Walk_Backward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Backward->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_LEFT);
			auto _animState_Pistol_Crouch_Walk_Left = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
			_animState_Pistol_Crouch_Walk_Left->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Left->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT, CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
			auto _animState_Pistol_Crouch_Walk_Right = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
			_animState_Pistol_Crouch_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_L
					);
			_animState_Pistol_Crouch_Walk_Right->AddEvent(


				[_pAudio]() {
					//0: walk/1: Cwalk/2: Run;

					_pAudio->PlayEvent("event:/Player/Player_Footsteps", "WalkCWalkRun", 0);
				},
				PLAYER_FOOTSTEP_CROUCH_R
					);

			// Pistol_Die
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_DIE);
			auto _animState_Pistol_Stand_Die = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);

			// Pistol_Dead
			_animLayer_Pistol->AddStateMap(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DEAD, CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_DEAD);
			auto _animState_Pistol_Stand_Dead = _animLayer_Pistol->GetState(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DEAD);

			// end of State

			/// Transition 설정
			{
				//Rifle_Dead
				{
					_animState_Pistol_Stand_Die->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DEAD);
					_animState_Pistol_Stand_Die->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDead, true);
				}

				// Stand
				{
					// ANIMLAYER_PLAYER_PISTOL_STAND_IDLE
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_IDLE -> Stand_Die
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(0)->m_FadingPeriod = 8;

						// Stand_Die -> Pistol_Dead
						_animState_Pistol_Stand_Die->AddEvent(
							[obj]() {
								obj->GetComponent<PlayerController>()->PostDie();
							}, 0.99f
						);

						// ANIMLAYER_PLAYER_PISTOL_STAND_IDLE -> Walking
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(2)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(3)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(4)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> Sprint
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(5)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						// ANIMLAYER_PLAYER_RIFLE_STAND_IDLE -> "Crouch_Idle"
						_animState_Pistol_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Idle->m_Transition_V->at(6)->m_FadingPeriod = 5;
					}

					// Stand_Walk_Forward
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_FORWARD -> Stand_Die
						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(4)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(5)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Backward
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_BACKWARD -> Stand_Die
						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Pistol_Stand_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Left
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_LEFT -> Stand_Die
						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Pistol_Stand_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Left->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Walk_Right
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_RIGHT -> Stand_Die
						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 80;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_V, false);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 15;
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bSprint, true);

						_animState_Pistol_Stand_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(6)->AddParameter(&_playerController->m_bCrouch, true);
						_animState_Pistol_Stand_Walk_Right->m_Transition_V->at(6)->m_FadingPeriod = 30;
					}

					// Stand_Sprint
					{
						// ANIMLAYER_PLAYER_PISTOL_STAND_SPRINT -> Stand_Die
						_animState_Pistol_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Stand_Walk_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Stand_Walk_Sprint->m_Transition_V->at(1)->AddParameter(&_playerController->m_bSprint, false);

					}
				} // end of Stand


				// Crouch
				{
					// "Crouch_Idle"
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_IDLE -> Crouch_Die
						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_IDLE);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(1)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(1)->AddParameter(&_playerController->m_bCrouch, false);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(4)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(5)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(5)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(6)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Idle->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);

					}

					// Crouch_Walk_Forward
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_FORWARD -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_V, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(2)->m_FadingPeriod = 20;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(3)->m_FadingPeriod = 20;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_VForAnim, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_V, false);

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(5)->m_FadingPeriod = 30;

						_animState_Pistol_Crouch_Walk_Forward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_SPRINT);
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(6)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Forward->m_Transition_V->at(6)->AddParameter(&_playerController->m_bSprint, true);
					}

					// Crouch_Walk_Backward
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_BACKWARD -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_V, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_H, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_H, false);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(2)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, false);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(3)->m_FadingPeriod = 10;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, true);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(4)->m_FadingPeriod = 15;
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_VForAnim, true);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_V, true);

						_animState_Pistol_Crouch_Walk_Backward->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Backward->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Left
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_LEFT -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_H, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(-1.0f, &_playerController->m_HForAnim, true);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(4)->AddParameter(0.1f, &_playerController->m_H, true);

						_animState_Pistol_Crouch_Walk_Left->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Left->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}

					// Crouch_Walk_Right
					{
						// ANIMLAYER_PLAYER_PISTOL_CROUCH_RIGHT -> Crouch_Die
						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_DIE);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(0)->AddParameter(&_playerController->m_bIsDie, true);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(0)->m_FadingPeriod = 80;

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_IDLE);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->m_FadingPeriod = 3;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_H, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(-0.001f, &_playerController->m_V, true);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(1)->AddParameter(0.001f, &_playerController->m_V, false);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_FORWARD);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(2)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(2)->AddParameter(0.1f, &_playerController->m_VForAnim, true);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_BACKWARD);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(3)->m_FadingPeriod = 5;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(0.1f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(3)->AddParameter(-0.1f, &_playerController->m_VForAnim, false);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_CROUCH_WALK_LEFT);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(4)->m_FadingPeriod = 30;
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(1.0f, &_playerController->m_HForAnim, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(4)->AddParameter(-0.1f, &_playerController->m_H, false);

						_animState_Pistol_Crouch_Walk_Right->AddTrnasition(CL::ResourcePath::ANIM_STATE_PLAYER_PISTOL_STAND_WALK_RIGHT);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(5)->AddParameter(&_playerController->m_bCrouch, false);
						_animState_Pistol_Crouch_Walk_Right->m_Transition_V->at(5)->m_FadingPeriod = 30;
					}
				} // end of Crouch

			} // end of Transition

		} // end of Layer[Pistol_Movement]

		// Pistol_Assassinate_Begin
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_BEGIN);
		auto _animLayer_Pistol_Assassinate_Begin = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_BEGIN);
		_animLayer_Pistol_Assassinate_Begin->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Pistol_Assassinate_Begin->SetEnabled(false);
		_animLayer_Pistol_Assassinate_Begin->AddStateMap("Assassinate_Begin", CL::ResourcePath::ANIM_PLAYER_PISTOL_ASSASSINATE_BEGIN);

		// Pistol_Assassinate_End
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END);
		auto _animLayer_Pistol_Assassinate_End = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END);
		_animLayer_Pistol_Assassinate_End->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Pistol_Assassinate_End->SetEnabled(false);
		_animLayer_Pistol_Assassinate_End->AddStateMap("Assassinate_End", CL::ResourcePath::ANIM_PLAYER_PISTOL_ASSASSINATE_END);

		// Event
		{
			// Assassinate_Begin -> End
			auto _animState_Pistol_Assassinate_Begin = _animLayer_Pistol_Assassinate_Begin->GetState("Assassinate_Begin");
			_animState_Pistol_Assassinate_Begin->AddEvent(
				[_meshObj]() {
					_meshObj->GetComponent<Animator>()->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_END);
					CA_TRACE("[Player] Assassinate Begin End");
				},
				0.99f
					);

			// Assassinate_End -> 해제
			auto _animState_Pistol_Assassinate_End = _animLayer_Pistol_Assassinate_End->GetState("Assassinate_End");
			_animState_Pistol_Assassinate_End->AddEvent(
				[obj]() {
					obj->GetComponent<PlayerController>()->PostAssassinate();
				},
				0.99f
					);
		} // end of Layer[Pistol_Assassinate]

		// Pistol_Bited_Die
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_BITED_DIE);
		auto _animLayer_Pistol_Bited_Die = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_BITED_DIE);
		_animLayer_Pistol_Bited_Die->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Pistol_Bited_Die->SetEnabled(false);
		_animLayer_Pistol_Bited_Die->AddStateMap("Bited_Die", CL::ResourcePath::ANIM_PLAYER_PISTOL_BITED_DIE);
		_animLayer_Pistol_Bited_Die->GetState("Bited_Die")->AddEvent(
			[_meshObj, obj]() {
				_meshObj->GetComponent<Animator>()->SetNoneAnimLayer(CL::ResourcePath::ANIM_PLAYER_PISTOL_BITED_DEAD);
				obj->GetComponent<PlayerController>()->m_bIsDie = false;
				obj->GetComponent<PlayerController>()->m_bIsDead = true;
			},
			0.99f
				);

		// Pistol_Bited_Dead
		_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_BITED_DEAD);
		auto _animLayer_Pistol_Bited_Dead = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_BITED_DEAD);
		_animLayer_Pistol_Bited_Dead->m_MaskingType = eMASKING_TYPE::NONE;
		_animLayer_Pistol_Bited_Dead->SetEnabled(false);
		_animLayer_Pistol_Bited_Dead->AddStateMap("Bited_Dead", CL::ResourcePath::ANIM_PLAYER_PISTOL_BITED_DEAD);



		/// Aiming
		// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
		{
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
			auto _animLayer_Rifle_Aim = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);

			_animLayer_Rifle_Aim->SetEnabled(false);
			_animLayer_Rifle_Aim->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_animLayer_Rifle_Aim->AddLocomotion("Rifle_Pitch", CL::ResourcePath::ANIM_PLAYER_RIFLE_PITCH, &_playerController->m_PitchValue);

			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_AIM);
			auto _animLayer_Pistol_Aim = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_AIM);

			_animLayer_Pistol_Aim->SetEnabled(false);
			_animLayer_Pistol_Aim->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_animLayer_Pistol_Aim->AddLocomotion("Pistol_Pitch", CL::ResourcePath::ANIM_PLAYER_PISTOL_PITCH, &_playerController->m_PitchValue);
		}

		/// Shoot
		{
			// Rifle 만
			_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SHOOT);
			auto _animLayer_Rifle_Shoot = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SHOOT);

			_animLayer_Rifle_Shoot->SetEnabled(false);
			_animLayer_Rifle_Shoot->m_MaskingType = eMASKING_TYPE::OVERRINDING;
			_animLayer_Rifle_Shoot->AddStateMap("Rifle_Shoot", CL::ResourcePath::ANIM_PLAYER_RIFLE_SHOOT);

			auto _animState_Rifle_Shoot = _animLayer_Rifle_Shoot->GetState("Rifle_Shoot");
			_animState_Rifle_Shoot->AddEvent(
				[obj, _meshObj]() {
					obj->GetComponent<PlayerController>()->m_bShoot = false;
					_meshObj->GetComponent<Animator>()->SetOverrideAnimLayer("");
					CA_TRACE("사격 딜레이 끝");
				}
				,
					0.99f
					);
		}

		///  Reload
		// 주무기, 보조무기, 투척무기 3가지 종류 필요하고 구분해서 재생해야할듯...
		{
			// LongGun
			{

				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);
				auto _animLayer_Rifle_Stand_Reload = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);

				_animLayer_Rifle_Stand_Reload->SetEnabled(false);
				_animLayer_Rifle_Stand_Reload->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Stand_Reload->AddStateMap("Rifle_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_STAND_RELOAD);
				auto _animState_Rifle_Stand_Reload = _animLayer_Rifle_Stand_Reload->GetState("Rifle_Reload");
				_animState_Rifle_Stand_Reload->AddEvent(
					[_pAudio]() {
						//0 : pistol 1 : Rifle

						_pAudio->PlayEvent("event:/Player/Player_Reload", "Reload", 1);
					},
					PLAYER_RELOAD
						);

				// Crouch
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);
				auto _animLayer_Rifle_Crouch_Reload = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);

				_animLayer_Rifle_Crouch_Reload->SetEnabled(false);
				_animLayer_Rifle_Crouch_Reload->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Crouch_Reload->AddStateMap("Rifle_Crouch_Reload", CL::ResourcePath::ANIM_PLAYER_RIFLE_CROUCH_RELOAD);
				auto _animState_Rifle_Crouch_Reload = _animLayer_Rifle_Crouch_Reload->GetState("Rifle_Crouch_Reload");
				_animState_Rifle_Crouch_Reload->AddEvent(
					[_pAudio]() {
						//0 : pistol 1 : Rifle

						_pAudio->PlayEvent("event:/Player/Player_Reload", "Reload", 1);
					},
					PLAYER_RELOAD
						);
			}

			// HandGun
			{

				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_STAND_RELOAD);
				auto _animLayer_Pistol_Stand_Reload = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_STAND_RELOAD);

				_animLayer_Pistol_Stand_Reload->SetEnabled(false);
				_animLayer_Pistol_Stand_Reload->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Pistol_Stand_Reload->AddStateMap("Pistol_Reload", CL::ResourcePath::ANIM_PLAYER_PISTOL_STAND_RELOAD);
				auto _animState_Pistol_Stand_Reload = _animLayer_Pistol_Stand_Reload->GetState("Pistol_Reload");
				_animState_Pistol_Stand_Reload->AddEvent(
					[_pAudio]() {
						//0 : pistol 1 : Rifle

						_pAudio->PlayEvent("event:/Player/Player_Reload", "Reload", 0);
					},
					PLAYER_RELOAD
						);
				// Crouch
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_CROUCH_RELOAD);
				auto _animLayer_Pistol_Crouch_Reload = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_CROUCH_RELOAD);

				_animLayer_Pistol_Crouch_Reload->SetEnabled(false);
				_animLayer_Pistol_Crouch_Reload->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Pistol_Crouch_Reload->AddStateMap("Pistol_Crouch_Reload", CL::ResourcePath::ANIM_PLAYER_PISTOL_CROUCH_RELOAD);
				auto _animState_Pistol_Crouch_Reload = _animLayer_Pistol_Crouch_Reload->GetState("Pistol_Crouch_Reload");
				_animState_Pistol_Crouch_Reload->AddEvent(
					[_pAudio]() {
						//0 : pistol 1 : Rifle

						_pAudio->PlayEvent("event:/Player/Player_Reload", "Reload", 0);
					},
					PLAYER_RELOAD
						);
			}
		}

		/// Swap
		{
			// Rifle
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);
				auto _animLayer_Rifle_Swap = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON);

				_animLayer_Rifle_Swap->SetEnabled(false);
				_animLayer_Rifle_Swap->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Swap->AddStateMap("SwapWeapon", CL::ResourcePath::ANIM_PLAYER_RIFLE_SWAP_WEAPON);
				auto _animState_Rifle_Swap = _animLayer_Rifle_Swap->GetState("SwapWeapon");
				_animState_Rifle_Swap->AddEvent(
					[_pAudio] {

						_pAudio->PlayEvent("event:/Player/Player_WeaponChange");
					},
					PLAYER_WEAPON_SWAP
						);

				_animState_Rifle_Swap->AddEvent(
					[obj]() {
						obj->GetComponent<PlayerController>()->m_Inventory->ChangeWeapon(
							obj->GetComponent<PlayerController>()->m_NextSlotNum
						);
						obj->GetComponent<PlayerController>()->ChangeWeaponMesh();
					},
					0.0f
						);
			}

			// Pistol
			{

				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_SWAP_WEAPON);
				auto _animLayer_Pistol_Swap = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_SWAP_WEAPON);

				_animLayer_Pistol_Swap->SetEnabled(false);
				_animLayer_Pistol_Swap->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Pistol_Swap->AddStateMap("SwapWeapon", CL::ResourcePath::ANIM_PLAYER_PISTOL_SWAP_WEAPON);
				auto _animState_Pistol_Swap = _animLayer_Pistol_Swap->GetState("SwapWeapon");
				_animState_Pistol_Swap->AddEvent(

					[_pAudio] {

						_pAudio->PlayEvent("event:/Player/Player_WeaponChange");
					},
					PLAYER_WEAPON_SWAP
						);

				_animState_Pistol_Swap->AddEvent(
					[obj]() {
						obj->GetComponent<PlayerController>()->m_Inventory->ChangeWeapon(
							obj->GetComponent<PlayerController>()->m_NextSlotNum
						);
						obj->GetComponent<PlayerController>()->ChangeWeaponMesh();
					},
					0.0f
						);
			}
		}

		/// Routing Item
		{
			// Rifle
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ROUTING_ITEM);
				auto _animLayer_Rifle_Routing_Item = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ROUTING_ITEM);

				_animLayer_Rifle_Routing_Item->SetEnabled(false);
				_animLayer_Rifle_Routing_Item->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Routing_Item->AddStateMap("Routing_Item", CL::ResourcePath::ANIM_PLAYER_RIFLE_ROUTING_ITEM);
				_animLayer_Rifle_Routing_Item->GetState("Routing_Item")->AddEvent(
					[obj]() {
						obj->GetComponent<PlayerController>()->m_bIsRoutingItem = false;
						obj->GetComponent<PlayerController>()->GetAnimator()->SetOverrideAnimLayer("");
					},
					0.99f
						);
				_animLayer_Rifle_Routing_Item->GetState("Routing_Item")->AddEvent(
					[_pAudio] {

						_pAudio->PlayEvent("event:/SFX/SFX_GetItem");
					},
					PLAYER_ITEM_ROUTING
						);
			}

			// Pistol
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ROUTING_ITEM);
				auto _animLayer_Pistol_Routing_Item = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ROUTING_ITEM);

				_animLayer_Pistol_Routing_Item->SetEnabled(false);
				_animLayer_Pistol_Routing_Item->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Pistol_Routing_Item->AddStateMap("Routing_Item", CL::ResourcePath::ANIM_PLAYER_PISTOL_ROUTING_ITEM);
				_animLayer_Pistol_Routing_Item->GetState("Routing_Item")->AddEvent(
					[obj]() {
						obj->GetComponent<PlayerController>()->m_bIsRoutingItem = false;
						obj->GetComponent<PlayerController>()->GetAnimator()->SetOverrideAnimLayer("");
					},
					0.99f
						);
				_animLayer_Pistol_Routing_Item->GetState("Routing_Item")->AddEvent(
					[_pAudio] {

						_pAudio->PlayEvent("event:/SFX/SFX_GetItem");
					},
					PLAYER_ITEM_ROUTING
						);
			}
		}

		/// Heal
		{
			// Rifle
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_HEAL);
				auto _animLayer_Rifle_Heal = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_HEAL);

				_animLayer_Rifle_Heal->SetEnabled(false);
				_animLayer_Rifle_Heal->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Rifle_Heal->AddStateMap("Heal", CL::ResourcePath::ANIM_PLAYER_RIFLE_HEAL);
				_animLayer_Rifle_Heal->GetState("Heal")->AddEvent(
					[obj]() {
						obj->GetComponent<PlayerController>()->m_bIsHeal = false;
						obj->GetComponent<PlayerController>()->GetAnimator()->SetOverrideAnimLayer("");
						obj->GetComponent<PlayerController>()->Heal();
						obj->GetComponent<PlayerController>()->PostHeal();
					},
					0.99f
						);
				_animLayer_Rifle_Heal->GetState("Heal")->AddEvent(
					[_pAudio]()
					{
						_pAudio->PlayEvent("event:/Player/Player_Healling");
					},
					PLAYER_HEAL
				);
			}

			// Pistol
			{
				_playerAnimator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_HEAL);
				auto _animLayer_Pistol_Heal = _playerAnimator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_HEAL);

				_animLayer_Pistol_Heal->SetEnabled(false);
				_animLayer_Pistol_Heal->m_MaskingType = eMASKING_TYPE::OVERRINDING;
				_animLayer_Pistol_Heal->AddStateMap("Heal", CL::ResourcePath::ANIM_PLAYER_PISTOL_HEAL);
				_animLayer_Pistol_Heal->GetState("Heal")->AddEvent(
					[obj]() {
						obj->GetComponent<PlayerController>()->m_bIsHeal = false;
						obj->GetComponent<PlayerController>()->GetAnimator()->SetOverrideAnimLayer("");
						obj->GetComponent<PlayerController>()->Heal();
						obj->GetComponent<PlayerController>()->PostHeal();
					},
					0.99f
						);
				_animLayer_Pistol_Heal->GetState("Heal")->AddEvent(
					[_pAudio]()
					{
						_pAudio->PlayEvent("event:/Player/Player_Healling");
					},
					PLAYER_HEAL
						);
			}
		}


		/// 처음 레이어 세팅
		_playerAnimator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);

		_meshObj->AddComponent<Animator>(_playerAnimator);

		/// 부모로 
		_meshObj->SetParent(obj);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/// 메인 카메라
	{
		/// 카메라 포지션(피벗) CameraParent
		GameObject* _CameraParent = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_CameraParent->SetObjectName("CameraParent");

		_CameraParent->AddComponent<OrbitCamera>(new OrbitCamera);
		_CameraParent->GetComponent<OrbitCamera>()->SetFocusTrasform(*obj);
		////////////////////////////////////////////////////////////////////////////////////////////////////


		/// 메인 카메라 오브젝트
		{
			GameObject* _MainCamera = DLLEngine::CreateObject(DLLEngine::GetNowScene());
			_MainCamera->SetObjectName("MainCamera");

			/// 부모로 
			_MainCamera->SetParent(_CameraParent);
			_MainCamera->m_Transform->SetPosition({ 0.6f, 0.23f, 0.0f });
			//m_pMainCamera->m_Transform->SetRotationFromVec({ 10.0f, 0.0f, 0.0f });

			Camera* _newCamera = new Camera(_MainCamera->GetObjectName());

			_MainCamera->AddComponent<Camera>(_newCamera);

			// CameraBehavior
			CameraBehavior* _CameraBehavior = new CameraBehavior();
			_MainCamera->AddComponent<CameraBehavior>(_CameraBehavior);

			//CameraController* _CameraController = new CameraController();
			//m_MainCamera->AddComponent<CameraController>(_CameraController);

			GameObject* _CameraCollisionPivot = DLLEngine::CreateObject(DLLEngine::GetNowScene());
			_CameraCollisionPivot->SetObjectName("CameraCollisionPivot");
			_CameraCollisionPivot->SetParent(_CameraParent);
			_CameraCollisionPivot->m_Transform->SetPosition({ 0.6f, 0.23f, 0.0f });

			////////////////////////////////////////////////////////////////////				
		}

		/// 조준 위치용 오브젝트
		GameObject* _ZoomInPosition = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_ZoomInPosition->SetObjectName("ZoomInPosition");
		_ZoomInPosition->SetParent(_CameraParent);

		//_ZoomInPosition->m_Transform->SetPosition({ 0.63f, 0.25f, 0.2f });
		_ZoomInPosition->m_Transform->SetPosition({ 0.63f, 0.25f, 0.3f });

		/// 전방 라이트 오브젝트
		GameObject* _PlayerLight = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_PlayerLight->SetObjectName("HeadLight");

		// 라이트
		Light* _light = new Light();
		_PlayerLight->AddComponent<Light>(_light);

		// 헤드라이트 컨트롤러
		_PlayerLight->AddComponent<HeadLightController>(new HeadLightController());

	}

	return obj;
}

/// <summary>
/// 힐 포션을 들고 있을 왼손 오브젝트
/// </summary>
/// <param name="obj">만들 오브젝트 포인터</param>
/// <returns></returns>
GameObject* ObjectBuilder::MakeLeftHand(GameObject* obj)
{
	/// 플레이어 매쉬 손 위치에 부착할 부모오브젝트
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("LeftHand");

	// [LeftHandController]
	obj->AddComponent<LeftHandController>(new LeftHandController());

	/// 장비중인 HealPotion Obj
	// 힐포션 복용(사용) 중에만 보이도록 렌더 On/Off 한다
	{
		// 힐 포션 위치에 맞춰 세팅
		GameObject* _EquipedMesh = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_EquipedMesh->SetObjectName("HealPotionMesh");
		_EquipedMesh->m_Transform->SetPosition({ -0.03f, 0.03f, 0.08f });
		_EquipedMesh->m_Transform->SetRotationFromVec({ 95.0f, 90.0f, 180.0f });
		_EquipedMesh->m_Transform->SetScale({ 1.0f, 1.2f, 1.0f });
		_EquipedMesh->SetParent(obj);

		MeshFilter* _meshFilter = new MeshFilter();
		_meshFilter->SetMesh(CL::ResourcePath::MESH_HEAL_POTION);
		_EquipedMesh->AddComponent<MeshFilter>(_meshFilter);
		_EquipedMesh->AddComponent<MeshRenderer>(new MeshRenderer());
	}

	return obj;
}

/// <summary>
/// 플레이어 캐릭터 메쉬의 손에 위치하는 장비 아이템 오브젝트
/// </summary>
/// <param name="obj"></param>
/// <returns></returns>
GameObject* ObjectBuilder::MakeRightHand(GameObject* obj)
{
	/// 플레이어 매쉬 손 위치에 부착할 부모오브젝트
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("RightHand");

	// [EquipmentController]

	obj->AddComponent<EquipmentController>(new EquipmentController());

	/// 장비중인 Mesh Obj
	{
		// 초기장비 주무기(라이플)에 맞춰 세팅
		GameObject* _EquipedMesh = DLLEngine::CreateObject(DLLEngine::GetNowScene());
		_EquipedMesh->SetObjectName("EquipedMesh");
		_EquipedMesh->m_Transform->SetPosition({ 0.07f, 0.0f, 0.2f });
		_EquipedMesh->m_Transform->SetRotationFromVec({ 10.0f, 100.0f, 80.0f });
		_EquipedMesh->m_Transform->SetScale({ 0.3f, 0.3f, 0.3f });
		_EquipedMesh->SetParent(obj);

		MeshFilter* _meshFilter = new MeshFilter();
		_meshFilter->SetMesh("Rifle.bin");
		_EquipedMesh->AddComponent<MeshFilter>(_meshFilter);
		_EquipedMesh->AddComponent<MeshRenderer>(new MeshRenderer());

		/// 머즐플래쉬 Obj
		{
			GameObject* _MuzzleFlashObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
			_MuzzleFlashObj->SetObjectName("MuzzleFlash");
			_MuzzleFlashObj->SetParent(_EquipedMesh);
			_MuzzleFlashObj->m_Transform->SetPosition({ 0.0f, 1.0f, 0.0f });

			// 임시 위치 확인용 메쉬 필터
			_MuzzleFlashObj->m_Transform->SetScale({ 0.1f, 0.1f, 0.1f });
			MeshFilter* _meshFilter = new MeshFilter();
			_meshFilter->SetMesh("0_Sphere.bin");
			_MuzzleFlashObj->AddComponent<MeshFilter>(_meshFilter);
			//_MuzzleFlashObj->AddComponent<MeshRenderer>(new MeshRenderer());

			// [Light]
			_MuzzleFlashObj->AddComponent<Light>(new Light);

			// [ParticleSpawner]
			ParticleSpawner* _particleSpawner = new ParticleSpawner();
			_particleSpawner->AddSpriteName("RifleFront10.png");
			_MuzzleFlashObj->AddComponent<ParticleSpawner>(_particleSpawner);

			// [MuzzleFlash]
			MuzzleFlash* _muzzleFlash = new MuzzleFlash();
			_MuzzleFlashObj->AddComponent<MuzzleFlash>(_muzzleFlash);
		}
	}

	return obj;
}

GameObject* ObjectBuilder::MakePartner(GameObject* obj)
{
	/// 최상위 부모(루트) 오브젝트 생성
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Partner");
	DLLEngine::SetTag("Partner", obj);

	/// <summary>
	/// Audio
	/// </summary>
	obj->AddComponent<Audio>(new Audio());

	PhysicsActor* _physicsActor = new PhysicsActor({ 0.15f, 0.60f, 0.15f }, RigidType::Dynamic);
	obj->AddComponent<PhysicsActor>(_physicsActor);
	_physicsActor->SetFreezeRotation(true, false, true);
	_physicsActor->SetFreezePosition(false, true, false);

	obj->AddComponent<Health>(new Health);

	Partner_Move* _Partner_Move = new Partner_Move();
	obj->AddComponent<Partner_Move>(_Partner_Move);
	obj->AddComponent<Partner_AI>(new Partner_AI());

	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);
	////////////////////////////////////////////////////////////////////////////////////

	/// 자식 (메쉬)오브젝트 생성
	GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());

	_meshObj->SetObjectName("Partner_Mesh");
	DLLEngine::SetTag("PartnerMesh", _meshObj);

	_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	_meshObj->m_Transform->SetPosition({ 0.0f, -0.55f, 0.0f });

	_meshObj->AddComponent<MeshFilter>(new MeshFilter());
	_meshObj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_PARTNER);

	_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
	_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);
	_meshObj->GetComponent<MeshRenderer>()->SetCastShadow(true);

	// 부모 설정
	_meshObj->SetParent(obj);

	// [Animator]
	// 애니메이터 생성
	Animator* _animator = new Animator();

	/// AnimLayer
	// Partner_Movemt
	_animator->AddAnimLayer(CL::ResourcePath::ANIM_LAYER_PARTNER_MOVENT);
	auto _animLayer_Partner_Movement = _animator->GetAnimLayer(CL::ResourcePath::ANIM_LAYER_PARTNER_MOVENT);
	_animLayer_Partner_Movement->SetEnabled(true);
	_animLayer_Partner_Movement->m_MaskingType = eMASKING_TYPE::NONE;

	/// State 추가
	// Stand
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE, CL::ResourcePath::ANIM_PARTNER_STAND_IDLE);
	auto _animState_Partner_Stand_Idle = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);

	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK, CL::ResourcePath::ANIM_PARTNER_STAND_WALK);
	auto _animState_Partner_Stand_Walk = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
	_animState_Partner_Stand_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 0);
		},
		PARTNER_FOOTSTEP_WALK_L
			);
	_animState_Partner_Stand_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 0);
		},
		PARTNER_FOOTSTEP_WALK_R
			);

	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_SPRINT, CL::ResourcePath::ANIM_PARTNER_STAND_SPRINT);
	auto _animState_Partner_Stand_Sprint = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_SPRINT);
	_animState_Partner_Stand_Sprint->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 2);
		},
		PARTNER_FOOTSTEP_RUN_L
			);
	_animState_Partner_Stand_Sprint->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 2);
		},
		PARTNER_FOOTSTEP_RUN_R
			);

	// Crouch
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE, CL::ResourcePath::ANIM_PARTNER_CROUCH_IDLE);
	auto _animState_Partner_Crouch_Idle = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE);

	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK, CL::ResourcePath::ANIM_PARTNER_CROUCH_WALK);
	auto _animState_Partner_Crouch_Walk = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK);
	_animState_Partner_Crouch_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 1);
		},
		PARTNER_FOOTSTEP_CROUCH_L
			);
	_animState_Partner_Crouch_Walk->AddEvent(
		[obj] {
			obj->GetComponent<Audio>()->PlayEvent("event:/Partner_Footsteps", "Partner_WCR", 1);
		},
		PARTNER_FOOTSTEP_CROUCH_R
			);

	// Hit
	// Begin
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN, CL::ResourcePath::ANIM_PARTNER_HIT_BEGIN);
	auto _animState_Partner_Hit_Begin = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
	_animState_Partner_Hit_Begin->AddEvent(
		[obj]() {
			obj->GetComponent<Partner_Move>()->m_bHitBegin = false;
			obj->GetComponent<Partner_Move>()->m_bHitMiddle = true;
		},
		0.99f
			);


	// Middle
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_MIDDLE, CL::ResourcePath::ANIM_PARTNER_HIT_MIDDLE);
	auto _animState_Partner_Hit_Middle = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_MIDDLE);

	// End
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_END, CL::ResourcePath::ANIM_PARTNER_HIT_END);
	auto _animState_Partner_Hit_End = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_END);
	_animState_Partner_Hit_End->AddEvent(
		[obj]() {
			obj->GetComponent<Partner_Move>()->m_bHitEnd = false;
			obj->GetComponent<Partner_Move>()->m_State = Partner_Move::State::eFollow;
			obj->GetComponent<PhysicsActor>()->SetFreezePosition(false, true, false);
		},
		0.99f
			);


	// Die
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_DIE, CL::ResourcePath::ANIM_PARTNER_DIE);
	auto _animState_Partner_Die = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_DIE);
	_animState_Partner_Die->AddEvent(
		[obj]() {
			obj->GetComponent<Partner_Move>()->m_bHitMiddle = false;
			obj->GetComponent<Partner_Move>()->m_bDie = false;
			obj->GetComponent<Partner_Move>()->m_bDead = true;
		},
		0.99f
			);


	// Dead
	_animLayer_Partner_Movement->AddStateMap(CL::ResourcePath::ANIM_STATE_PARTNER_DEAD, CL::ResourcePath::ANIM_PARTNER_DEAD);
	auto _animState_Partner_Dead = _animLayer_Partner_Movement->GetState(CL::ResourcePath::ANIM_STATE_PARTNER_DEAD);

	/// Transition 설정
	{
		// Stand
		{
			// ANIM_LAYER_PARTNER_STAND_IDLE
			{
				// ANIM_LAYER_PARTNER_STAND_IDLE -> HitBegin
				_animState_Partner_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Stand_Idle->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_STAND_IDLE -> Stand_Walking
				_animState_Partner_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
				_animState_Partner_Stand_Idle->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Idle->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, true);

				// ANIM_LAYER_PARTNER_STAND_IDLE -> Crouch_Idle
				_animState_Partner_Stand_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE);
				_animState_Partner_Stand_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Idle->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bCrouch, true);
			}

			// ANIM_LAYER_PARTNER_STAND_WALK
			{
				// ANIM_LAYER_PARTNER_STAND_WALK -> HitBegin
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Stand_Walk->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_STAND_WALK -> Stand_Idle
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);
				_animState_Partner_Stand_Walk->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Walk->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, false);

				// ANIM_LAYER_PARTNER_STAND_WALK -> Stand_Sprint
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_SPRINT);
				_animState_Partner_Stand_Walk->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Walk->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bSprint, true);

				// ANIM_LAYER_PARTNER_STAND_WALK -> Crouch_Walk
				_animState_Partner_Stand_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK);
				_animState_Partner_Stand_Walk->m_Transition_V->at(3)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Walk->m_Transition_V->at(3)->AddParameter(&_Partner_Move->m_bCrouch, true);
			}

			// ANIM_LAYER_PARTNER_STAND_SPRINT
			{
				// ANIM_LAYER_PARTNER_STAND_SPRINT -> HitBegin
				_animState_Partner_Stand_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Stand_Sprint->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_STAND_SPRINT -> Stand_WALK
				_animState_Partner_Stand_Sprint->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
				_animState_Partner_Stand_Sprint->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Stand_Sprint->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bSprint, false);
			}
		}

		// Crouch
		{
			// ANIM_LAYER_PARTNER_CROUCH_IDLE
			{
				// ANIM_LAYER_PARTNER_CROUCH_IDLE -> HitBegin
				_animState_Partner_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Crouch_Idle->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_CROUCH_IDLE -> Crouch_Walking
				_animState_Partner_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_WALK);
				_animState_Partner_Crouch_Idle->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Idle->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, true);

				// ANIM_LAYER_PARTNER_CROUCH_IDLE -> Stand_Idle
				_animState_Partner_Crouch_Idle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);
				_animState_Partner_Crouch_Idle->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Idle->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bCrouch, false);
			}

			// ANIM_LAYER_PARTNER_CROUCH_WALK
			{
				// ANIM_LAYER_PARTNER_CROUCH_WALK -> HitBegin
				_animState_Partner_Crouch_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
				_animState_Partner_Crouch_Walk->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitBegin, true);

				// ANIM_LAYER_PARTNER_CROUCH_WALK -> Crouch_Idle
				_animState_Partner_Crouch_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_CROUCH_IDLE);
				_animState_Partner_Crouch_Walk->m_Transition_V->at(1)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Walk->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bWalk, false);

				// ANIM_LAYER_PARTNER_CROUCH_WALK -> Stand_Walk
				_animState_Partner_Crouch_Walk->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_WALK);
				_animState_Partner_Crouch_Walk->m_Transition_V->at(2)->m_FadingPeriod = 10;
				_animState_Partner_Crouch_Walk->m_Transition_V->at(2)->AddParameter(&_Partner_Move->m_bCrouch, false);
			}
		}

		// Hit Begin -> Middle
		{
			_animState_Partner_Hit_Begin->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_MIDDLE);
			_animState_Partner_Hit_Begin->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitMiddle, true);

		}

		// Hit Middle 
		{
			// Middle -> End
			_animState_Partner_Hit_Middle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_END);
			_animState_Partner_Hit_Middle->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitEnd, true);

			// Middle -> Die
			_animState_Partner_Hit_Middle->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_DIE);
			_animState_Partner_Hit_Middle->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bDie, true);

		}

		// Hit End
		{
			// End -> Follow(Stand_Idle)
			_animState_Partner_Hit_End->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_STAND_IDLE);
			_animState_Partner_Hit_End->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bHitEnd, false);

			// End -> Begin
			_animState_Partner_Hit_End->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_HIT_BEGIN);
			_animState_Partner_Hit_End->m_Transition_V->at(1)->AddParameter(&_Partner_Move->m_bHitBegin, true);

		}

		// Die -> Dead
		{
			_animState_Partner_Die->AddTrnasition(CL::ResourcePath::ANIM_STATE_PARTNER_DEAD);
			_animState_Partner_Die->m_Transition_V->at(0)->AddParameter(&_Partner_Move->m_bDead, true);
		}

	}


	_meshObj->AddComponent<Animator>(_animator);

	////////////////////////////////////////////////////////////////////////////////////

	return obj;
}


/// <summary>
/// 러너 좀비 생성
/// 기본 컴포넌트들을 추가하고 세팅한다.
/// (각각 별도로 세팅해야하는 웨이포인트 같은 것들은 따로 해줘야 한다.)
/// </summary>
/// <param name="obj">생성할 오브젝트 포인터</param>
/// <returns>생성된 오브젝트 포인터</returns>
GameObject* ObjectBuilder::MakeZombie_Runner(GameObject* obj)
{
	static uint num = 0;
	std::string runnerNum = std::to_string(++num);

	/// 최상위 부모(루트) 오브젝트 생성
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Zombie_Runner" + runnerNum);
	DLLEngine::SetTag("Zombie", obj);

	/// <summary>
	/// Audio
	/// </summary>
	Audio* pAudio = new Audio();
	obj->AddComponent<Audio>(pAudio);

	PhysicsActor* _physicsActor = new PhysicsActor({ 0.15f, 0.45f, 0.15f }, RigidType::Dynamic);
	obj->AddComponent<PhysicsActor>(_physicsActor);
	_physicsActor->SetFreezeRotation(true, false, true);
	_physicsActor->SetFreezePosition(false, true, false);
	//obj->SetKinematic(true);

	obj->AddComponent<Health>(new Health);

	obj->AddComponent<Zombie_Runner_Move>(new Zombie_Runner_Move());
	obj->GetComponent<Zombie_Runner_Move>()->m_MoveSpeed = 6.0f;
	obj->GetComponent<Zombie_Runner_Move>()->m_DetectionRange = 6.0f;
	obj->GetComponent<Zombie_Runner_Move>()->m_ViewSight.angle = 140.0f;
	obj->AddComponent<Zombie_Runner_AI>(new Zombie_Runner_AI());

	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);
	////////////////////////////////////////////////////////////////////////////////////

	/// 자식 (메쉬)오브젝트 생성
	GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());

	std::string meshName = "Zombie_Runner" + runnerNum + "_Mesh";
	_meshObj->SetObjectName(meshName);

	DLLEngine::SetTag("ZombieMesh", _meshObj);

	_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	_meshObj->m_Transform->SetPosition({ 0.0f, -0.45f, 0.0f });

	_meshObj->AddComponent<MeshFilter>(new MeshFilter());
	_meshObj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_Zombie_RUNNER);

	_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
	_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);
	_meshObj->GetComponent<MeshRenderer>()->SetCastShadow(true);

	// 부모 설정
	_meshObj->SetParent(obj);

	// [Animator]
	// 애니메이터 생성
	Animator* _animator = new Animator();

	/// AnimLayer

	std::string Hunt = "Hunt";
	_animator->AddAnimLayer(Hunt);
	_animator->GetAnimLayer(Hunt)->SetEnabled(false);
	_animator->GetAnimLayer(Hunt)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Hunt)->AddStateMap(Hunt, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_HUNT);
	_animator->GetAnimLayer(Hunt)->GetState(Hunt)->AddEvent(
		[pAudio]() {
			pAudio->PlayEvent("event:/RZ/RZ_Chase", false);
		},
		RZOMBIE_CHASE
			);

	std::string Die = "Die";
	_animator->AddAnimLayer(Die);
	_animator->GetAnimLayer(Die)->SetEnabled(false);
	_animator->GetAnimLayer(Die)->m_MaskingType = eMASKING_TYPE::NONE;

	_animator->GetAnimLayer(Die)->AddStateMap(Die, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_DIE);
	_animator->GetAnimLayer(Die)->AddStateMap("Dead", CL::ResourcePath::ANIM_ZOMBIE_RUNNER_DEAD);

	_animator->GetAnimLayer(Die)->GetState(Die)->AddTrnasition("Dead");
	_animator->GetAnimLayer(Die)->GetState(Die)->m_Transition_V->at(0)->AddParameter(&obj->GetComponent<Zombie_Runner_Move>()->m_bIsDead, true);

	_animator->GetAnimLayer(Die)->GetState(Die)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->m_bIsDie = false;
			obj->GetComponent<Zombie_Runner_Move>()->m_bIsDead = true;
		}, 0.99f
	);
	_animator->GetAnimLayer(Die)->GetState(Die)->AddEvent(
		[pAudio]() {
			pAudio->AllEventStop();
		},
		RZOMBIE_DIE
			);


	std::string patrol = "Patrol";
	_animator->AddAnimLayer(patrol);
	_animator->GetAnimLayer(patrol)->SetEnabled(true);
	_animator->GetAnimLayer(patrol)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(patrol)->AddStateMap(patrol, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_PATROL);
	_animator->GetAnimLayer(patrol)->GetState(patrol)->AddEvent(
		[pAudio]() {

			pAudio->PlayEvent("event:/RZ/RZ_Move", false);
		},
		RZOMBIE_IDLE
			);

	std::string Wait = "Wait";
	_animator->AddAnimLayer(Wait);
	_animator->GetAnimLayer(Wait)->SetEnabled(false);
	_animator->GetAnimLayer(Wait)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Wait)->AddStateMap(Wait, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_WAIT);
	_animator->GetAnimLayer(Wait)->GetState(Wait)->AddEvent(
		[pAudio]() {

			pAudio->PlayEvent("event:/RZ/RZ_Move", false);
		},
		RZOMBIE_IDLE
			);

	std::string AttackPartner = "AttackPartner";
	_animator->AddAnimLayer(AttackPartner);
	_animator->GetAnimLayer(AttackPartner)->SetEnabled(false);
	_animator->GetAnimLayer(AttackPartner)->m_MaskingType = eMASKING_TYPE::NONE;
	{
		std::string AttackPartner_Begin = "AttackPartner_Begin";
		std::string AttackPartner_Middle = "AttackPartner_Middle";
		std::string AttackPartner_End = "AttackPartner_End";

		_animator->GetAnimLayer(AttackPartner)->AddStateMap(AttackPartner_Begin, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_BEGIN);
		auto _AttackPartner_Begin = _animator->GetAnimLayer(AttackPartner)->GetState(AttackPartner_Begin);
		_AttackPartner_Begin->AddEvent(
			[obj]() {
				obj->GetComponent<Zombie_Runner_Move>()->m_bIsAttackPartner_Middle = true;
			},
			0.99
				);
		_AttackPartner_Begin->AddEvent(
			[pAudio]() {

				pAudio->PlayEvent("event:/RZ/RZ_PartnerAtk");
			},
			RZOMBIE_PARTER_ATTACK
				);
		_animator->GetAnimLayer(AttackPartner)->AddStateMap(AttackPartner_Middle, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_MIDDLE);
		auto _AttackPartner_Middle = _animator->GetAnimLayer(AttackPartner)->GetState(AttackPartner_Middle);
		_AttackPartner_Middle->AddEvent(
			[pAudio]() {

				pAudio->PlayEvent("event:/RZ/RZ_PartnerAtk");
			},
			RZOMBIE_PARTER_ATTACK
				);

		_animator->GetAnimLayer(AttackPartner)->AddStateMap(AttackPartner_End, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PARTNER_END);
		auto _AttackPartner_End = _animator->GetAnimLayer(AttackPartner)->GetState(AttackPartner_End);
		_AttackPartner_End->AddEvent(
			[obj]() {
				obj->GetComponent<Zombie_Runner_Move>()->m_State &= ~Zombie_Runner_Move::State::eAttackPartner;
				obj->GetComponent<Zombie_Runner_Move>()->m_bIsAttackPartner_End = false;

				obj->GetComponent<Zombie_Runner_Move>()->m_State |= Zombie_Runner_Move::State::eReturn;
			},
			0.99
				);


		/// Transition
		// Begin -> Middle
		{
			_AttackPartner_Begin->AddTrnasition(AttackPartner_Middle);
			_AttackPartner_Begin->m_Transition_V->at(0)->AddParameter(&obj->GetComponent< Zombie_Runner_Move>()->m_bIsAttackPartner_Middle, true);
		}

		// Middle -> End
		{
			_AttackPartner_Middle->AddTrnasition(AttackPartner_End);
			_AttackPartner_Middle->m_Transition_V->at(0)->AddParameter(&obj->GetComponent< Zombie_Runner_Move>()->m_bIsAttackPartner_End, true);
		}
	}

	std::string AttackPlayer = "AttackPlayer";
	_animator->AddAnimLayer(AttackPlayer);
	_animator->GetAnimLayer(AttackPlayer)->SetEnabled(false);
	_animator->GetAnimLayer(AttackPlayer)->m_MaskingType = eMASKING_TYPE::NONE;

	_animator->GetAnimLayer(AttackPlayer)->AddStateMap(AttackPlayer, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ATTACK_PLAYER);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->DamageToPlayer();
		}, 0.50f
	);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAttackPlayer();
		}, 0.99f
	);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[pAudio]() {

			pAudio->PlayEvent("event:/RZ/RZ_Attack");
		},
		RZOMBIE_PLAYER_ATTACK
			);

	std::string AwakenSight = "AwakenSight";
	_animator->AddAnimLayer(AwakenSight);
	_animator->GetAnimLayer(AwakenSight)->SetEnabled(false);
	_animator->GetAnimLayer(AwakenSight)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(AwakenSight)->AddStateMap(AwakenSight, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_AWAKEN_SIGHT);
	_animator->GetAnimLayer(AwakenSight)->GetState(AwakenSight)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAwakenSight();

		}, 0.99f
	);
	_animator->GetAnimLayer(AwakenSight)->GetState(AwakenSight)->AddEvent(
		[pAudio]() {
			//0: sound
			//1: sight 

			pAudio->PlayEvent("event:/RZ/RZ_Find", "RZ_Find", 1);
		},
		RZOMBIE_FIND
			);

	std::string AwakenSound = "AwakenSound";
	_animator->AddAnimLayer(AwakenSound);
	_animator->GetAnimLayer(AwakenSound)->SetEnabled(false);
	_animator->GetAnimLayer(AwakenSound)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(AwakenSound)->AddStateMap(AwakenSound, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_AWAKEN_SOUNED);
	_animator->GetAnimLayer(AwakenSound)->GetState(AwakenSound)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAwakenSound();
		},
		0.99f
			);
	_animator->GetAnimLayer(AwakenSound)->GetState(AwakenSound)->AddEvent(
		[pAudio]() {
			//0: sound
			//1: sight 
			pAudio->PlayEvent("event:/RZ/RZ_Find", "RZ_Find", 0);
		},
		RZOMBIE_FIND
			);

	std::string Assassinated = "Assassinated";
	_animator->AddAnimLayer(Assassinated);
	_animator->GetAnimLayer(Assassinated)->SetEnabled(false);
	_animator->GetAnimLayer(Assassinated)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Assassinated)->AddStateMap(Assassinated, CL::ResourcePath::ANIM_ZOMBIE_RUNNER_ASSASSINATED);
	_animator->GetAnimLayer(Assassinated)->GetState(Assassinated)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Runner_Move>()->PostAssassinated();

		}, 0.99f
	);
	_animator->GetAnimLayer(Assassinated)->GetState(Assassinated)->AddEvent(
		[pAudio]() {

			pAudio->PlayEvent("event:/Player/Player_Assassin", "Assassin", 1, false);
		},
		RZOMBIE_ASSASSIN
			);
	_meshObj->AddComponent<Animator>(_animator);
	////////////////////////////////////////////////////////////////////////////////////

	return obj;
}

/// <summary>
/// 클리커 좀비 생성
/// 기본 컴포넌트들을 추가하고 세팅한다.
/// (각각 별도로 세팅해야하는 웨이포인트 같은 것들은 따로 해줘야 한다.)
/// </summary>
/// <param name="obj">생성할 오브젝트 포인터</param>
/// <returns>생성된 오브젝트 포인터</returns>
GameObject* ObjectBuilder::MakeZombie_Clicker(GameObject* obj)
{
	static uint num = 0;
	std::string clickerNum = std::to_string(++num);

	/// 최상위 부모(루트) 오브젝트 생성
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("Zombie_Clicker" + clickerNum);
	DLLEngine::SetTag("Zombie", obj);

	PhysicsActor* _physicsActor = new PhysicsActor({ 0.15f, 0.45f, 0.15f }, RigidType::Dynamic);
	obj->AddComponent<PhysicsActor>(_physicsActor);
	_physicsActor->SetFreezeRotation(true, false, true);
	_physicsActor->SetFreezePosition(false, true, false);
	//obj->SetKinematic(true);

	/// <summary>
	/// Audio
	/// </summary>
	Audio* pAudio = new Audio();
	obj->AddComponent<Audio>(pAudio);

	obj->AddComponent<Health>(new Health);

	obj->AddComponent<Zombie_Clicker_Move>(new Zombie_Clicker_Move());
	obj->GetComponent<Zombie_Clicker_Move>()->m_MoveSpeed = 4.0f;
	obj->AddComponent<Zombie_Clicker_AI>(new Zombie_Clicker_AI());

	obj->AddComponent<NavMeshAgent>(new NavMeshAgent());
	obj->GetComponent<NavMeshAgent>()->SetDebugMode(true);
	////////////////////////////////////////////////////////////////////////////////////

	/// 자식 (메쉬)오브젝트 생성
	GameObject* _meshObj = DLLEngine::CreateObject(DLLEngine::GetNowScene());

	std::string meshName = "Zombie_Clicker" + clickerNum + "_Mesh";
	_meshObj->SetObjectName(meshName);

	DLLEngine::SetTag("ZombieMesh", _meshObj);

	_meshObj->m_Transform->SetRotationFromVec({ 0.0f, 180.0f, 0.0f });
	_meshObj->m_Transform->SetPosition({ 0.0f, -0.45f, 0.0f });

	_meshObj->AddComponent<MeshFilter>(new MeshFilter());
	_meshObj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_Zombie_Clicker);

	_meshObj->AddComponent<MeshRenderer>(new MeshRenderer);
	_meshObj->GetComponent<MeshRenderer>()->SetGizmo(false);
	_meshObj->GetComponent<MeshRenderer>()->SetCastShadow(true);

	// 부모 설정
	_meshObj->SetParent(obj);

	// [Animator]
	// 애니메이터 생성
	Animator* _animator = new Animator();

	/// AnimLayer
	std::string Die = "Die";
	_animator->AddAnimLayer(Die);
	_animator->GetAnimLayer(Die)->SetEnabled(false);
	_animator->GetAnimLayer(Die)->m_MaskingType = eMASKING_TYPE::NONE;

	_animator->GetAnimLayer(Die)->AddStateMap(Die, CL::ResourcePath::ANIM_ZOMBIE_CLICKER_DIE);
	_animator->GetAnimLayer(Die)->AddStateMap("Dead", CL::ResourcePath::ANIM_ZOMBIE_CLICKER_DEAD);

	_animator->GetAnimLayer(Die)->GetState(Die)->AddTrnasition("Dead");
	_animator->GetAnimLayer(Die)->GetState(Die)->m_Transition_V->at(0)->AddParameter(&obj->GetComponent<Zombie_Clicker_Move>()->m_bIsDead, true);

	_animator->GetAnimLayer(Die)->GetState(Die)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Clicker_Move>()->m_bIsDie = false;
			obj->GetComponent<Zombie_Clicker_Move>()->m_bIsDead = true;
		}, 0.99f
	);
	_animator->GetAnimLayer(Die)->GetState(Die)->AddEvent(
		[pAudio]() {
			pAudio->AllEventStop();
		},
		0.99f
			);

	std::string patrol = "Patrol";
	_animator->AddAnimLayer(patrol);
	_animator->GetAnimLayer(patrol)->SetEnabled(true);
	_animator->GetAnimLayer(patrol)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(patrol)->AddStateMap(patrol, CL::ResourcePath::ANIM_ZOMBIE_CLICKER_PATROL);
	_animator->GetAnimLayer(patrol)->GetState(patrol)->AddEvent(
		[pAudio]() {
			pAudio->PlayEvent("event:/CZ/CZ_Move");

		},
		CZOMBIE_IDLE
			);

	std::string Wait = "Wait";
	_animator->AddAnimLayer(Wait);
	_animator->GetAnimLayer(Wait)->SetEnabled(false);
	_animator->GetAnimLayer(Wait)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Wait)->AddStateMap(Wait, CL::ResourcePath::ANIM_ZOMBIE_CLICKER_WAIT);
	_animator->GetAnimLayer(Wait)->GetState(Wait)->AddEvent(
		[pAudio]() {
			pAudio->PlayEvent("event:/CZ/CZ_Move");
		},
		CZOMBIE_IDLE
			);

	std::string AttackPlayer = "AttackPlayer";
	_animator->AddAnimLayer(AttackPlayer);
	_animator->GetAnimLayer(AttackPlayer)->SetEnabled(false);
	_animator->GetAnimLayer(AttackPlayer)->m_MaskingType = eMASKING_TYPE::NONE;

	_animator->GetAnimLayer(AttackPlayer)->AddStateMap(AttackPlayer, CL::ResourcePath::ANIM_ZOMBIE_CLICKER_ATTACK_PLAYER);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Clicker_Move>()->DamageToPlayer();
			obj->GetComponent<Zombie_Clicker_Move>()->PostAttackPlayer();
		}, 0.99f
	);
	_animator->GetAnimLayer(AttackPlayer)->GetState(AttackPlayer)->AddEvent(
		[pAudio]() {
			pAudio->PlayEvent("event:/CZ/CZ_Attack");
		},
		CZOMBIE_ATTACK
			);


	std::string AwakenSound = "AwakenSound";
	_animator->AddAnimLayer(AwakenSound);
	_animator->GetAnimLayer(AwakenSound)->SetEnabled(false);
	_animator->GetAnimLayer(AwakenSound)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(AwakenSound)->AddStateMap(AwakenSound, CL::ResourcePath::ANIM_ZOMBIE_CLICKER_AWAKEN_SOUNED);
	_animator->GetAnimLayer(AwakenSound)->GetState(AwakenSound)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Clicker_Move>()->PostAwakenSound();
		},
		0.99f
			);
	_animator->GetAnimLayer(AwakenSound)->GetState(AwakenSound)->AddEvent(
		[pAudio]() {

			pAudio->PlayEvent("event:/CZ/CZ_Find");
		},
		CZOMBIE_FIND
			);
	std::string Assassinated = "Assassinated";
	_animator->AddAnimLayer(Assassinated);
	_animator->GetAnimLayer(Assassinated)->SetEnabled(false);
	_animator->GetAnimLayer(Assassinated)->m_MaskingType = eMASKING_TYPE::NONE;
	_animator->GetAnimLayer(Assassinated)->AddStateMap(Assassinated, CL::ResourcePath::ANIM_ZOMBIE_CLICKER_ASSASSINATED);
	_animator->GetAnimLayer(Assassinated)->GetState(Assassinated)->AddEvent(
		[obj]() {
			obj->GetComponent<Zombie_Clicker_Move>()->PostAssassinated();

		}, 0.99f
	);
	_animator->GetAnimLayer(Assassinated)->GetState(Assassinated)->AddEvent(
		[pAudio]() {

			pAudio->PlayEvent("event:/Player/Player_Assassin", "Assassin", 0, false);
		},
		CZOMBIE_ASSASSIN
			);
	_meshObj->AddComponent<Animator>(_animator);
	////////////////////////////////////////////////////////////////////////////////////


	return obj;
}

GameObject* ObjectBuilder::MakeWayPoint(GameObject* obj)
{
	static uint num = 0;
	std::string waypointNum = std::to_string(++num);

	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("WayPoint_" + waypointNum);
	obj->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh("0_Sphere.bin");

	//obj->AddComponent<MeshRenderer>(new MeshRenderer);
	//obj->GetComponent<MeshRenderer>()->SetGizmo(true);

	return obj;
}

GameObject* ObjectBuilder::MakeLongGunAmmo(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("LongGunAmmo");
	DLLEngine::SetTag("DeployedItem", obj);

	obj->m_Transform->SetScale({ 0.7f, 0.7f, 0.7f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_AMMO);

	obj->AddComponent<MeshRenderer>(new MeshRenderer());

	return obj;
}

GameObject* ObjectBuilder::MakeHandGunAmmo(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("HandGunAmmo");
	DLLEngine::SetTag("DeployedItem", obj);

	obj->m_Transform->SetScale({ 0.7f, 0.7f, 0.7f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_AMMO);

	obj->AddComponent<MeshRenderer>(new MeshRenderer());

	return obj;
}

GameObject* ObjectBuilder::MakeCardKey(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("CardKey");
	DLLEngine::SetTag("DeployedItem", obj);

	obj->m_Transform->SetScale({ 2.0f, 2.0f, 2.0f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_CARD_KEY);

	obj->AddComponent<MeshRenderer>(new MeshRenderer());

	return obj;
}

GameObject* ObjectBuilder::MakeFile(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("File");
	DLLEngine::SetTag("DeployedItem", obj);

	//obj->m_Transform->SetScale({ 0.2f, 0.2f, 0.2f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_FILE);

	obj->AddComponent<MeshRenderer>(new MeshRenderer());

	return obj;
}

GameObject* ObjectBuilder::MakeHealPotion(GameObject* obj)
{
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("HealPotion");
	DLLEngine::SetTag("DeployedItem", obj);


	obj->m_Transform->SetScale({ 2.0f, 2.0f, 2.0f });

	obj->AddComponent<MeshFilter>(new MeshFilter());
	obj->GetComponent<MeshFilter>()->SetMesh(CL::ResourcePath::MESH_HEAL_POTION);

	obj->AddComponent<MeshRenderer>(new MeshRenderer());

	return obj;
}

GameObject* ObjectBuilder::MakeIngameUI(GameObject* obj, bool* isPause)
{
	/// UI 루트 컨테이너(Canvas 같은거)
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("IngameUI");
	obj->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });

	//////////////////////////////////////////////////////////////////////

	/// CSV 맵을 받아오는 역할
	GameObject* _CSVLoader = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_CSVLoader->SetObjectName("CSVLoader");

	CSVLoader* _csvLoader = new CSVLoader();
	_csvLoader->SetSceneKind(DLLEngine::GetNowScene()->GetSceneKind());

	_CSVLoader->AddComponent<CSVLoader>(_csvLoader);
	_CSVLoader->SetParent(obj);

	//////////////////////////////////////////////////////////////////////

	GameObject* _DialogueManager = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_DialogueManager->SetObjectName("Dialogue Manager");

	Text* _DialogueText1 = new Text();
	_DialogueText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		0.f, 500.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f,
		eUIAxis::Center,
		eTextPoint::Center);

	_DialogueManager->AddComponent<DialogueManager>(new DialogueManager());
	_DialogueManager->AddComponent<Text>(_DialogueText1);
	_DialogueManager->SetParent(obj);

	//////////////////////////////////////////////////////////////////////

	GameObject* _aimPoint = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_aimPoint->SetObjectName("Aim Point");

	Sprite2D* _aimSptire = new Sprite2D;
	_aimSptire->SetType(eResourceType::eSingleImage);
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_Off.png");
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_On.png"); 
	_aimSptire->m_SpriteName_V.push_back("UI_AimPoint_Hit.png"); 
	_aimSptire->SetProportion(1.f, 1.f);
	_aimSptire->SetPivot(0.5f, 0.5f);
	_aimSptire->SetUIAxis(eUIAxis::Center);

	_aimPoint->AddComponent<Sprite2D>(_aimSptire);
	_aimPoint->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	/// 퀘스트 텍스트(텍스트 X2, sprite) 
	///퀘스트 제목(퀘스트 매니저도 붙인다)
	GameObject* _questText = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_questText->SetObjectName("Quest Text");

	Sprite2D* _QuestSptire = new Sprite2D;
	_QuestSptire->SetType(eResourceType::eSingleImage);
	_QuestSptire->m_SpriteName_V.push_back("Quest_Text_Line.png");
	_QuestSptire->SetProportion(1.0f, 1.0f);
	_QuestSptire->SetPivot(0.5f, 0.5f);
	_QuestSptire->SetUIAxis(eUIAxis::RightUp);

	// (( "발전기 수리" ))
	Text* _QuestText1 = new Text();
	_QuestText1->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		190.f, 40.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 40.f,
		eUIAxis::RightUp,
		eTextPoint::Center);

	_questText->AddComponent<QuestManager>(new QuestManager());
	_questText->AddComponent<Sprite2D>(_QuestSptire);
	_questText->AddComponent<Text>(_QuestText1);
	_questText->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	///	퀘스트 내용
	GameObject* _questText_Sub = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_questText_Sub->SetObjectName("Quest Text Sub");

	Text* _QuestText2 = new Text();
	_QuestText2->SetSpriteTextInfo(
		L"../Data/Fonts/Font1.ttf",
		190.f, 100.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 26.f,
		eUIAxis::RightUp,
		eTextPoint::Center);

	_questText_Sub->AddComponent<Text>(_QuestText2);
	_questText_Sub->SetParent(_questText);
	////////////////////////////////////////////////////////////////////////

	float _battleStatePosX = 535.f;
	float _battleStatePosY = 150.f;

	///
	/// 전투 스탯
	///
	//(스프라이트 X6, 텍스트 X6, 스프라이트(life) X6)
	GameObject* _combatStat = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combatStat->SetObjectName("Combet Stat");

	/// 장착 무기
	GameObject* _combatStat_Weapon = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combatStat_Weapon->SetObjectName("Combet Weapon Stat");

	Sprite2D* _WeaponSptire = new Sprite2D;
	_WeaponSptire->SetType(eResourceType::eSingleImage);
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Pistol.png");
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Riflel.png");
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Flask.png");
	_WeaponSptire->m_SpriteName_V.push_back("UI_Weapon_Fire.png");
	_WeaponSptire->SetProportion(1.f, 1.f);
	_WeaponSptire->SetPivot(1.0f, 1.0f);
	_WeaponSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _WeaponText1 = new Text();
	_WeaponText1->SetSpriteTextInfo(
		L"../Data/Fonts/malgun.ttf",
		_battleStatePosX - 23.f, _battleStatePosY - 48.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combatStat_Weapon->AddComponent<Sprite2D>(_WeaponSptire);
	_combatStat_Weapon->AddComponent<Text>(_WeaponText1);
	_combatStat_Weapon->SetParent(obj);

	/// 장착 무기 남은 수량 텍스트
	GameObject* _combatStat_Weapon_sub_Text = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combatStat_Weapon_sub_Text->SetObjectName("Combet Weapon Stat Sub Text");

	Text* _WeaponText2 = new Text();
	_WeaponText2->SetSpriteTextInfo(
		L"../Data/Fonts/malgun.ttf",
		_battleStatePosX - 45.f, _battleStatePosY - 61.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 28.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combatStat_Weapon_sub_Text->AddComponent<Text>(_WeaponText2);
	_combatStat_Weapon_sub_Text->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 화염병
	GameObject* _combatStat_Fire = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combatStat_Fire->SetObjectName("Combet Fire Stat");

	Sprite2D* _FireSptire = new Sprite2D;
	_FireSptire->SetType(eResourceType::eSingleImage);
	_FireSptire->m_SpriteName_V.push_back("UI_Item_Bottle_On.png");
	_FireSptire->m_SpriteName_V.push_back("UI_Item_Bottle_Off.png");
	_FireSptire->SetProportion(1.f, 1.f);
	_FireSptire->SetPivot(1.0f, 1.0f);
	_FireSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _FireText = new Text();
	_FireText->SetSpriteTextInfo(
		L"../Data/Fonts/malgun.ttf",
		_battleStatePosX - 232.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combatStat_Fire->AddComponent<Sprite2D>(_FireSptire);
	_combatStat_Fire->AddComponent<Text>(_FireText);
	_combatStat_Fire->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 힐
	GameObject* _combatStat_Heal = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combatStat_Heal->SetObjectName("Combet Heal");

	Sprite2D* _HealSptire = new Sprite2D;
	_HealSptire->SetType(eResourceType::eSingleImage);
	_HealSptire->m_SpriteName_V.push_back("UI_Item_Heal_On.png");
	_HealSptire->m_SpriteName_V.push_back("UI_Item_Heal_Off.png");
	_HealSptire->SetProportion(1.f, 1.f);
	_HealSptire->SetPivot(1.0f, 1.0f);
	_HealSptire->SetUIAxis(eUIAxis::RightDown);

	Text* _HealText = new Text();
	_HealText->SetSpriteTextInfo(
		L"../Data/Fonts/malgun.ttf",
		_battleStatePosX - 312.f, _battleStatePosY - 65.f,
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 22.f,
		eUIAxis::RightDown,
		eTextPoint::LeftUP);

	_combatStat_Heal->AddComponent<Sprite2D>(_HealSptire);
	_combatStat_Heal->AddComponent<Text>(_HealText);
	_combatStat_Heal->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// 조수 수신호
	GameObject* _combatStat_Assist = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combatStat_Assist->SetObjectName("Combet AssistState");

	Sprite2D* _AssistSptire = new Sprite2D;
	_AssistSptire->SetType(eResourceType::eSingleImage);
	_AssistSptire->m_SpriteName_V.push_back("UI_Partner_Seperate2.png");
	_AssistSptire->m_SpriteName_V.push_back("UI_Partner_Seperate2(caution).png");
	_AssistSptire->m_SpriteName_V.push_back("UI_Partner_With.png");
	_AssistSptire->m_SpriteName_V.push_back("UI_Partner_With(caution).png");
	_AssistSptire->SetProportion(1.f, 1.f);
	_AssistSptire->SetPivot(1.0f, 1.0f);
	_AssistSptire->SetUIAxis(eUIAxis::RightDown);

	_combatStat_Assist->AddComponent<Sprite2D>(_AssistSptire);
	_combatStat_Assist->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	///
	/// 플레이어 Life
	///
	GameObject* _Life = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Life->SetObjectName("Player Life Stat");

	Sprite2D* _LifeSptire = new Sprite2D;
	_LifeSptire->SetType(eResourceType::eSingleImage);
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_0.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_1.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_2.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_3.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_4.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_5.png");
	_LifeSptire->m_SpriteName_V.push_back("UI_HP_Player_6.png");
	_LifeSptire->SetProportion(1.0f, 1.0f);
	_LifeSptire->SetPivot(1.0f, 1.0f);
	_LifeSptire->SetUIAxis(eUIAxis::RightDown);

	_Life->AddComponent<Sprite2D>(_LifeSptire);
	_Life->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	///
	/// 암살UI
	///
	GameObject* _AssassinIcon = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_AssassinIcon->SetObjectName("AssassinIcon");
	_AssassinIcon->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _AssassinSprite = new Billboard;
	_AssassinSprite->SeteResourceType(eResourceType::eSingleImage);
	_AssassinSprite->SetRotationType(eRotationType::LookAt);
	_AssassinSprite->m_SpriteName_V.push_back("UI_Assassin.png");
	_AssassinSprite->SetProportion(0.0015f, 0.0015f);
	_AssassinSprite->SetPivot(1.0f, 1.0f);
	_AssassinSprite->OnDisable();

	_AssassinIcon->AddComponent<Billboard>(_AssassinSprite);
	_AssassinIcon->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	/// 
	/// 조수 Life(꺼져있는것이 default)
	///
	GameObject* _AssistLifeBar = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_AssistLifeBar->SetObjectName("Assist Life Bar");
	_AssistLifeBar->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _AssistLifeBarSptire = new Billboard;
	_AssistLifeBarSptire->SeteResourceType(eResourceType::eSingleImage);
	_AssistLifeBarSptire->SetRotationType(eRotationType::LookAt);
	_AssistLifeBarSptire->m_SpriteName_V.push_back("UI_Struggle2.png");
	_AssistLifeBarSptire->SetProportion(0.001f, 0.001f);
	_AssistLifeBarSptire->SetPivot(0.5f, 0.5f);
	//_AssistLifeBarSptire->OnDisable();

	_AssistLifeBar->AddComponent<Billboard>(_AssistLifeBarSptire);
	_AssistLifeBar->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	/// 조수 생명(10초) Red
	GameObject* _AssistLife = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_AssistLife->SetObjectName("Assist Life");
	_AssistLife->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _AssistLifeSptire = new Billboard;
	_AssistLifeSptire->SeteResourceType(eResourceType::eSingleImage);
	_AssistLifeSptire->SetRotationType(eRotationType::LookAt);
	_AssistLifeSptire->m_SpriteName_V.push_back("UI_Struggle_Gage2.png");
	_AssistLifeSptire->SetProportion(0.001f, 0.001f);
	_AssistLifeSptire->SetPivot(1.0f, 1.0f);
	//_AssistLifeSptire->OnDisable();

	_AssistLife->AddComponent<Billboard>(_AssistLifeSptire);
	_AssistLife->SetParent(obj);
	////////////////////////////////////////////////////////////////////////

	/// ---------------------------------------------------
	/// 인게임 상황에 맞게 껐다 켜야함
	/// ---------------------------------------------------

	/// 서류 스프라이트
	GameObject* _item_Document = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_item_Document->SetObjectName("BB_Document");

	Sprite2D* _DocumentSptire = new Sprite2D;
	_DocumentSptire->SetType(eResourceType::eSingleImage);
	_DocumentSptire->m_SpriteName_V.push_back("Document.png");
	_DocumentSptire->SetProportion(1.0f, 1.0f);
	_DocumentSptire->SetPivot(0.5f, 0.5f);
	_DocumentSptire->SetUIAxis(eUIAxis::BackGround);
	_DocumentSptire->OnDisable();

	_item_Document->AddComponent<Sprite2D>(_DocumentSptire);
	_item_Document->SetParent(obj);


	/////////////////////////////////////////////////////////////////////////////
	/// 피격시 피효과

	GameObject* _combat_Blood = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_combat_Blood->SetObjectName("Combet Blood");

	Sprite2D* _BloodSptire = new Sprite2D;
	_BloodSptire->SetType(eResourceType::eSingleImage);
	_BloodSptire->m_SpriteName_V.push_back("HitBlood_01.png");
	_BloodSptire->m_SpriteName_V.push_back("HitBlood_02.png");
	_BloodSptire->SetProportion(1.0f, 1.0f);
	_BloodSptire->SetPivot(0.5f, 0.5f);
	_BloodSptire->SetUIAxis(eUIAxis::BackGround);

	PlaneAnimation* _BloodAni = new PlaneAnimation;
	_BloodAni->SetFPS(eFrameRate::FPS24, 8);

	_combat_Blood->AddComponent<Sprite2D>(_BloodSptire);
	_combat_Blood->AddComponent<PlaneAnimation>(_BloodAni);
	_combat_Blood->SetParent(obj);
	
	/*
	/////////////////////////////////////////////////////////////////////////////
	*/
	/// ---------------------------------------------------
	/// 빌보드 : 인게임 상황에 맞게 껐다 켜야함
	/// ---------------------------------------------------
	/// 1. 힐템 획득 (빌보드)
	GameObject* _BB_Heal = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_Heal->SetObjectName("BB_Heal");
	_BB_Heal->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_HealSptire = new Billboard;
	_BB_HealSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_HealSptire->SetRotationType(eRotationType::LookAt);
	_BB_HealSptire->m_SpriteName_V.push_back("UI_Field_Item_Heal.png");
	_BB_HealSptire->SetProportion(0.002f, 0.002f);
	_BB_HealSptire->SetPivot(0.5f, 0.5f);
	_BB_HealSptire->OnDisable();

	_BB_Heal->AddComponent<Billboard>(_BB_HealSptire);
	_BB_Heal->SetParent(obj);

	/// 2. 시드볼트 획득 (빌보드)
	GameObject* _BB_SeedVault = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_SeedVault->SetObjectName("BB_SeedVault");
	_BB_SeedVault->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_SeedVaultSptire = new Billboard;
	_BB_SeedVaultSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_SeedVaultSptire->SetRotationType(eRotationType::LookAt);
	_BB_SeedVaultSptire->m_SpriteName_V.push_back("UI_FieldAction1.png");
	_BB_SeedVaultSptire->SetProportion(0.001f, 0.001f);
	_BB_SeedVaultSptire->SetPivot(0.5f, 0.5f);
	_BB_SeedVaultSptire->OnDisable();

	_BB_SeedVault->AddComponent<Billboard>(_BB_SeedVaultSptire);
	_BB_SeedVault->SetParent(obj);

	/// 3. 권총탄 획득 (빌보드)
	GameObject* _BB_HandGun = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_HandGun->SetObjectName("BB_HandGun");
	_BB_HandGun->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_HandGunSptire = new Billboard;
	_BB_HandGunSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_HandGunSptire->SetRotationType(eRotationType::LookAt);
	_BB_HandGunSptire->m_SpriteName_V.push_back("UI_Field_Pistol.png");
	_BB_HandGunSptire->SetProportion(0.0022f, 0.0022f);
	_BB_HandGunSptire->SetPivot(0.5f, 0.5f);
	_BB_HandGunSptire->OnDisable();

	_BB_HandGun->AddComponent<Billboard>(_BB_HandGunSptire);
	_BB_HandGun->SetParent(obj);

	/// 4. 장총탄 획득 (빌보드)
	GameObject* _BB_Rifle = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_Rifle->SetObjectName("BB_Rifle");
	_BB_Rifle->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_RifleSptire = new Billboard;
	_BB_RifleSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_RifleSptire->SetRotationType(eRotationType::LookAt);
	_BB_RifleSptire->m_SpriteName_V.push_back("UI_Field_Rifle.png");
	_BB_RifleSptire->SetProportion(0.0022f, 0.0022f);
	_BB_RifleSptire->SetPivot(0.5f, 0.5f);
	_BB_RifleSptire->OnDisable();

	_BB_Rifle->AddComponent<Billboard>(_BB_RifleSptire);
	_BB_Rifle->SetParent(obj);

	/// 5. 카드키 획득 (빌보드)
	GameObject* _BB_CardKeyGet = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_CardKeyGet->SetObjectName("BB_CardKeyGet");
	_BB_CardKeyGet->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_CardKeyGetSptire = new Billboard;
	_BB_CardKeyGetSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_CardKeyGetSptire->SetRotationType(eRotationType::LookAt);
	_BB_CardKeyGetSptire->m_SpriteName_V.push_back("UI_FieldAction1.png");
	_BB_CardKeyGetSptire->SetProportion(0.001f, 0.001f);
	_BB_CardKeyGetSptire->SetPivot(0.5f, 0.5f);
	_BB_CardKeyGetSptire->OnDisable();

	_BB_CardKeyGet->AddComponent<Billboard>(_BB_CardKeyGetSptire);
	_BB_CardKeyGet->SetParent(obj);

	/// 6.카드키 대기 (빌보드)
	GameObject* _BB_CardKeyWait = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_CardKeyWait->SetObjectName("BB_CardKeyWait");
	_BB_CardKeyWait->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_CardKeyWaitSptire = new Billboard;
	_BB_CardKeyWaitSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_CardKeyWaitSptire->SetRotationType(eRotationType::LookAt);
	_BB_CardKeyWaitSptire->m_SpriteName_V.push_back("UI_FieldAction2.png");
	_BB_CardKeyWaitSptire->SetProportion(0.001f, 0.001f);
	_BB_CardKeyWaitSptire->SetPivot(0.5f, 0.5f);
	_BB_CardKeyWaitSptire->OnDisable();

	_BB_CardKeyWait->AddComponent<Billboard>(_BB_CardKeyWaitSptire);
	_BB_CardKeyWait->SetParent(obj);

	/// 7.메뉴얼 획득 (빌보드)
	GameObject* _BB_ManualGet = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_ManualGet->SetObjectName("BB_ManualGet");
	_BB_ManualGet->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_ManualGetSptire = new Billboard;
	_BB_ManualGetSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_ManualGetSptire->SetRotationType(eRotationType::LookAt);
	_BB_ManualGetSptire->m_SpriteName_V.push_back("UI_FieldAction3.png");
	_BB_ManualGetSptire->SetProportion(0.001f, 0.001f);
	_BB_ManualGetSptire->SetPivot(0.5f, 0.5f);
	_BB_ManualGetSptire->OnDisable();

	_BB_ManualGet->AddComponent<Billboard>(_BB_ManualGetSptire);
	_BB_ManualGet->SetParent(obj);

	/// 8.패널 조정 (빌보드)
	GameObject* _BB_PanelSetting = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_PanelSetting->SetObjectName("BB_PanelSetting");
	_BB_PanelSetting->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_PanelSettingSptire = new Billboard;
	_BB_PanelSettingSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_PanelSettingSptire->SetRotationType(eRotationType::LookAt);
	_BB_PanelSettingSptire->m_SpriteName_V.push_back("UI_FieldAction4.png");
	_BB_PanelSettingSptire->SetProportion(0.001f, 0.001f);
	_BB_PanelSettingSptire->SetPivot(0.5f, 0.5f);
	_BB_PanelSettingSptire->OnDisable();

	_BB_PanelSetting->AddComponent<Billboard>(_BB_PanelSettingSptire);
	_BB_PanelSetting->SetParent(obj);

	/// 9.리프트 작동 (빌보드)
	GameObject* _BB_LiftMove = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_LiftMove->SetObjectName("BB_LiftMove");
	_BB_LiftMove->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_LiftMoveSptire = new Billboard;
	_BB_LiftMoveSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_LiftMoveSptire->SetRotationType(eRotationType::LookAt);
	_BB_LiftMoveSptire->m_SpriteName_V.push_back("UI_FieldAction5.png");
	_BB_LiftMoveSptire->SetProportion(0.001f, 0.001f);
	_BB_LiftMoveSptire->SetPivot(0.5f, 0.5f);
	_BB_LiftMoveSptire->OnDisable();

	_BB_LiftMove->AddComponent<Billboard>(_BB_LiftMoveSptire);
	_BB_LiftMove->SetParent(obj);

	/// 10.읽기 (빌보드)
	GameObject* _BB_Read = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_Read->SetObjectName("BB_Read");
	_BB_Read->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_ReadSptire = new Billboard;
	_BB_ReadSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_ReadSptire->SetRotationType(eRotationType::LookAt);
	_BB_ReadSptire->m_SpriteName_V.push_back("UI_FieldAction6.png");
	_BB_ReadSptire->SetProportion(0.001f, 0.001f);
	_BB_ReadSptire->SetPivot(0.5f, 0.5f);
	_BB_ReadSptire->OnDisable();

	_BB_Read->AddComponent<Billboard>(_BB_ReadSptire);
	_BB_Read->SetParent(obj);

	/// 11.문열기 (빌보드)
	GameObject* _BB_OpenDoor = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_BB_OpenDoor->SetObjectName("BB_OpenDoor");
	_BB_OpenDoor->m_Transform->SetPosition({ 0.f, 0.f, 0.f });

	Billboard* _BB_OpenDoorSptire = new Billboard;
	_BB_OpenDoorSptire->SeteResourceType(eResourceType::eSingleImage);
	_BB_OpenDoorSptire->SetRotationType(eRotationType::LookAt);
	_BB_OpenDoorSptire->m_SpriteName_V.push_back("UI_FieldAction7.png");
	_BB_OpenDoorSptire->SetProportion(0.001f, 0.001f);
	_BB_OpenDoorSptire->SetPivot(0.5f, 0.5f);
	_BB_OpenDoorSptire->OnDisable();

	_BB_OpenDoor->AddComponent<Billboard>(_BB_OpenDoorSptire);
	_BB_OpenDoor->SetParent(obj);

	////////////////////////////////////////////////////////////////////////

	_questText->m_Transform->SetPosition({ 160.f, 80.f, 0.0f });

	_combatStat_Weapon->m_Transform->SetPosition({ _battleStatePosX, _battleStatePosY, 0.0f });
	_combatStat_Fire->m_Transform->SetPosition({ _battleStatePosX - 180.f, _battleStatePosY - 20.f, 0.0f });
	_combatStat_Heal->m_Transform->SetPosition({ _battleStatePosX - 260.f, _battleStatePosY - 20.f, 0.0f });
	_combatStat_Assist->m_Transform->SetPosition({ _battleStatePosX - 340.f, _battleStatePosY - 20.f, 0.0f });

	_Life->m_Transform->SetPosition({ _battleStatePosX, _battleStatePosY - 100.f, 0.0f });

	///////////////////////////////////////////////////////////////////////////

	obj->AddComponent<IngameUIManager>(new IngameUIManager());

	//////////////////////////////////////////////////////////////////////////

	return 	obj;
}

GameObject* ObjectBuilder::MakePauseUI(GameObject* obj, bool* isSceneRun)
{

	/// UI 루트 컨테이너(Canvas 같은거)
	obj = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	obj->SetObjectName("PauseUI");
	obj->m_Transform->SetPosition({ 0.0f, 0.0f, 0.0f });

	//UI sound 추가
	Audio* _pAudio = DLLEngine::FindContinualGameObjectByName("GlobalAudioPlayer")->GetComponent<Audio>();
	///////////////////////////////////////////////////////////////////////////

	/// 반투명 배경
	GameObject* _backGround01 = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_backGround01->SetObjectName("BackGround01");

	Sprite2D* _backGround01Sptire = new Sprite2D;
	_backGround01Sptire->SetType(eResourceType::eSingleImage);
	_backGround01Sptire->m_SpriteName_V.push_back("PauseUIBackground01.png");
	_backGround01Sptire->SetProportion(1.f, 1.f);
	_backGround01Sptire->SetPivot(0.5f, 0.5f);
	_backGround01Sptire->SetUIAxis(eUIAxis::BackGround);

	_backGround01->AddComponent<Sprite2D>(_backGround01Sptire);
	_backGround01->SetParent(obj);

	float _x = 30.f;
	float _y = 750.f;

	/// Continue 버튼(기능 PauseUI에서 설정)
	GameObject* _Continue = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Continue->m_Transform->SetPosition({ _x, _y, 1.0f });
	_Continue->SetObjectName("PauseContinueButton");

	Button* _ContinueButton = new Button();

	_ContinueButton->m_OffSprite = "UI_Pause_Back_Off.png";
	_ContinueButton->m_OnSprite = "UI_Pause_Back_On.png";

	_ContinueButton->SetProportion(1.0f, 1.0f);
	_ContinueButton->SetPivot(0.f, 1.f);
	_ContinueButton->SetAxis(eUIAxis::RightDown);
	_ContinueButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_ContinueButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	_Continue->AddComponent<Button>(_ContinueButton);

	/// Chapter 버튼
	GameObject* _Chapter = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Chapter->m_Transform->SetPosition({ _x, _y - 70, 1.0f });
	_Chapter->SetObjectName("PauseChapterButton");

	Button* _ChapterButton = new Button();

	_ChapterButton->m_OffSprite = "UI_Pause_Chapter_Off.png";
	_ChapterButton->m_OnSprite = "UI_Pause_Chapter_On.png";

	_ChapterButton->SetProportion(1.0f, 1.0f);
	_ChapterButton->SetPivot(0.f, 1.f);
	_ChapterButton->SetAxis(eUIAxis::RightDown);
	_ChapterButton->SetFunc([]() { CA_TRACE("PauseUI 챕터선택 버튼"); });
	_ChapterButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_ChapterButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	_Chapter->AddComponent<Button>(_ChapterButton);

	/// Title 버튼
	GameObject* _Title = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Title->m_Transform->SetPosition({ _x, _y - 140, 1.0f });
	_Title->SetObjectName("PauseTitleButton");

	Button* _TitleButton = new Button();

	_TitleButton->m_OffSprite = "UI_Pause_Title_Off.png";
	_TitleButton->m_OnSprite = "UI_Pause_Title_On.png";

	_TitleButton->SetProportion(1.0f, 1.0f);
	_TitleButton->SetPivot(0.f, 1.f);
	_TitleButton->SetAxis(eUIAxis::RightDown);
	_TitleButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_TitleButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	_Title->AddComponent<Button>(_TitleButton);

	/// 조작법 버튼
	GameObject* _Manual = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Manual->m_Transform->SetPosition({ _x, _y - 210, 1.0f });
	_Manual->SetObjectName("PauseManualButton");

	Button* _ManualButton = new Button();

	_ManualButton->m_OffSprite = "UI_Button_HtControl_Off.png";
	_ManualButton->m_OnSprite = "UI_Button_HtControl_On.png";

	_ManualButton->SetProportion(1.0f, 1.0f);
	_ManualButton->SetPivot(0.f, 1.f);
	_ManualButton->SetAxis(eUIAxis::RightDown);
	_ManualButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_ManualButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	_Manual->AddComponent<Button>(_ManualButton);

	/// Exit 버튼
	GameObject* _Exit = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Exit->m_Transform->SetPosition({ _x, _y - 280, 1.0f });
	_Exit->SetObjectName("PauseExitButton");

	Button* _ExitButton = new Button();

	_ExitButton->m_OffSprite = "UI_Pause_Exit_Off.png";
	_ExitButton->m_OnSprite = "UI_Pause_Exit_On.png";

	_ExitButton->SetProportion(1.0f, 1.0f);
	_ExitButton->SetPivot(0.f, 1.f);
	_ExitButton->SetAxis(eUIAxis::RightDown);

	_ExitButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_ExitButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_Exit->AddComponent<Button>(_ExitButton);

	/// /////////////////////////////////////////////////////////////////////////
	/// POPUP창
	/// /////////////////////////////////////////////////////////////////////////

	/// PopUp창
	GameObject* _popUp = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_popUp->SetObjectName("PopUp");

	Sprite2D* _popUpSptire = new Sprite2D;
	_popUpSptire->SetType(eResourceType::eSingleImage);

	_popUpSptire->m_SpriteName_V.push_back("UI_Popup_Main.png");
	_popUpSptire->SetProportion(1.f, 1.f);
	_popUpSptire->SetPivot(0.5f, 0.5f);
	_popUpSptire->SetUIAxis(eUIAxis::Center);

	_popUp->AddComponent<Sprite2D>(_popUpSptire);
	_popUp->SetParent(obj);

	/// PopUp 질문들
	GameObject* _popUpQuestions = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_popUpQuestions->SetObjectName("PopUpQuestions");

	Sprite2D* _popUpQuestionsSptire = new Sprite2D;
	_popUpQuestionsSptire->SetType(eResourceType::eSingleImage);
	_popUpQuestionsSptire->m_SpriteName_V.push_back("UI_Popup_Chapter_Tutorial(context).png");
	_popUpQuestionsSptire->m_SpriteName_V.push_back("UI_Popup_Chapter_Generater(context).png");
	_popUpQuestionsSptire->m_SpriteName_V.push_back("UI_Popup_Chapter_TItle(context).png");
	_popUpQuestionsSptire->m_SpriteName_V.push_back("UI_Popup_Exit(context).png");
	_popUpQuestionsSptire->SetProportion(0.5f, 0.5f);
	_popUpQuestionsSptire->SetPivot(0.5f, 0.5f);
	_popUpQuestionsSptire->SetUIAxis(eUIAxis::Center);

	_popUpQuestions->AddComponent<Sprite2D>(_popUpQuestionsSptire);
	_popUpQuestions->SetParent(obj);

	/// 조작법 이미지
	GameObject* _manualPopUp = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_manualPopUp->SetObjectName("PopUpManual");

	Sprite2D* _manualPopUpSptire = new Sprite2D;
	_manualPopUpSptire->SetType(eResourceType::eSingleImage);
	_manualPopUpSptire->m_SpriteName_V.push_back("Keysetting.png");
	_manualPopUpSptire->SetProportion(0.5f, 0.5f);
	_manualPopUpSptire->SetPivot(0.5f, 0.5f);
	_manualPopUpSptire->SetUIAxis(eUIAxis::Center);

	_manualPopUp->AddComponent<Sprite2D>(_manualPopUpSptire);
	_manualPopUp->SetParent(obj);


	float _popUpx = -150.f;
	float _popUpy = 50.f;

	/// Chapter 1 버튼(기능 완)
	GameObject* _Chapter1 = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Chapter1->m_Transform->SetPosition({ _popUpx, _popUpy - 50.f, 1.0f });
	_Chapter1->SetObjectName("Chapter1");

	Button* _Chapter1Button = new Button();

	_Chapter1Button->m_OffSprite = "UI_Chapter_Tutorial_Off.png";
	_Chapter1Button->m_OnSprite = "UI_Chapter_Tutorial_On.png";

	_Chapter1Button->SetProportion(1.0f, 1.0f);
	_Chapter1Button->SetPivot(1.f, 1.f);
	_Chapter1Button->SetAxis(eUIAxis::Center);
	_Chapter1Button->SetFunc([]() { DLLEngine::PickScene("Tutorial_SH"); });
	_Chapter1Button->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_Chapter1Button->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });

	_Chapter1->AddComponent<Button>(_Chapter1Button);

	/// Chapter 2 버튼(기능 완)
	GameObject* _Chapter2 = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Chapter2->m_Transform->SetPosition({ _popUpx + 220, _popUpy - 50.f, 1.0f });
	_Chapter2->SetObjectName("Chapter2");

	Button* _Chapter2Button = new Button();

	_Chapter2Button->m_OffSprite = "UI_Chapter_Generator_Off.png";
	_Chapter2Button->m_OnSprite = "UI_Chapter_Generator_On.png";

	_Chapter2Button->SetProportion(1.0f, 1.0f);
	_Chapter2Button->SetPivot(1.f, 1.f);
	_Chapter2Button->SetAxis(eUIAxis::Center);
	_Chapter2Button->SetFunc([]() { DLLEngine::PickScene("GenRoom"); });
	_Chapter2Button->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_Chapter2Button->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_Chapter2->AddComponent<Button>(_Chapter2Button);


	/// Yes버튼(완)
	GameObject* _Yes = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_Yes->m_Transform->SetPosition({ _popUpx, _popUpy, 1.0f });
	_Yes->SetObjectName("PauseYes");

	Button* _YesButton = new Button();

	_YesButton->m_OffSprite = "UI_Button_Yes_Off.png";
	_YesButton->m_OnSprite = "UI_Button_Yes_On.png";

	_YesButton->SetProportion(1.0f, 1.0f);
	_YesButton->SetPivot(1.f, 1.f);
	_YesButton->SetAxis(eUIAxis::Center);
	_YesButton->SetFunc([]() { DLLEngine::PickScene("GenRoom"); });
	_YesButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_YesButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	_Yes->AddComponent<Button>(_YesButton);

	/// No버튼(완)
	GameObject* _No = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	_No->m_Transform->SetPosition({ _popUpx + 220, _popUpy, 1.0f });
	_No->SetObjectName("PauseNo");

	Button* _NoButton = new Button();

	_NoButton->m_OffSprite = "UI_Button_No_Off.png";
	_NoButton->m_OnSprite = "UI_Button_No_On.png";

	_NoButton->SetProportion(1.0f, 1.0f);
	_NoButton->SetPivot(1.f, 1.f);
	_NoButton->SetAxis(eUIAxis::Center);
	_NoButton->SetFunc([]() { DLLEngine::PickScene("GenRoom"); });
	_NoButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_NoButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	_No->AddComponent<Button>(_NoButton);

	obj->AddComponent<PauseUIManager>(new PauseUIManager(isSceneRun));


	return 	obj;

}