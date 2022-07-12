#include "pch.h"
#include "PlayerController.h"
#include "EquipmentController.h"

EquipmentController::EquipmentController()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerMesh(nullptr)
	, m_PlayerController(nullptr)

	, m_WeaponMesh(nullptr)
	, m_WeaponParent(nullptr)

	, m_MuzzleFlash(nullptr)
{
}

EquipmentController::~EquipmentController()
{
}

void EquipmentController::Start()
{
	// �θ� Ʈ������(������) ��ġ ����
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// �÷��̾� ��Ʈ�ѷ� ����
	m_PlayerController = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>();

	// ���� �޽� ������Ʈ ����
	GameObject* equipedMesh = DLLEngine::FindGameObjectByName("EquipedMesh");
	if (equipedMesh != nullptr)
	{
		m_WeaponMesh = equipedMesh->GetComponent<MeshFilter>();
	}
	assert(m_WeaponMesh != nullptr);

	// ���� �÷��� ����
	m_MuzzleFlash = DLLEngine::FindGameObjectByName("MuzzleFlash");
	assert(m_MuzzleFlash != nullptr);
}

void EquipmentController::Update(float dTime)
{
	// �÷��̾ �׾������� �������� ����.
	if (m_PlayerController->m_bIsDead == true)
	{
		return;
	}

	// �θ� Ʈ������(������) ��ġ ����
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// Ÿ�� ������Ʈ�� Ʈ������ ����d�� ���Ѵ�
	m_WeaponParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// Ÿ���� �������� myObject�� Ʈ������ ����
	m_Transform->SetPosition(m_TargetTranslation);
	m_Transform->SetRotation(m_TargetQuaternion);
	m_Transform->SetScale(m_TargetScale);

	//CA_TRACE("pos - %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);
}

void EquipmentController::OnRender()
{
}

/// <summary>
/// ���� ��ü�� ���� �� ������ ���� �޽��� Ʈ�������� ����, �����ϴ� ���
/// </summary>
/// <param name="type"></param>
void EquipmentController::ChangeWeaponMesh(IItem::Type type)
{
	// �ٲ� �÷��̾� �޽��� �������� �θ� �������� �ٽ� ����
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));
	
	switch (type)
	{
	case IItem::Type::eLongGun:
	{
		// ���� �޽� ����
		m_WeaponMesh->SetMesh(CL::ResourcePath::MESH_RIFLE);
		m_WeaponMesh->m_Transform->SetPosition({ 0.07f, 0.0f, 0.2f });
		m_WeaponMesh->m_Transform->SetRotationFromVec({ 10.0f, 100.0f, 80.0f });
		m_WeaponMesh->m_Transform->SetScale({ 0.3f, 0.3f, 0.3f });

		// �����÷��� ����
		m_MuzzleFlash->m_Transform->SetPosition({ 0.0f, 1.0f, 0.0f });
		break;
	}

	case IItem::Type::eHandGun:
	{
		m_WeaponMesh->SetMesh(CL::ResourcePath::MESH_PISTOL);
		m_WeaponMesh->m_Transform->SetPosition({ 0.03f, 0.05f, 0.12f });
		m_WeaponMesh->m_Transform->SetRotationFromVec({ 180.0f, 270.0f, 180.0f });
		m_WeaponMesh->m_Transform->SetScale({ 0.028f, 0.028f, 0.03f });

		// �����÷��� ����
		m_MuzzleFlash->m_Transform->SetPosition({ -3.0f, -1.0f, 1.5f });
		break;
	}
		
	case IItem::Type::eFlask:
		break;
	case IItem::Type::eFireFlask:
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
}
