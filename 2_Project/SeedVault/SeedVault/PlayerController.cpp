#include "pch.h"
#include "GameManager.h"
#include "Health.h"
#include "Inventory.h"
#include "Gun.h"
#include "MuzzleFlash.h"
#include "EquipmentController.h"
#include "Zombie_Runner_Move.h"
#include "Zombie_Clicker_Move.h"
#include "CInteraction.h"
#include "CardKeyPad.h"
#include "Generator.h"
#include "Lift.h"
#include "Note.h"
#include "Door.h"
#include "PlayerController.h"

PlayerController::PlayerController()
	: ComponentBase(ComponentType::GameLogic)
	, m_Health(nullptr)
	, m_Inventory(nullptr)
	, m_MyPhysicsActor(nullptr)

	, m_PlayerMesh(nullptr)
	, m_Animator(nullptr)
	, m_Meshfilter(nullptr)

	, m_EquipmentController(nullptr)

	, m_HForAnim(0.0f)
	, m_H(0.0f)
	, m_VForAnim(0.0f)
	, m_V(0.0f)

	, m_bCrouch(false)
	, m_bSprint(false)
	, m_bAim(false)

	, m_bIsReloading(false)
	, m_bIsThrowing(false)
	, m_bIsHealing(false)
	, m_bIsAssassinate(false)
	, m_bCanAssassinate(false)
	, m_bIsRoutingItem(false)
	, m_bCanRoutingItem(false)
	, m_bCanInteraction(false)

	, m_bIsSwaping(false)

	, m_PitchValue(0.0f)

	, m_bIsDie(false)
	, m_bIsDead(false)
	, m_bIsBited(false)

	, m_PoseMode(PoseMode::Stand)
	, m_MovingMode(MovingMode::Idle)

	, m_StandSpeed(0.0f)
	, m_CrouchSpeed(0.0f)
	, m_SprintSpeed(0.0f)
	, m_MoveSpeed(m_StandSpeed)
	, m_SwapSpeed(0.0f)
	, m_SwapCoolTimer(0.0f)
	, m_CameraParent(nullptr)

	, m_SwapInput(-1)
	, m_NextSlotNum(-1)

	, m_FInput(false)
	, m_AssassinateTarget(nullptr)
	, m_RoutingItem(nullptr)
	, m_bIsHeal(nullptr)

	, m_pAudio(nullptr)

	, m_bShoot(false)
	, m_InteractionObj(nullptr)

	// save/load 용 변수
	, m_SaveData(new PlayerController_Save())

{

}

PlayerController::~PlayerController()
{
	Release();
}

void PlayerController::Start()
{
	/// <summary>
	/// 기본 상태 변수들 초기화
	/// </summary>
	{
		// 이동 애니메이션 관련 설정
		m_HForAnim = 0.0f;
		m_H = 0.0f;
		m_VForAnim = 0.0f;
		m_V = 0.0f;

		m_bAim = false;
		m_bShoot = false;

		m_bIsReloading = false;
		m_bIsThrowing = false;
		m_bIsHealing = false;
		m_bIsAssassinate = false;
		m_bCanAssassinate = false;
		m_bIsRoutingItem = false;
		m_bCanInteraction = false;
		m_bIsSwaping = false;

		m_PitchValue = 0.0f;

		m_bIsDie = false;
		m_bIsDead = false;

		m_bCrouch = false;
		m_bSprint = false;
		m_PoseMode = PoseMode::Stand;
		m_MovingMode = MovingMode::Idle;

		m_StandSpeed = 7.0f;
		m_CrouchSpeed = m_StandSpeed / 2.0f;
		m_SprintSpeed = m_StandSpeed * 2.0f;
		m_MoveSpeed = m_StandSpeed;
		m_SwapSpeed = 1.5f;
		m_SwapCoolTimer = 0.0f;

		// 전투관련 값 설정
		m_bAim = false;


		// 스타트될 때(재시작 포함) 살아 움직이게 Die 관련은 false로 초기화
		m_bIsDie = false;
		m_bIsDead = false;

		// 장비 교체 입력 초기화
		m_SwapInput = -1;
		m_NextSlotNum = -1;

		// F 입력 초기화
		m_FInput = false;
	}

	// 커서 화면 중앙으로 이동
	DLLWindow::MoveCursorToCenter();

	/// myObject 컴포넌트 연결
	{
		// Health 연결
		m_Health = m_pMyObject->GetComponent<Health>();
		assert(m_Health != nullptr);
		m_Health->SetMaxHp(6.0f);
		m_Health->SetHp(m_Health->GetMaxHp());

		// Inventory 연결
		m_Inventory = m_pMyObject->GetComponent<Inventory>();
		assert(m_Inventory != nullptr);

		// Animator/MeshFilter 연결
		if (DLLEngine::FindGameObjectByName("Player_Mesh") != nullptr)
		{
			m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh");
			m_Animator = m_PlayerMesh->GetComponent<Animator>();
			m_Meshfilter = m_PlayerMesh->GetComponent<MeshFilter>();
		}
		assert(m_Animator != nullptr);
		assert(m_Meshfilter != nullptr);
	}

	// CameraParent 연결
	GameObject* cameraParent = DLLEngine::FindGameObjectByName("CameraParent");
	assert(cameraParent != nullptr);

	m_MyPhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();

	m_CameraParent = cameraParent->m_Transform;

	// RightHand (EquipmentController) 연결
	GameObject* rightHand = DLLEngine::FindGameObjectByName("RightHand");
	m_EquipmentController = rightHand->GetComponent<EquipmentController>();
	assert(m_EquipmentController != nullptr);

	m_pAudio = m_pMyObject->GetComponent<Audio>();
	assert(m_pAudio != nullptr);

}

void PlayerController::Update(float dTime)
{
	// 임시 Hp 감소 코드
	if (DLLInput::InputKeyDown(static_cast<uint>(CL::KeyCode::KEY_Z)))
	{
		//m_Health->DecreaseHp(1.0f);
	}

	// 죽어있으면 바로 리턴
	if (IsDead())
	{
		return;
	}

	// 클리커 좀비에게 물어 뜯기는 중이면 리턴
	if (m_bIsBited == true)
	{
		return;
	}

	/// 생존여부 업데이트
	UpdateAlive();

	// GameMager 의 s_bTPSMode 가 true 조작한다 
	if (GameManager::IsTPSMode() == true)
	{
		/// 유저 키 입력 반영
		UpdateUserInput();
	}

	// 암살 가능 여부 업데이트
	m_bCanAssassinate = CanAssassinate();

	// 아이템 루팅 가능 여부 업데이트
	m_bCanRoutingItem = CanItemRouting();

	// 상호작용 가능 여부 업데이트
	m_bCanInteraction = CanInteraction();

	/// 장비 교체 쿨타이머
	if (m_bIsSwaping == true)
	{
		SwapCoolDown();
	}

	/// 장비 교체
	else if (m_SwapInput != -1)
	{
		Swap();

		m_SwapInput = -1;
	}

	/// 장전
	if (DLLInput::InputKeyDown(CL::KeyMap::RELOAD))
	{
		// 현재 Gun을 장비중인지 확인
		std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_Inventory->GetCurrentEquipment());
		if (_gun != nullptr)
		{
			if (_gun->IsMagazineFull() == false)
			{
				Reload(_gun);
			}
		}
	}

	/// 장비중인 아이템 사용
	{
		UseEquipedItem();
	}

	/// 특수 기술(암살, 루팅, 상호작용) 사용
	if (m_FInput == true)
	{
		FFunction();
		m_FInput = false;
	}


	/// 이동
	// 암살중이 아닐 때에만 이동
	if (m_bIsAssassinate == false)
	{
		// WASD 전후좌우 이동
		Move();
	}

	/// 디버그 정보 렌더
	if (GameManager::IsDebugMode())
	{
		DebugRender();
	}

}

void PlayerController::OnRender()
{
}



void PlayerController::UpdateUserInput()
{
	/// 이동관련 Axis 입력 업데이트
	m_H = CL::Input::GetAxisRaw("Horizontal");
	m_V = CL::Input::GetAxisRaw("Vertical");

	/// 애니메이션 전환용 Axis 업데이트
	/// 역방향 전환을 위해..(앞 <-> 뒤 , 좌 <-> 우)
	m_HForAnim = CL::Input::GetAxis("Horizontal");
	m_VForAnim = CL::Input::GetAxis("Vertical");

	// 앉기 키 누름
	if (DLLInput::InputKeyDown(CL::KeyMap::CROUCH))
	{
		// 서기 <-> 앉기 자세 변경
		m_bCrouch = !m_bCrouch;

		if (m_bCrouch == true)
		{
			SetPoseMode(PoseMode::Crouch);
		}

		else
		{
			SetPoseMode(PoseMode::Stand);
		}
	}

	/// 달리기
	// 달리기 키랑 전진 키를 누르고 있으면
	if (DLLInput::InputKey(CL::KeyMap::SPRINT) && DLLInput::InputKey(CL::KeyMap::FORWARD))
	{
		m_bSprint = true;
		m_bCrouch = false;	// 달릴 때는 무조건 서있게 만든다

		// 이동속도 변경
		if (m_pMyObject->GetComponent<PhysicsActor>())
		{
			m_MoveSpeed = m_SprintSpeed;
		}

		else
		{
			m_MoveSpeed = m_SprintSpeed; // 피직스 액터가 없이 트랜스폼 이동할 때 기본 달리기속도 적용
		}
	}

	// 달리기 버튼이나 전진 키를 떼면
	else if (DLLInput::InputKeyUp(CL::KeyMap::SPRINT) || DLLInput::InputKeyUp(CL::KeyMap::FORWARD))
	{
		// 달리기 상태 해제
		m_bSprint = false;
		m_MoveSpeed = m_StandSpeed;
	}

	/// 조준 
	if (CanAim() == true
		&& DoNothing() == true
		&& DLLInput::InputKey(CL::KeyMap::AIM) == true)
	{
		// 조준 상태 설정
		m_bAim = true;

		// 달리기 항상 off
			m_bSprint = false;

		//m_Animator->SetOverrideAnimLayer("");

		// 사격중이 아닐 때만 조준 애니메이션 설정
		// 사격중이면 카메라 줌인은 되지만 애니메이션은 사격 애니메이션으로
		if (m_bShoot == false)
		{
			// 이동 상태, 애니메이션 적용
			if (m_PoseMode == PoseMode::Stand)
			{
				m_MoveSpeed = m_StandSpeed;

				// 활성화 하고 싶은 레이어 이름만 가지고 함수 호출
				switch (m_Inventory->m_EquiptedSlotIndex)
				{
					case 0:
					{
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_AIM);
						break;
					}

					case 1:
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_AIM);
						break;

					case 2:
						break;

				}

				if (m_Inventory->m_EquipedItem->GetType() == IItem::Type::eLongGun)
				{
					if (m_H == 0.0f)
					{
						// Idle이나 앞뒤로 움직일 때 살짝 오른쪽으로 회전
						m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 240.0f, 0.0f });
					}

					else
					{
						if (m_V == 0.0f)
						{
							// 좌우로 움직일 때는 기본 방향 그대로
							m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
						}
					}
				}

				if (m_Inventory->m_EquipedItem->GetType() == IItem::Type::eHandGun)
				{
					if (m_H == 0.0f && m_V == 0.0f)
					{
						m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 220.0f, 0.0f });
					}

					else if (m_H != 0.0f)
					{
						m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 220.0f, 0.0f });
					}

					else
					{
						if (m_V != 0.0f)
						{
							m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 240.0f, 0.0f });
						}
					}
				}

			}

			else if (m_PoseMode == PoseMode::Crouch)
			{
				m_MoveSpeed = m_CrouchSpeed;

				m_Animator->SetOverrideAnimLayer("");
				m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
			}
		}


	}

	/// 비조준
	else
	{
		m_bAim = false;

		if (DoNothing() == true)
		{
			if (m_bShoot == false)
			{
				m_Animator->SetOverrideAnimLayer("");
				m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 190.0f, 0.0f });
			}
			//m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
		}
	}

	/// 장비 교체 입력
	if (DoNothing() == true && m_bSprint == false)
	{
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_1)))
		{
			m_SwapInput = static_cast<int>(Inventory::Slot::eLongGun);
		}

		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_2)))
		{
			m_SwapInput = static_cast<int>(Inventory::Slot::eHandGun);
		}

		//if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyMap::SLOT_3)))
		//{
		//	//m_SwapInput = static_cast<int>(Inventory::Slot::eFlask);
		//}
	}

	/// 회복 키 입력
	if (DoNothing() == true)
	{
		if (DLLInput::InputKeyDown(CL::KeyMap::SLOT_4))
		{
			// 힐 포션을 가지고 있고, Hp가 닳아 있을 때
			if (m_Inventory->GetItemCount(IItem::Type::eHealPotion) > 0
				&& m_Health->GetHp() < m_Health->GetMaxHp())
			{
				m_bIsHeal = true;

				// 애니메이션 FSM, 레이어 세팅
				IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
				switch (_type)
				{
					case IItem::Type::eLongGun:
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_HEAL);
						break;

					case IItem::Type::eHandGun:
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_HEAL);
						break;

					case IItem::Type::eMax:
						break;
					default:
						break;
				}

				// 즉시 힐 슬롯으로 변경(추후 애니메이션 종료시에 메인 무기로 스왑)
				m_Inventory->ChangeWeapon(static_cast<uint>(Inventory::Slot::eHealPotion));

				CA_TRACE("[PlayerController] HealPotion 장비");
			}
		}

	}

	/// F키 입력
	if (DoNothing() == true)
	{
		if (DLLInput::InputKeyDown(CL::KeyMap::INTERATION))
		{
			m_FInput = true;
		}
	}
}

void PlayerController::UpdateAlive()
{
	if (m_Health->IsAlive())
	{
		m_bIsDie = false;
	}

	else
	{
		m_bIsDie = true;
	}
}

void PlayerController::Move()
{
	// PhysicsActor 컴포넌트가 있으면 물리로 움직인다.
	if (GetMyObject()->GetComponent<PhysicsActor>())
	{
		MoveByPhysX();
	}

	// PhysicsActor 이 없는 상태면 기본 트랜스폼으로 이동한다.
	else
	{
		MoveByTransform();
	}
}

/// <summary>
/// physX 물리가 아닌 Transform을 적용해 이동하는 함수
/// 대각선으로 이동할 때 이동속도 보정이 필요하다.(아직 미적용)
/// </summary>
void PlayerController::MoveByTransform()
{
	/// <summary>
	///  전후좌우 이동
	/// </summary>

	if (m_V > 0.0f)
	{
		// 전방 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(m_MoveSpeed * CL::Input::s_DeltaTime);

	}

	if (m_V < 0.0f)
	{
		// 후방 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (m_H < 0.0f)
	{
		// 좌로 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (m_H > 0.0f)
	{
		// 우로 이동
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(m_MoveSpeed * CL::Input::s_DeltaTime);
	}
}

/// <summary>
/// physX 물리를 이용해서 움직이는 함수
/// Object에 PhysicsActor 컴포넌트가 있을 때 사용
/// </summary>
void PlayerController::MoveByPhysX()
{
	/// 키네마틱 설정 되어있으면 게임엔진 트랜스폼으로 이동하고 피직스씬에 트랜스폼도 동기화
	if (GetMyObject()->GetComponent<PhysicsActor>()->IsKinematic())
	{
		MoveByTransform();
		DLLEngine::SetPhysicsTransform(m_Transform);
		//DLLEngine::SetTransform(GetMyObject(), *m_Transform);

		return;
	}

	/// 키네막틱 false 일때 물리이동
	// 카메라 방향 
	//Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_RotationTM.Forward();
	Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_WorldTM.Forward();
	Vector3 _cameraRightDir = m_CameraParent->m_Transform->m_WorldTM.Right();

	// 계산할 이동 방향
	Vector3 _moveDir = Vector3::Zero;

	/// <summary>
	///  전후좌우 이동
	/// </summary>
	if (m_V > 0.0f)
	{
		// 전방 이동
		_moveDir += { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};
	}

	if (m_V < 0.0f)
	{
		// 후방 이동
		_moveDir -= { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};
	}

	if (m_H < 0.0f)
	{
		// 좌로 이동
		_moveDir -= { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	if (m_H > 0.0f)
	{
		// 우로 이동
		_moveDir += { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	// 밀림 방지
	if (m_V == 0.0f && m_H == 0.0f)
	{
		m_MyPhysicsActor->SetFreezePosition(true, true, true);
	}
	else
	{
		m_MyPhysicsActor->SetFreezePosition(false, true, false);
	}

	// 이동방향 정규화
	_moveDir.Normalize();

	// 이동량 계산w
	_moveDir *= m_MoveSpeed * CL::Input::s_DeltaTime;

	// 이동
	//DLLEngine::SetVelocity(m_MyPhysicsActor, _moveDir);
	m_MyPhysicsActor->SetVelocity(_moveDir);

	//CA_TRACE("pos = %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);

	// Look 방향 전환
	m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
}


PoseMode PlayerController::SetPoseMode(PoseMode pose)
{
	// PoseMode 변경
	m_PoseMode = pose;

	switch (m_PoseMode)
	{
		case PoseMode::Stand:
		{
			m_MoveSpeed = m_StandSpeed;		// Stand 이동 속도 

			//CA_TRACE("[PlayerController] Now Mode : Idle / Now MoveSpeed = %f", m_MoveSpeed);
			break;
		}


		case PoseMode::Crouch:
		{
			m_MoveSpeed = m_CrouchSpeed;		// Crouch 이동 속도

			//CA_TRACE("[PlayerController] Now Mode : Crouch / Now MoveSpeed = %f", m_MoveSpeed);
			break;
		}

		case PoseMode::Max:
			break;
		default:
			break;
	}

	return m_PoseMode;
}

bool PlayerController::UseEquipedItem() const
{
	// 비조준 시 사용(공격, 투척)키 눌렀을 때
	if (DLLInput::InputKeyDown(CL::KeyMap::ATTACK))
	{
		// 현재 Gun을 장비중인지 확인
		std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_Inventory->GetCurrentEquipment());
		if (_gun != nullptr)
		{
			// 탄창이 비어있으면 장전
			if (_gun->IsMagazineEmpty() == true)
			{
				Reload(_gun);
			}
		}

		_gun = nullptr;

		if (DLLInput::InputKey(CL::KeyMap::AIM) && DoNothing() == true)
		{
			m_Inventory->m_EquipedItem->Use();

			return true;
		}

	}

	return false;
}

bool PlayerController::PostHeal()
{
	// 강제(즉시) 0번(주무기)로 변경
	m_Inventory->ChangeWeapon(static_cast<uint>(Inventory::Slot::eLongGun));

	// 무기 메쉬 변경
	ChangeWeaponMesh();

	// 스왑 종료후 스왑한 메쉬, 애님 레이어로 세팅
	IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
	switch (_type)
	{
		case IItem::Type::eLongGun:
		{
			m_Meshfilter->SetMesh(CL::ResourcePath::MESH_PLAYER_RIFLE);
			m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
			break;
		}

		case IItem::Type::eHandGun:
		{
			m_Meshfilter->SetMesh(CL::ResourcePath::MESH_PLAYER_PISTOL);
			m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
			break;
		}

		case IItem::Type::eFlask:
		{
			break;
		}
	}

	return true;
}

void PlayerController::SwapCoolDown()
{
	m_SwapCoolTimer -= CL::Input::s_DeltaTime;

	if (m_SwapCoolTimer <= 0.0f)
	{
		m_bIsSwaping = false;
		CA_TRACE("[PlayerController] 장비교체 타이머 끝");

		// 무기교체 애니메이션 해제
		m_Animator->SetOverrideAnimLayer("");

		// 스왑 종료후 스왑한 메쉬, 애님 레이어로 세팅
		IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
		switch (_type)
		{
			case IItem::Type::eLongGun:
			{
				m_Meshfilter->SetMesh(CL::ResourcePath::MESH_PLAYER_RIFLE);
				m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
				break;
			}

			case IItem::Type::eHandGun:
			{
				m_Meshfilter->SetMesh(CL::ResourcePath::MESH_PLAYER_PISTOL);
				m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
				break;
			}

			case IItem::Type::eFlask:
			{
				break;
			}
		}
	}
}

/// <summary>
/// 현재 총 장비중이고 탄창여분 공간이 있으면 장전한다.
/// </summary>
void PlayerController::Reload(std::shared_ptr<Gun> gun) const
{
	if (gun == nullptr)
	{
		gun = std::dynamic_pointer_cast<Gun>(m_Inventory->GetCurrentEquipment());
	}

	if (DoNothing() == false)
	{
		return;
	}

	if (gun != nullptr)
	{
		IItem::Type _gunType = gun->GetType();
		// 인벤토리에 탄약이 떨어졌을 때
		if (_gunType == IItem::Type::eLongGun)
		{
			if (m_Inventory->GetItemCount(IItem::Type::eLongGunAmmo) <= 0)
			{
				return;
			}
		}

		else if (_gunType == IItem::Type::eHandGun)
		{
			if (m_Inventory->GetItemCount(IItem::Type::eHandGunAmmo) <= 0)
			{
				return;
			}
		}

		if (m_bIsReloading == false)
		{
			gun->StartReload();

			// 현재 장비중인 슬롯 번호
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;

			// 자세 확인
			switch (m_PoseMode)
			{
				case PoseMode::Stand:
				{
					if (_currentSlotNum == static_cast<uint>(Inventory::Slot::eLongGun))
					{
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_STAND_RELOAD);
					}

					else if (_currentSlotNum == static_cast<uint>(Inventory::Slot::eHandGun))
					{
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_STAND_RELOAD);
					}

					break;
				}


				case PoseMode::Crouch:
				{
					if (_currentSlotNum == static_cast<uint>(Inventory::Slot::eLongGun))
					{
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_CROUCH_RELOAD);
					}

					else if (_currentSlotNum == static_cast<uint>(Inventory::Slot::eHandGun))
					{
						m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_CROUCH_RELOAD);
					}
					break;
				}
			}// end of switch
		}
	}
}

/// <summary>
/// 플레이어 Hp 회복
/// </summary>
void PlayerController::Heal()
{
	// 회복 약 들고 있는 상태면
	if (m_Inventory->GetCurrentEquipment()->GetType() == IItem::Type::eHealPotion)
	{
		// 사용
		m_Inventory->m_EquipedItem->Use();
	}

}

bool PlayerController::FFunction()
{
	// 암살
	if (m_bCanAssassinate == true)
	{
		Assassinate();
		return true;
	}

	// 아이템 루팅
	if (m_bCanRoutingItem == true)
	{
		RoutingItem();
		return true;
	}

	// 상호작용
	if (m_bCanInteraction == true)
	{
		Interaction();
		return true;
	}

	return false;
}

/// <summary>
/// 암살 액션 가능한지 판단
/// </summary>
/// <returns>암살 실행 가능 여부</returns>
bool PlayerController::CanAssassinate()
{
	// 암살 사정 거리
	const float _assassinateRange = 1.5f;

	// 암살 범위 각
	const float _assassinateAngle = 90.0f;
	float _leftAssassinateAngle = m_Transform->m_EulerAngles.y + (_assassinateAngle / 2.0f);
	float _rightAssassinateAngle = m_Transform->m_EulerAngles.y - (_assassinateAngle / 2.0f);

	UtilityFunction::NormalizeAngle(_leftAssassinateAngle);
	UtilityFunction::NormalizeAngle(_rightAssassinateAngle);

	/// 최종적으로 선택할 타겟
	m_AssassinateTarget = nullptr;

	// 좀비의 뒤에 있으면서 어그로가 끌리지 않았고
	// 좀비를 바라보고...
	/// 암살 가능한 좀비들을 찾는다
	auto& zombies = DLLEngine::FindGameObjectByTag("Zombie");

	for (auto& zombie : zombies)
	{
		Enemy_Move* _enemyMove = nullptr;
		_enemyMove = zombie->GetComponent<Zombie_Runner_Move>();

		if (_enemyMove == nullptr)
		{
			_enemyMove = zombie->GetComponent<Zombie_Clicker_Move>();
		}
		assert(_enemyMove != nullptr);

		/// 좀비 암살 가능 여부 초기화 
		_enemyMove->m_bCanBeAssassinated = false;

		/// 생존여부 체크
		if (_enemyMove->IsDead() == true)
		{
			continue;
		}

		/// 어그로 인식 체크
		if (_enemyMove->m_Target.object == nullptr)
		{
			/// Step 1. 플레이어가 좀비의 뒤 암살영역 안에 있는지 검사
			bool _onAssassinatePos = false;

			/// 거리 체크
			if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *zombie, _assassinateRange) == false)
			{
				// 거리가 안되면 패스
				continue;
			}

			// 거리가 되면 플레이어의 위치 확인
			const float _assassinatePosAngle = 160.0f;
			float _backAngleY = _enemyMove->m_Transform->m_EulerAngles.y - 180.0f;
			float _leftAngle = _backAngleY + _assassinatePosAngle / 2.0f;
			float _rightAngle = _backAngleY - _assassinatePosAngle / 2.0f;

			UtilityFunction::NormalizeAngle(_backAngleY);
			UtilityFunction::NormalizeAngle(_leftAngle);
			UtilityFunction::NormalizeAngle(_rightAngle);

			float _angleZombieToPlayer = UtilityFunction::CalcAngleToTarget(*zombie, *m_pMyObject);
			UtilityFunction::NormalizeAngle(_angleZombieToPlayer);

			bool _TargetInPosition = false;
			// 암살 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
			if (_leftAngle > _rightAngle)
			{
				if (_angleZombieToPlayer <= _leftAngle &&
					_angleZombieToPlayer >= _rightAngle)
				{
					_TargetInPosition = true;
				}

			}
			// 암살 사이에 0(360)도가 끼어서 범위가 분리되는 경우
			else
			{
				if ((0.0f <= _angleZombieToPlayer) && (_angleZombieToPlayer <= _leftAngle) ||		// 좌쪽 범위 체크
					((_rightAngle <= _angleZombieToPlayer) && (_angleZombieToPlayer <= 360.0f)))	// 우측 범위 체크

				{
					_TargetInPosition = true;
				}
			}

			// 암살 유효 포지션 밖에 있는 경우
			if (_TargetInPosition == false)
			{
				continue;
			}
			//

			/// 암살 가능한 유효 위치에 있는 경우
			/// 플레이어의 방향 => 암살 범위 각 체크(각도는 플레이어를 기준으로)
			float _angleToTarget = UtilityFunction::CalcAngleToTarget(*m_pMyObject, *zombie);
			UtilityFunction::NormalizeAngle(_angleToTarget);

			bool _TargetInSight = false;
			// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
			if (_leftAssassinateAngle > _rightAssassinateAngle)
			{
				if (_angleToTarget <= _leftAssassinateAngle &&
					_angleToTarget >= _rightAssassinateAngle)
				{
					_TargetInSight = true;
				}

			}
			// 시야각 사이에 0(360)도가 끼어서 범위가 분리되는 경우
			else
			{
				if ((0.0f <= _angleToTarget) && (_angleToTarget <= _leftAssassinateAngle) ||		// 좌쪽 범위 체크
					((_rightAssassinateAngle <= _angleToTarget) && (_angleToTarget <= 360.0f)))	// 우측 범위 체크

				{
					_TargetInSight = true;
				}
			}

			// 암살 범위(각) 밖에 있는 경우
			if (_TargetInSight == false)
			{
				continue;
			}

			/// 가장 가까운 암살 가능한 좀비 선택
			if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *zombie, _assassinateRange) == true)
			{
				// 암살 가능 조건을 만족하는 좀비 발견
				// 기존 타겟과 비교해서 더 가까운 좀비를 최종 타겟으로 선정
				if (m_AssassinateTarget == nullptr)
				{
					m_AssassinateTarget = _enemyMove;

					continue;
				}

				else
				{
					float _distToCurrentTarget = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_AssassinateTarget->m_Transform->m_Position);
					float _distToNextZombie = SimpleMath::Vector3::Distance(m_Transform->m_Position, _enemyMove->m_Transform->m_Position);

					if (_distToCurrentTarget > _distToNextZombie)
					{
						m_AssassinateTarget = _enemyMove;
					}
				}
			}

		}
	}

	// 암살 가능 타겟이 없는경우
	if (m_AssassinateTarget == nullptr)
	{
		//CA_TRACE("[PlayerController] CanAssassinate 가능한 좀비 없음..");
		return false;
	}

	// 암살 가능 타겟이 있는 경우
	// (타겟은 암살당하는 FSM으로 변경해야함)
	m_AssassinateTarget->m_bCanBeAssassinated = true;	// 이 변수를 기준으로 UI 띄워주면 될 것이다.

	//CA_TRACE("[PlayerController] CanAssassinate 가능!!");
	return true;
}

bool PlayerController::Assassinate()
{
	CA_TRACE("[PlayerController] Assassinate()");

	// 암살상태 설정
	m_bIsAssassinate = true;

	// 플레이어 무적상태로 변경
	m_Health->SetInvincible(true);

	// 플레이어 위치 보정
	SimpleMath::Vector3 _assassinatePos = m_AssassinateTarget->m_Transform->m_WorldPosition;
	_assassinatePos -= m_AssassinateTarget->m_Transform->m_RotationTM.Forward() * 0.7f;
	m_Transform->m_Position = _assassinatePos;

	// 좀비를 바라보도록
	m_Transform->LookAtYaw(m_AssassinateTarget->m_Transform->m_WorldPosition);

	// 밀림 방지
	m_MyPhysicsActor->SetFreezePosition(true, true, true);
	m_MyPhysicsActor->SetFreezeRotation(true, true, true);

	// 타겟 좀비 피암살 상태 설정
	m_AssassinateTarget->m_State |= Enemy_Move::State::eAssassinated;

	// 애니메이션 FSM, 레이어 세팅
	IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
	switch (_type)
	{
		case IItem::Type::eLongGun:
			m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ASSASSINATE_BEGIN);
			break;

		case IItem::Type::eHandGun:
			m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ASSASSINATE_BEGIN);
			break;

		case IItem::Type::eFlask:
			break;
		case IItem::Type::eFireFlask:
			break;
		case IItem::Type::eHealPotion:
			break;

		case IItem::Type::eMax:
			break;
		default:
			break;
	}


	return true;
}

/// <summary>
/// 루팅(획득)가능한 아이템이 있는지 검사
/// </summary>
/// <returns>검사 결과</returns>
bool PlayerController::CanItemRouting()
{
	// 루팅 범위 각
	const float _routingAngle = 180.0f;
	float _leftRoutingAngle = m_Transform->m_EulerAngles.y + (_routingAngle / 2.0f);
	float _rightRoutingAngle = m_Transform->m_EulerAngles.y - (_routingAngle / 2.0f);

	UtilityFunction::NormalizeAngle(_leftRoutingAngle);
	UtilityFunction::NormalizeAngle(_rightRoutingAngle);

	// 루팅 사정 거리
	const float _routingRange = 1.0f;

	/// 루팅 가능한 아이템
	m_RoutingItem = nullptr;

	/// 아이템 오브젝트들을 얻는다
	auto& _deployedItems = DLLEngine::FindGameObjectByTag("DeployedItem");

	for (auto& item : _deployedItems)
	{
		/// 루팅 범위 각 체크(각도는 카메라를 기준으로 해야 불편하지 않다)
		float _angleToTarget = UtilityFunction::CalcAngleToTarget(*m_CameraParent->GetMyObject(), *item);
		UtilityFunction::NormalizeAngle(_angleToTarget);

		bool _TargetInSight = false;
		// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
		if (_leftRoutingAngle > _rightRoutingAngle)
		{
			if (_angleToTarget <= _leftRoutingAngle &&
				_angleToTarget >= _rightRoutingAngle)
			{
				_TargetInSight = true;
			}

		}
		// 시야각 사이에 0(360)도가 끼어서 범위가 분리되는 경우
		else
		{
			if ((0.0f <= _angleToTarget) && (_angleToTarget <= _leftRoutingAngle) ||		// 좌쪽 범위 체크
				((_rightRoutingAngle <= _angleToTarget) && (_angleToTarget <= 360.0f)))		// 우측 범위 체크

			{
				_TargetInSight = true;
			}
		}

		// 루팅 범위(각) 밖에 있는 경우
		if (_TargetInSight == false)
		{
			continue;
		}

		/// 거리 체크
		if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *item, _routingRange) == true)
		{
			// 획득 가능 조건을 만족하는 아이템 발견
			// 기존 타겟과 비교해서 더 가까운 아이템 최종 타겟으로 선정
			if (m_RoutingItem == nullptr)
			{
				m_RoutingItem = item;

				continue;
			}

			else
			{
				float _distToCurrentItem = SimpleMath::Vector3::Distance(m_Transform->m_Position, m_RoutingItem->m_Transform->m_Position);
				float _distToNextItem = SimpleMath::Vector3::Distance(m_Transform->m_Position, item->m_Transform->m_Position);

				if (_distToCurrentItem > _distToNextItem)
				{
					m_RoutingItem = item;
				}
			}
		}
	}

	// 획득 가능 아이템이 없는경우
	if (m_RoutingItem == nullptr)
	{
		//CA_TRACE("[PlayerController] CanRouting 루팅 가능한 아이템 없음..");
		return false;
	}

	// 획득 가능 타겟이 있는 경우
	//CA_TRACE("[PlayerController] CanRouting 가능!!");
	return true;
}

bool PlayerController::RoutingItem()
{
	// 아이템 루팅 상태로 변경
	m_bIsRoutingItem = true;

	// 아이템 종류를 이름으로 파악 후 
	// 인벤토리에 해당 아이템 갯수 증가
	std::string _routingItemName = m_RoutingItem->GetObjectName();
	if (strcmp(_routingItemName.c_str(), "LongGunAmmo") == 0)
	{
		m_Inventory->IncreaseItemCount(IItem::Type::eLongGunAmmo, 4);
		CA_TRACE("[PlayerController] RoutingItem LongGunAmmo! now Count -> %d", m_Inventory->GetItemCount(IItem::Type::eLongGunAmmo));
	}

	else if (strcmp(_routingItemName.c_str(), "HandGunAmmo") == 0)
	{
		m_Inventory->IncreaseItemCount(IItem::Type::eHandGunAmmo, 6);
		CA_TRACE("[PlayerController] RoutingItem HandGunAmmo! now Count -> %d", m_Inventory->GetItemCount(IItem::Type::eHandGunAmmo));
	}

	else if (strcmp(_routingItemName.c_str(), "HealPotion") == 0)
	{
		m_Inventory->IncreaseItemCount(IItem::Type::eHealPotion, 1);
		CA_TRACE("[PlayerController] RoutingItem HealPotion! now Count -> %d", m_Inventory->GetItemCount(IItem::Type::eHealPotion));
	}

	else if (strcmp(_routingItemName.c_str(), "CardKey") == 0)
	{
		m_Inventory->IncreaseItemCount(IItem::Type::eCardKey, 1);
		CA_TRACE("[PlayerController] RoutingItem FirstKey! now Count -> %d", m_Inventory->GetItemCount(IItem::Type::eCardKey));
	}

	else if (strcmp(_routingItemName.c_str(), "File") == 0)
	{
		m_Inventory->IncreaseItemCount(IItem::Type::eFile, 1);
		CA_TRACE("[PlayerController] RoutingItem File! now Count -> %d", m_Inventory->GetItemCount(IItem::Type::eFile));
	}

	// 획득한 배치 아이템은 제거
	DLLGameObject::DeleteGameObject(DLLEngine::GetNowScene(), m_RoutingItem);

	// 애니메이션 FSM, 레이어 세팅
	IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
	switch (_type)
	{
		case IItem::Type::eLongGun:
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_ROUTING_ITEM);
			break;

		case IItem::Type::eHandGun:
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_ROUTING_ITEM);
			break;

		case IItem::Type::eFlask:
			break;
		case IItem::Type::eFireFlask:
			break;
		case IItem::Type::eHealPotion:
			break;
	}

	return true;
}

/// <summary>
/// 상호작용 가능한 대상이 있는지 검사
/// </summary>
/// <returns></returns>
bool PlayerController::CanInteraction()
{
	// 상호작용 사정 거리
	const float _interactionRange = 1.2f;

	// 상호작용 범위 각
	const float _ainteractionAngle = 60.0f;
	float _leftinteractionAngle = m_Transform->m_EulerAngles.y + (_ainteractionAngle / 2.0f);
	float _rightinteractionAngle = m_Transform->m_EulerAngles.y - (_ainteractionAngle / 2.0f);

	UtilityFunction::NormalizeAngle(_leftinteractionAngle);
	UtilityFunction::NormalizeAngle(_rightinteractionAngle);

	/// 최종적으로 선택할 타겟
	m_InteractionObj = nullptr;

	// 상호작용 오브젝트의 앞에 있으면서 오브젝트를 바라보고...
	/// 상호작용 가능한 오브젝트들을 찾는다
	auto& _interactionObjs = DLLEngine::FindGameObjectByTag("Interaction");

	for (auto& interactionObj : _interactionObjs)
	{
		CInteraction* _interaction = nullptr;
		_interaction = interactionObj->GetComponent<CardKeyPad>();
		
		if (_interaction == nullptr)
		{
			_interaction = interactionObj->GetComponent<Generator>();
		}

		if (_interaction == nullptr)
		{
			_interaction = interactionObj->GetComponent<Lift>();
		}

		if (_interaction == nullptr)
		{
			_interaction = interactionObj->GetComponent<Note>();
		}

		if (_interaction == nullptr)
		{
			_interaction = interactionObj->GetComponent<Door>();
		}
		
		if (_interaction == nullptr)
		{
			continue;
		}


		/// Step 1. 플레이어가 상호작용 오브젝트의 앞에 있는지 검사
		bool _onInteractionPos = false;

		/// 거리 체크
		if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *interactionObj, _interactionRange) == false)
		{
			// 거리가 안되면 패스
			continue;
		}

		// 거리가 되면 플레이어의 위치 확인
		const float _InteractionPosAngle = 160.0f;
		float _angleY = interactionObj->m_Transform->m_EulerAngles.y;
		float _leftAngle = _angleY + _InteractionPosAngle / 2.0f;
		float _rightAngle = _angleY - _InteractionPosAngle / 2.0f;

		UtilityFunction::NormalizeAngle(_angleY);
		UtilityFunction::NormalizeAngle(_leftAngle);
		UtilityFunction::NormalizeAngle(_rightAngle);

		float _angleInteractionObjToPlayer = UtilityFunction::CalcAngleToTarget(*interactionObj, *m_pMyObject);
		UtilityFunction::NormalizeAngle(_angleInteractionObjToPlayer);

		bool _TargetInPosition = false;
		// 상호작용 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
		if (_leftAngle > _rightAngle)
		{
			if (_angleInteractionObjToPlayer <= _leftAngle &&
				_angleInteractionObjToPlayer >= _rightAngle)
			{
				_TargetInPosition = true;
			}

		}
		// 상호작용 사이에 0(360)도가 끼어서 범위가 분리되는 경우
		else
		{
			if ((0.0f <= _angleInteractionObjToPlayer) && (_angleInteractionObjToPlayer <= _leftAngle) ||		// 좌쪽 범위 체크
				((_rightAngle <= _angleInteractionObjToPlayer) && (_angleInteractionObjToPlayer <= 360.0f)))			// 우측 범위 체크

			{
				_TargetInPosition = true;
			}
		}

		// 상호작용 유효 포지션 밖에 있는 경우
		if (_TargetInPosition == false)
		{
			continue;
		}
		//

		/// 상호작용 가능한 유효 위치에 있는 경우
		/// 플레이어의 방향 => 상호작용 범위 각 체크(각도는 플레이어를 기준으로)
		float _angleToTarget = UtilityFunction::CalcAngleToTarget(*m_pMyObject, *interactionObj);
		UtilityFunction::NormalizeAngle(_angleToTarget);

		bool _TargetInSight = false;
		// 시야각 사이가 연결 되는 경우(좌측과 우측 사이에 0(360)이 안 걸리는 경우)
		if (_leftinteractionAngle > _rightinteractionAngle)
		{
			if (_angleToTarget <= _leftinteractionAngle &&
				_angleToTarget >= _rightinteractionAngle)
			{
				_TargetInSight = true;
			}

		}
		// 시야각 사이에 0(360)도가 끼어서 범위가 분리되는 경우
		else
		{
			if ((0.0f <= _angleToTarget) && (_angleToTarget <= _leftinteractionAngle) ||		// 좌쪽 범위 체크
				((_rightinteractionAngle <= _angleToTarget) && (_angleToTarget <= 360.0f)))		// 우측 범위 체크

			{
				_TargetInSight = true;
			}
		}

		// 암살 범위(각) 밖에 있는 경우
		if (_TargetInSight == false)
		{
			continue;
		}

		else
		{
			m_InteractionObj = _interaction;
		}


	}

	// 상호작용 타겟이 없는경우
	if (m_InteractionObj == nullptr)
	{
		//CA_TRACE("[PlayerController] CanInteraction 가능한 오브젝트 없음..");
		return false;
	}

	//CA_TRACE("[PlayerController] CanInteraction 가능!!");
	return true;
}

bool PlayerController::Interaction()
{
	//CInteraction* _interactionObj = nullptr;
	//_interactionObj = m_InteractionObj->GetComponent<CardKeyPad>();
	
	if (m_InteractionObj == nullptr)
	{
		return false;
	}

	else
	{
		// 작동
		m_InteractionObj->Run();
	}

	return true;
}

float PlayerController::GetStandSpeed() const
{
	return m_StandSpeed;
}

float PlayerController::GetCrouchSpeed() const
{
	return m_CrouchSpeed;
}

float PlayerController::GetSprintSpeed() const
{
	return m_SprintSpeed;
}

Animator* PlayerController::GetAnimator() const
{
	return m_Animator;
}

Enemy_Move* PlayerController::GetAssassinateTarget() const
{
	return m_AssassinateTarget;
}

void PlayerController::PostDie()
{
	m_bIsDie = false;	// 쓰러지고 있는 상태 해제
	m_bIsDead = true;	// 죽어서 누워있는상태 true
}

void PlayerController::PostAssassinate()
{
	// 애니메이션, 상태 세팅
	IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
	switch (_type)
	{
		case IItem::Type::eLongGun:
			m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE);
			break;

		case IItem::Type::eHandGun:
			m_Animator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL);
			break;

		case IItem::Type::eFlask:
			break;
		case IItem::Type::eFireFlask:
			break;
		case IItem::Type::eAlcohol:
			break;
		case IItem::Type::eHerb:
			break;
		case IItem::Type::eHealPotion:
			break;
		case IItem::Type::eLongGunAmmo:
			break;
		case IItem::Type::eHandGunAmmo:
			break;
		case IItem::Type::eMax:
			break;
		default:
			break;
	}

	// 무적상태 해제
	m_Health->SetInvincible(false);

	// 암살상태 해제
	m_bIsAssassinate = false;

	// 고정 해제
	m_MyPhysicsActor->SetFreezePosition(false, true, false);
	m_MyPhysicsActor->SetFreezeRotation(false, false, true);
}

/// <summary>
/// 플레이어 사망 판단
/// </summary>
/// <returns></returns>
bool PlayerController::IsDead()
{
	// 쓰러지고 있거나, 이미 죽어서 누워있는 경우
	if (m_bIsDie == true || m_bIsDead == true)
	{
		return true;
	}

	// Alive
	return false;
}

/// <summary>
/// 현재 조준 가능한가 판단
/// </summary>
/// <returns>판단 여부</returns>
bool PlayerController::CanAim() const
{
	if (m_bIsReloading == false
		&& m_bIsThrowing == false
		&& m_bIsSwaping == false
		&& m_bIsHealing == false
		&& m_bIsAssassinate == false
		&& m_bIsRoutingItem == false
		&& m_bIsHeal == false)
	{
		return true;
	}

	else
	{
		return false;
	}
}

/// <summary>
/// 현재 무기 스왑이 가능한지 판단
/// </summary>
/// <returns>스왑 가능 여부</returns>
bool PlayerController::CanSwap() const
{
	if (m_bIsSwaping == false && m_bSprint == false)
	{
		//CA_TRACE("[PlayerController] Now, Can Swap ~");
		return true;
	}

	else
	{
		//CA_TRACE("[PlayerController] Can't Swap, yet...");
		return false;
	}
}

/// <summary>
/// 장비교체 함수
/// 플레이어가 슬롯 번호를 입력하면
/// 해당 슬롯의 장비로 교체한다
/// </summary>
/// <returns></returns>
bool PlayerController::Swap()
{
	// 교체할 장비 슬롯 번호 변경
	m_NextSlotNum = m_SwapInput;

	// 현재 장비중인 슬롯인지 확인
	uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
	if (_currentSlotNum == m_NextSlotNum)
	{
		CA_TRACE("[PlayerController] 장비교체 실패 - > 현재 장비한 아이템이다..");

		return false;
	}

	// 장비 교체 중인 상태로 변경
	m_bIsSwaping = true;
	m_SwapCoolTimer = m_SwapSpeed;

	// 애니메이션 변경
	switch (_currentSlotNum)
	{
		case 0:
		{
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON, true);
			break;
		}

		case 1:
		{
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_SWAP_WEAPON, true);
			break;
		}

		case 2:
		{
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON, true);
			break;
		}

		case 3:
		{
			m_Animator->SetOverrideAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_SWAP_WEAPON, true);
			break;
		}

		default:
			break;
	}

	CA_TRACE("[PlayerController] 장비교체 시작");

	return true;
}

bool PlayerController::DoNothing() const
{
	if (m_bIsReloading == false
		&& m_bIsSwaping == false
		&& m_bIsAssassinate == false
		&& m_bIsRoutingItem == false
		&& m_bShoot == false
		&& m_bIsHeal == false)
	{
		return true;
	}

	//else
	{
		return false;
	}
}

/// <summary>
/// 장비한 무기에 따라서 메쉬 변경
/// </summary>
void PlayerController::ChangeWeaponMesh()
{
	IItem::Type _type = m_Inventory->GetCurrentEquipment()->GetType();
	switch (_type)
	{
		case IItem::Type::eLongGun:
		{
			m_EquipmentController->ChangeWeaponMesh(IItem::Type::eLongGun);
			break;
		}

		case IItem::Type::eHandGun:
		{
			m_EquipmentController->ChangeWeaponMesh(IItem::Type::eHandGun);
			break;
		}

		case IItem::Type::eFlask:
		{
			break;
		}
	}
}

void PlayerController::DebugRender()
{
	std::string _hitObject = "hitObj = ";

	/*
	if (m_LastHitTarget)
	{
		_hitObject += m_LastHitTarget->GetObjectName();
		_hitObject += "\n HP = ";

		if (m_LastHitTarget->GetComponent<Health>() != nullptr)
		{
			float hp = m_LastHitTarget->GetComponent<Health>()->GetHp();
			_hitObject += std::to_string(hp);
		}

	}
	// 변환용 변수
	TCHAR* wstr = StringHelper::StringToWchar(_hitObject);

	Text* text = m_pMyObject->GetComponent<Text>();
	text->PrintSpriteText((TCHAR*)L"%s", wstr);

	SAFE_DELETE(wstr);
	*/


	// 테스트 레이 렌더 (버그)
	//DLLEngine::DrawRay(m_Transform->m_Position, m_Transform->m_RotationTM.Forward(), 100.0f);
}

void PlayerController::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_HForAnim = m_HForAnim;
	m_SaveData->m_VForAnim = m_VForAnim;
	m_SaveData->m_H = m_H;
	m_SaveData->m_V = m_V;

	m_SaveData->m_bCrouch = m_bCrouch;
	m_SaveData->m_bSprint = m_bSprint;
	m_SaveData->m_bAim = m_bAim;

	//m_SaveData->m_Animator = m_Animator->GetComponetId();

	m_SaveData->m_PoseMode = static_cast<int>(m_PoseMode);
	m_SaveData->m_MovingMode = static_cast<int>(m_MovingMode);

	m_SaveData->m_StandSpeed = m_StandSpeed;
	m_SaveData->m_CrouchSpeed = m_CrouchSpeed;
	m_SaveData->m_SprintSpeed = m_SprintSpeed;
	m_SaveData->m_MoveSpeed = m_MoveSpeed;

	m_SaveData->m_CameraParent = m_CameraParent->GetComponetId();

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void PlayerController::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_HForAnim = m_SaveData->m_HForAnim;
	m_VForAnim = m_SaveData->m_VForAnim;
	m_H = m_SaveData->m_H;
	m_V = m_SaveData->m_V;
	m_bCrouch = m_SaveData->m_bCrouch;
	m_bSprint = m_SaveData->m_bSprint;
	m_bAim = m_SaveData->m_bAim;

	//m_Animator = m_SaveData->m_Animator;	// 컴포넌트 불러오는건 Start 에서 연결해준다
	m_PoseMode = static_cast<PoseMode>(m_SaveData->m_PoseMode);
	m_MovingMode = static_cast<MovingMode>(m_SaveData->m_MovingMode);

	m_StandSpeed = m_SaveData->m_StandSpeed;
	m_CrouchSpeed = m_SaveData->m_CrouchSpeed;
	m_SprintSpeed = m_SaveData->m_SprintSpeed;
	m_MoveSpeed = m_SaveData->m_MoveSpeed;
}

void PlayerController::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{

}
