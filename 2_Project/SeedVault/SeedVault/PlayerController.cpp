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

	// save/load �� ����
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
	/// �⺻ ���� ������ �ʱ�ȭ
	/// </summary>
	{
		// �̵� �ִϸ��̼� ���� ����
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

		// �������� �� ����
		m_bAim = false;


		// ��ŸƮ�� ��(����� ����) ��� �����̰� Die ������ false�� �ʱ�ȭ
		m_bIsDie = false;
		m_bIsDead = false;

		// ��� ��ü �Է� �ʱ�ȭ
		m_SwapInput = -1;
		m_NextSlotNum = -1;

		// F �Է� �ʱ�ȭ
		m_FInput = false;
	}

	// Ŀ�� ȭ�� �߾����� �̵�
	DLLWindow::MoveCursorToCenter();

	/// myObject ������Ʈ ����
	{
		// Health ����
		m_Health = m_pMyObject->GetComponent<Health>();
		assert(m_Health != nullptr);
		m_Health->SetMaxHp(6.0f);
		m_Health->SetHp(m_Health->GetMaxHp());

		// Inventory ����
		m_Inventory = m_pMyObject->GetComponent<Inventory>();
		assert(m_Inventory != nullptr);

		// Animator/MeshFilter ����
		if (DLLEngine::FindGameObjectByName("Player_Mesh") != nullptr)
		{
			m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh");
			m_Animator = m_PlayerMesh->GetComponent<Animator>();
			m_Meshfilter = m_PlayerMesh->GetComponent<MeshFilter>();
		}
		assert(m_Animator != nullptr);
		assert(m_Meshfilter != nullptr);
	}

	// CameraParent ����
	GameObject* cameraParent = DLLEngine::FindGameObjectByName("CameraParent");
	assert(cameraParent != nullptr);

	m_MyPhysicsActor = m_pMyObject->GetComponent<PhysicsActor>();

	m_CameraParent = cameraParent->m_Transform;

	// RightHand (EquipmentController) ����
	GameObject* rightHand = DLLEngine::FindGameObjectByName("RightHand");
	m_EquipmentController = rightHand->GetComponent<EquipmentController>();
	assert(m_EquipmentController != nullptr);

	m_pAudio = m_pMyObject->GetComponent<Audio>();
	assert(m_pAudio != nullptr);

}

void PlayerController::Update(float dTime)
{
	// �ӽ� Hp ���� �ڵ�
	if (DLLInput::InputKeyDown(static_cast<uint>(CL::KeyCode::KEY_Z)))
	{
		//m_Health->DecreaseHp(1.0f);
	}

	// �׾������� �ٷ� ����
	if (IsDead())
	{
		return;
	}

	// Ŭ��Ŀ ���񿡰� ���� ���� ���̸� ����
	if (m_bIsBited == true)
	{
		return;
	}

	/// �������� ������Ʈ
	UpdateAlive();

	// GameMager �� s_bTPSMode �� true �����Ѵ� 
	if (GameManager::IsTPSMode() == true)
	{
		/// ���� Ű �Է� �ݿ�
		UpdateUserInput();
	}

	// �ϻ� ���� ���� ������Ʈ
	m_bCanAssassinate = CanAssassinate();

	// ������ ���� ���� ���� ������Ʈ
	m_bCanRoutingItem = CanItemRouting();

	// ��ȣ�ۿ� ���� ���� ������Ʈ
	m_bCanInteraction = CanInteraction();

	/// ��� ��ü ��Ÿ�̸�
	if (m_bIsSwaping == true)
	{
		SwapCoolDown();
	}

	/// ��� ��ü
	else if (m_SwapInput != -1)
	{
		Swap();

		m_SwapInput = -1;
	}

	/// ����
	if (DLLInput::InputKeyDown(CL::KeyMap::RELOAD))
	{
		// ���� Gun�� ��������� Ȯ��
		std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_Inventory->GetCurrentEquipment());
		if (_gun != nullptr)
		{
			if (_gun->IsMagazineFull() == false)
			{
				Reload(_gun);
			}
		}
	}

	/// ������� ������ ���
	{
		UseEquipedItem();
	}

	/// Ư�� ���(�ϻ�, ����, ��ȣ�ۿ�) ���
	if (m_FInput == true)
	{
		FFunction();
		m_FInput = false;
	}


	/// �̵�
	// �ϻ����� �ƴ� ������ �̵�
	if (m_bIsAssassinate == false)
	{
		// WASD �����¿� �̵�
		Move();
	}

	/// ����� ���� ����
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
	/// �̵����� Axis �Է� ������Ʈ
	m_H = CL::Input::GetAxisRaw("Horizontal");
	m_V = CL::Input::GetAxisRaw("Vertical");

	/// �ִϸ��̼� ��ȯ�� Axis ������Ʈ
	/// ������ ��ȯ�� ����..(�� <-> �� , �� <-> ��)
	m_HForAnim = CL::Input::GetAxis("Horizontal");
	m_VForAnim = CL::Input::GetAxis("Vertical");

	// �ɱ� Ű ����
	if (DLLInput::InputKeyDown(CL::KeyMap::CROUCH))
	{
		// ���� <-> �ɱ� �ڼ� ����
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

	/// �޸���
	// �޸��� Ű�� ���� Ű�� ������ ������
	if (DLLInput::InputKey(CL::KeyMap::SPRINT) && DLLInput::InputKey(CL::KeyMap::FORWARD))
	{
		m_bSprint = true;
		m_bCrouch = false;	// �޸� ���� ������ ���ְ� �����

		// �̵��ӵ� ����
		if (m_pMyObject->GetComponent<PhysicsActor>())
		{
			m_MoveSpeed = m_SprintSpeed;
		}

		else
		{
			m_MoveSpeed = m_SprintSpeed; // ������ ���Ͱ� ���� Ʈ������ �̵��� �� �⺻ �޸���ӵ� ����
		}
	}

	// �޸��� ��ư�̳� ���� Ű�� ����
	else if (DLLInput::InputKeyUp(CL::KeyMap::SPRINT) || DLLInput::InputKeyUp(CL::KeyMap::FORWARD))
	{
		// �޸��� ���� ����
		m_bSprint = false;
		m_MoveSpeed = m_StandSpeed;
	}

	/// ���� 
	if (CanAim() == true
		&& DoNothing() == true
		&& DLLInput::InputKey(CL::KeyMap::AIM) == true)
	{
		// ���� ���� ����
		m_bAim = true;

		// �޸��� �׻� off
			m_bSprint = false;

		//m_Animator->SetOverrideAnimLayer("");

		// ������� �ƴ� ���� ���� �ִϸ��̼� ����
		// ������̸� ī�޶� ������ ������ �ִϸ��̼��� ��� �ִϸ��̼�����
		if (m_bShoot == false)
		{
			// �̵� ����, �ִϸ��̼� ����
			if (m_PoseMode == PoseMode::Stand)
			{
				m_MoveSpeed = m_StandSpeed;

				// Ȱ��ȭ �ϰ� ���� ���̾� �̸��� ������ �Լ� ȣ��
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
						// Idle�̳� �յڷ� ������ �� ��¦ ���������� ȸ��
						m_PlayerMesh->m_Transform->SetRotationFromVec({ 0.0f, 240.0f, 0.0f });
					}

					else
					{
						if (m_V == 0.0f)
						{
							// �¿�� ������ ���� �⺻ ���� �״��
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

	/// ������
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

	/// ��� ��ü �Է�
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

	/// ȸ�� Ű �Է�
	if (DoNothing() == true)
	{
		if (DLLInput::InputKeyDown(CL::KeyMap::SLOT_4))
		{
			// �� ������ ������ �ְ�, Hp�� ��� ���� ��
			if (m_Inventory->GetItemCount(IItem::Type::eHealPotion) > 0
				&& m_Health->GetHp() < m_Health->GetMaxHp())
			{
				m_bIsHeal = true;

				// �ִϸ��̼� FSM, ���̾� ����
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

				// ��� �� �������� ����(���� �ִϸ��̼� ����ÿ� ���� ����� ����)
				m_Inventory->ChangeWeapon(static_cast<uint>(Inventory::Slot::eHealPotion));

				CA_TRACE("[PlayerController] HealPotion ���");
			}
		}

	}

	/// FŰ �Է�
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
	// PhysicsActor ������Ʈ�� ������ ������ �����δ�.
	if (GetMyObject()->GetComponent<PhysicsActor>())
	{
		MoveByPhysX();
	}

	// PhysicsActor �� ���� ���¸� �⺻ Ʈ���������� �̵��Ѵ�.
	else
	{
		MoveByTransform();
	}
}

/// <summary>
/// physX ������ �ƴ� Transform�� ������ �̵��ϴ� �Լ�
/// �밢������ �̵��� �� �̵��ӵ� ������ �ʿ��ϴ�.(���� ������)
/// </summary>
void PlayerController::MoveByTransform()
{
	/// <summary>
	///  �����¿� �̵�
	/// </summary>

	if (m_V > 0.0f)
	{
		// ���� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(m_MoveSpeed * CL::Input::s_DeltaTime);

	}

	if (m_V < 0.0f)
	{
		// �Ĺ� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveForwardOnXZ(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (m_H < 0.0f)
	{
		// �·� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(-m_MoveSpeed * CL::Input::s_DeltaTime);
	}

	if (m_H > 0.0f)
	{
		// ��� �̵�
		m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
		m_Transform->MoveSide(m_MoveSpeed * CL::Input::s_DeltaTime);
	}
}

/// <summary>
/// physX ������ �̿��ؼ� �����̴� �Լ�
/// Object�� PhysicsActor ������Ʈ�� ���� �� ���
/// </summary>
void PlayerController::MoveByPhysX()
{
	/// Ű�׸�ƽ ���� �Ǿ������� ���ӿ��� Ʈ���������� �̵��ϰ� ���������� Ʈ�������� ����ȭ
	if (GetMyObject()->GetComponent<PhysicsActor>()->IsKinematic())
	{
		MoveByTransform();
		DLLEngine::SetPhysicsTransform(m_Transform);
		//DLLEngine::SetTransform(GetMyObject(), *m_Transform);

		return;
	}

	/// Ű�׸�ƽ false �϶� �����̵�
	// ī�޶� ���� 
	//Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_RotationTM.Forward();
	Vector3 _cameraForwardDir = m_CameraParent->m_Transform->m_WorldTM.Forward();
	Vector3 _cameraRightDir = m_CameraParent->m_Transform->m_WorldTM.Right();

	// ����� �̵� ����
	Vector3 _moveDir = Vector3::Zero;

	/// <summary>
	///  �����¿� �̵�
	/// </summary>
	if (m_V > 0.0f)
	{
		// ���� �̵�
		_moveDir += { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};
	}

	if (m_V < 0.0f)
	{
		// �Ĺ� �̵�
		_moveDir -= { _cameraForwardDir.x, 0.0f, _cameraForwardDir.z};
	}

	if (m_H < 0.0f)
	{
		// �·� �̵�
		_moveDir -= { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	if (m_H > 0.0f)
	{
		// ��� �̵�
		_moveDir += { _cameraRightDir.x, 0.0f, _cameraRightDir.z};
	}

	// �и� ����
	if (m_V == 0.0f && m_H == 0.0f)
	{
		m_MyPhysicsActor->SetFreezePosition(true, true, true);
	}
	else
	{
		m_MyPhysicsActor->SetFreezePosition(false, true, false);
	}

	// �̵����� ����ȭ
	_moveDir.Normalize();

	// �̵��� ���w
	_moveDir *= m_MoveSpeed * CL::Input::s_DeltaTime;

	// �̵�
	//DLLEngine::SetVelocity(m_MyPhysicsActor, _moveDir);
	m_MyPhysicsActor->SetVelocity(_moveDir);

	//CA_TRACE("pos = %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);

	// Look ���� ��ȯ
	m_Transform->SetRotationFromVec({ 0.0f, m_CameraParent->m_Transform->m_EulerAngles.y, 0.0f });
}


PoseMode PlayerController::SetPoseMode(PoseMode pose)
{
	// PoseMode ����
	m_PoseMode = pose;

	switch (m_PoseMode)
	{
		case PoseMode::Stand:
		{
			m_MoveSpeed = m_StandSpeed;		// Stand �̵� �ӵ� 

			//CA_TRACE("[PlayerController] Now Mode : Idle / Now MoveSpeed = %f", m_MoveSpeed);
			break;
		}


		case PoseMode::Crouch:
		{
			m_MoveSpeed = m_CrouchSpeed;		// Crouch �̵� �ӵ�

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
	// ������ �� ���(����, ��ô)Ű ������ ��
	if (DLLInput::InputKeyDown(CL::KeyMap::ATTACK))
	{
		// ���� Gun�� ��������� Ȯ��
		std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_Inventory->GetCurrentEquipment());
		if (_gun != nullptr)
		{
			// źâ�� ��������� ����
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
	// ����(���) 0��(�ֹ���)�� ����
	m_Inventory->ChangeWeapon(static_cast<uint>(Inventory::Slot::eLongGun));

	// ���� �޽� ����
	ChangeWeaponMesh();

	// ���� ������ ������ �޽�, �ִ� ���̾�� ����
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
		CA_TRACE("[PlayerController] ���ü Ÿ�̸� ��");

		// ���ⱳü �ִϸ��̼� ����
		m_Animator->SetOverrideAnimLayer("");

		// ���� ������ ������ �޽�, �ִ� ���̾�� ����
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
/// ���� �� ������̰� źâ���� ������ ������ �����Ѵ�.
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
		// �κ��丮�� ź���� �������� ��
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

			// ���� ������� ���� ��ȣ
			uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;

			// �ڼ� Ȯ��
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
/// �÷��̾� Hp ȸ��
/// </summary>
void PlayerController::Heal()
{
	// ȸ�� �� ��� �ִ� ���¸�
	if (m_Inventory->GetCurrentEquipment()->GetType() == IItem::Type::eHealPotion)
	{
		// ���
		m_Inventory->m_EquipedItem->Use();
	}

}

bool PlayerController::FFunction()
{
	// �ϻ�
	if (m_bCanAssassinate == true)
	{
		Assassinate();
		return true;
	}

	// ������ ����
	if (m_bCanRoutingItem == true)
	{
		RoutingItem();
		return true;
	}

	// ��ȣ�ۿ�
	if (m_bCanInteraction == true)
	{
		Interaction();
		return true;
	}

	return false;
}

/// <summary>
/// �ϻ� �׼� �������� �Ǵ�
/// </summary>
/// <returns>�ϻ� ���� ���� ����</returns>
bool PlayerController::CanAssassinate()
{
	// �ϻ� ���� �Ÿ�
	const float _assassinateRange = 1.5f;

	// �ϻ� ���� ��
	const float _assassinateAngle = 90.0f;
	float _leftAssassinateAngle = m_Transform->m_EulerAngles.y + (_assassinateAngle / 2.0f);
	float _rightAssassinateAngle = m_Transform->m_EulerAngles.y - (_assassinateAngle / 2.0f);

	UtilityFunction::NormalizeAngle(_leftAssassinateAngle);
	UtilityFunction::NormalizeAngle(_rightAssassinateAngle);

	/// ���������� ������ Ÿ��
	m_AssassinateTarget = nullptr;

	// ������ �ڿ� �����鼭 ��׷ΰ� ������ �ʾҰ�
	// ���� �ٶ󺸰�...
	/// �ϻ� ������ ������� ã�´�
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

		/// ���� �ϻ� ���� ���� �ʱ�ȭ 
		_enemyMove->m_bCanBeAssassinated = false;

		/// �������� üũ
		if (_enemyMove->IsDead() == true)
		{
			continue;
		}

		/// ��׷� �ν� üũ
		if (_enemyMove->m_Target.object == nullptr)
		{
			/// Step 1. �÷��̾ ������ �� �ϻ쿵�� �ȿ� �ִ��� �˻�
			bool _onAssassinatePos = false;

			/// �Ÿ� üũ
			if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *zombie, _assassinateRange) == false)
			{
				// �Ÿ��� �ȵǸ� �н�
				continue;
			}

			// �Ÿ��� �Ǹ� �÷��̾��� ��ġ Ȯ��
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
			// �ϻ� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
			if (_leftAngle > _rightAngle)
			{
				if (_angleZombieToPlayer <= _leftAngle &&
					_angleZombieToPlayer >= _rightAngle)
				{
					_TargetInPosition = true;
				}

			}
			// �ϻ� ���̿� 0(360)���� ��� ������ �и��Ǵ� ���
			else
			{
				if ((0.0f <= _angleZombieToPlayer) && (_angleZombieToPlayer <= _leftAngle) ||		// ���� ���� üũ
					((_rightAngle <= _angleZombieToPlayer) && (_angleZombieToPlayer <= 360.0f)))	// ���� ���� üũ

				{
					_TargetInPosition = true;
				}
			}

			// �ϻ� ��ȿ ������ �ۿ� �ִ� ���
			if (_TargetInPosition == false)
			{
				continue;
			}
			//

			/// �ϻ� ������ ��ȿ ��ġ�� �ִ� ���
			/// �÷��̾��� ���� => �ϻ� ���� �� üũ(������ �÷��̾ ��������)
			float _angleToTarget = UtilityFunction::CalcAngleToTarget(*m_pMyObject, *zombie);
			UtilityFunction::NormalizeAngle(_angleToTarget);

			bool _TargetInSight = false;
			// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
			if (_leftAssassinateAngle > _rightAssassinateAngle)
			{
				if (_angleToTarget <= _leftAssassinateAngle &&
					_angleToTarget >= _rightAssassinateAngle)
				{
					_TargetInSight = true;
				}

			}
			// �þ߰� ���̿� 0(360)���� ��� ������ �и��Ǵ� ���
			else
			{
				if ((0.0f <= _angleToTarget) && (_angleToTarget <= _leftAssassinateAngle) ||		// ���� ���� üũ
					((_rightAssassinateAngle <= _angleToTarget) && (_angleToTarget <= 360.0f)))	// ���� ���� üũ

				{
					_TargetInSight = true;
				}
			}

			// �ϻ� ����(��) �ۿ� �ִ� ���
			if (_TargetInSight == false)
			{
				continue;
			}

			/// ���� ����� �ϻ� ������ ���� ����
			if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *zombie, _assassinateRange) == true)
			{
				// �ϻ� ���� ������ �����ϴ� ���� �߰�
				// ���� Ÿ�ٰ� ���ؼ� �� ����� ���� ���� Ÿ������ ����
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

	// �ϻ� ���� Ÿ���� ���°��
	if (m_AssassinateTarget == nullptr)
	{
		//CA_TRACE("[PlayerController] CanAssassinate ������ ���� ����..");
		return false;
	}

	// �ϻ� ���� Ÿ���� �ִ� ���
	// (Ÿ���� �ϻ���ϴ� FSM���� �����ؾ���)
	m_AssassinateTarget->m_bCanBeAssassinated = true;	// �� ������ �������� UI ����ָ� �� ���̴�.

	//CA_TRACE("[PlayerController] CanAssassinate ����!!");
	return true;
}

bool PlayerController::Assassinate()
{
	CA_TRACE("[PlayerController] Assassinate()");

	// �ϻ���� ����
	m_bIsAssassinate = true;

	// �÷��̾� �������·� ����
	m_Health->SetInvincible(true);

	// �÷��̾� ��ġ ����
	SimpleMath::Vector3 _assassinatePos = m_AssassinateTarget->m_Transform->m_WorldPosition;
	_assassinatePos -= m_AssassinateTarget->m_Transform->m_RotationTM.Forward() * 0.7f;
	m_Transform->m_Position = _assassinatePos;

	// ���� �ٶ󺸵���
	m_Transform->LookAtYaw(m_AssassinateTarget->m_Transform->m_WorldPosition);

	// �и� ����
	m_MyPhysicsActor->SetFreezePosition(true, true, true);
	m_MyPhysicsActor->SetFreezeRotation(true, true, true);

	// Ÿ�� ���� �Ǿϻ� ���� ����
	m_AssassinateTarget->m_State |= Enemy_Move::State::eAssassinated;

	// �ִϸ��̼� FSM, ���̾� ����
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
/// ����(ȹ��)������ �������� �ִ��� �˻�
/// </summary>
/// <returns>�˻� ���</returns>
bool PlayerController::CanItemRouting()
{
	// ���� ���� ��
	const float _routingAngle = 180.0f;
	float _leftRoutingAngle = m_Transform->m_EulerAngles.y + (_routingAngle / 2.0f);
	float _rightRoutingAngle = m_Transform->m_EulerAngles.y - (_routingAngle / 2.0f);

	UtilityFunction::NormalizeAngle(_leftRoutingAngle);
	UtilityFunction::NormalizeAngle(_rightRoutingAngle);

	// ���� ���� �Ÿ�
	const float _routingRange = 1.0f;

	/// ���� ������ ������
	m_RoutingItem = nullptr;

	/// ������ ������Ʈ���� ��´�
	auto& _deployedItems = DLLEngine::FindGameObjectByTag("DeployedItem");

	for (auto& item : _deployedItems)
	{
		/// ���� ���� �� üũ(������ ī�޶� �������� �ؾ� �������� �ʴ�)
		float _angleToTarget = UtilityFunction::CalcAngleToTarget(*m_CameraParent->GetMyObject(), *item);
		UtilityFunction::NormalizeAngle(_angleToTarget);

		bool _TargetInSight = false;
		// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
		if (_leftRoutingAngle > _rightRoutingAngle)
		{
			if (_angleToTarget <= _leftRoutingAngle &&
				_angleToTarget >= _rightRoutingAngle)
			{
				_TargetInSight = true;
			}

		}
		// �þ߰� ���̿� 0(360)���� ��� ������ �и��Ǵ� ���
		else
		{
			if ((0.0f <= _angleToTarget) && (_angleToTarget <= _leftRoutingAngle) ||		// ���� ���� üũ
				((_rightRoutingAngle <= _angleToTarget) && (_angleToTarget <= 360.0f)))		// ���� ���� üũ

			{
				_TargetInSight = true;
			}
		}

		// ���� ����(��) �ۿ� �ִ� ���
		if (_TargetInSight == false)
		{
			continue;
		}

		/// �Ÿ� üũ
		if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *item, _routingRange) == true)
		{
			// ȹ�� ���� ������ �����ϴ� ������ �߰�
			// ���� Ÿ�ٰ� ���ؼ� �� ����� ������ ���� Ÿ������ ����
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

	// ȹ�� ���� �������� ���°��
	if (m_RoutingItem == nullptr)
	{
		//CA_TRACE("[PlayerController] CanRouting ���� ������ ������ ����..");
		return false;
	}

	// ȹ�� ���� Ÿ���� �ִ� ���
	//CA_TRACE("[PlayerController] CanRouting ����!!");
	return true;
}

bool PlayerController::RoutingItem()
{
	// ������ ���� ���·� ����
	m_bIsRoutingItem = true;

	// ������ ������ �̸����� �ľ� �� 
	// �κ��丮�� �ش� ������ ���� ����
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

	// ȹ���� ��ġ �������� ����
	DLLGameObject::DeleteGameObject(DLLEngine::GetNowScene(), m_RoutingItem);

	// �ִϸ��̼� FSM, ���̾� ����
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
/// ��ȣ�ۿ� ������ ����� �ִ��� �˻�
/// </summary>
/// <returns></returns>
bool PlayerController::CanInteraction()
{
	// ��ȣ�ۿ� ���� �Ÿ�
	const float _interactionRange = 1.2f;

	// ��ȣ�ۿ� ���� ��
	const float _ainteractionAngle = 60.0f;
	float _leftinteractionAngle = m_Transform->m_EulerAngles.y + (_ainteractionAngle / 2.0f);
	float _rightinteractionAngle = m_Transform->m_EulerAngles.y - (_ainteractionAngle / 2.0f);

	UtilityFunction::NormalizeAngle(_leftinteractionAngle);
	UtilityFunction::NormalizeAngle(_rightinteractionAngle);

	/// ���������� ������ Ÿ��
	m_InteractionObj = nullptr;

	// ��ȣ�ۿ� ������Ʈ�� �տ� �����鼭 ������Ʈ�� �ٶ󺸰�...
	/// ��ȣ�ۿ� ������ ������Ʈ���� ã�´�
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


		/// Step 1. �÷��̾ ��ȣ�ۿ� ������Ʈ�� �տ� �ִ��� �˻�
		bool _onInteractionPos = false;

		/// �Ÿ� üũ
		if (UtilityFunction::IsTargetInDetectionRange(*m_pMyObject, *interactionObj, _interactionRange) == false)
		{
			// �Ÿ��� �ȵǸ� �н�
			continue;
		}

		// �Ÿ��� �Ǹ� �÷��̾��� ��ġ Ȯ��
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
		// ��ȣ�ۿ� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
		if (_leftAngle > _rightAngle)
		{
			if (_angleInteractionObjToPlayer <= _leftAngle &&
				_angleInteractionObjToPlayer >= _rightAngle)
			{
				_TargetInPosition = true;
			}

		}
		// ��ȣ�ۿ� ���̿� 0(360)���� ��� ������ �и��Ǵ� ���
		else
		{
			if ((0.0f <= _angleInteractionObjToPlayer) && (_angleInteractionObjToPlayer <= _leftAngle) ||		// ���� ���� üũ
				((_rightAngle <= _angleInteractionObjToPlayer) && (_angleInteractionObjToPlayer <= 360.0f)))			// ���� ���� üũ

			{
				_TargetInPosition = true;
			}
		}

		// ��ȣ�ۿ� ��ȿ ������ �ۿ� �ִ� ���
		if (_TargetInPosition == false)
		{
			continue;
		}
		//

		/// ��ȣ�ۿ� ������ ��ȿ ��ġ�� �ִ� ���
		/// �÷��̾��� ���� => ��ȣ�ۿ� ���� �� üũ(������ �÷��̾ ��������)
		float _angleToTarget = UtilityFunction::CalcAngleToTarget(*m_pMyObject, *interactionObj);
		UtilityFunction::NormalizeAngle(_angleToTarget);

		bool _TargetInSight = false;
		// �þ߰� ���̰� ���� �Ǵ� ���(������ ���� ���̿� 0(360)�� �� �ɸ��� ���)
		if (_leftinteractionAngle > _rightinteractionAngle)
		{
			if (_angleToTarget <= _leftinteractionAngle &&
				_angleToTarget >= _rightinteractionAngle)
			{
				_TargetInSight = true;
			}

		}
		// �þ߰� ���̿� 0(360)���� ��� ������ �и��Ǵ� ���
		else
		{
			if ((0.0f <= _angleToTarget) && (_angleToTarget <= _leftinteractionAngle) ||		// ���� ���� üũ
				((_rightinteractionAngle <= _angleToTarget) && (_angleToTarget <= 360.0f)))		// ���� ���� üũ

			{
				_TargetInSight = true;
			}
		}

		// �ϻ� ����(��) �ۿ� �ִ� ���
		if (_TargetInSight == false)
		{
			continue;
		}

		else
		{
			m_InteractionObj = _interaction;
		}


	}

	// ��ȣ�ۿ� Ÿ���� ���°��
	if (m_InteractionObj == nullptr)
	{
		//CA_TRACE("[PlayerController] CanInteraction ������ ������Ʈ ����..");
		return false;
	}

	//CA_TRACE("[PlayerController] CanInteraction ����!!");
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
		// �۵�
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
	m_bIsDie = false;	// �������� �ִ� ���� ����
	m_bIsDead = true;	// �׾ �����ִ»��� true
}

void PlayerController::PostAssassinate()
{
	// �ִϸ��̼�, ���� ����
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

	// �������� ����
	m_Health->SetInvincible(false);

	// �ϻ���� ����
	m_bIsAssassinate = false;

	// ���� ����
	m_MyPhysicsActor->SetFreezePosition(false, true, false);
	m_MyPhysicsActor->SetFreezeRotation(false, false, true);
}

/// <summary>
/// �÷��̾� ��� �Ǵ�
/// </summary>
/// <returns></returns>
bool PlayerController::IsDead()
{
	// �������� �ְų�, �̹� �׾ �����ִ� ���
	if (m_bIsDie == true || m_bIsDead == true)
	{
		return true;
	}

	// Alive
	return false;
}

/// <summary>
/// ���� ���� �����Ѱ� �Ǵ�
/// </summary>
/// <returns>�Ǵ� ����</returns>
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
/// ���� ���� ������ �������� �Ǵ�
/// </summary>
/// <returns>���� ���� ����</returns>
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
/// ���ü �Լ�
/// �÷��̾ ���� ��ȣ�� �Է��ϸ�
/// �ش� ������ ���� ��ü�Ѵ�
/// </summary>
/// <returns></returns>
bool PlayerController::Swap()
{
	// ��ü�� ��� ���� ��ȣ ����
	m_NextSlotNum = m_SwapInput;

	// ���� ������� �������� Ȯ��
	uint _currentSlotNum = m_Inventory->m_EquiptedSlotIndex;
	if (_currentSlotNum == m_NextSlotNum)
	{
		CA_TRACE("[PlayerController] ���ü ���� - > ���� ����� �������̴�..");

		return false;
	}

	// ��� ��ü ���� ���·� ����
	m_bIsSwaping = true;
	m_SwapCoolTimer = m_SwapSpeed;

	// �ִϸ��̼� ����
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

	CA_TRACE("[PlayerController] ���ü ����");

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
/// ����� ���⿡ ���� �޽� ����
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
	// ��ȯ�� ����
	TCHAR* wstr = StringHelper::StringToWchar(_hitObject);

	Text* text = m_pMyObject->GetComponent<Text>();
	text->PrintSpriteText((TCHAR*)L"%s", wstr);

	SAFE_DELETE(wstr);
	*/


	// �׽�Ʈ ���� ���� (����)
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

	//m_Animator = m_SaveData->m_Animator;	// ������Ʈ �ҷ����°� Start ���� �������ش�
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
