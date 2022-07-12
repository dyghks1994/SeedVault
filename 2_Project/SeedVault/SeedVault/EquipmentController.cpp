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
	// 부모 트랜스폼(오른손) 위치 연결
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// 플레이어 컨트롤러 연결
	m_PlayerController = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>();

	// 무기 메쉬 오브젝트 연결
	GameObject* equipedMesh = DLLEngine::FindGameObjectByName("EquipedMesh");
	if (equipedMesh != nullptr)
	{
		m_WeaponMesh = equipedMesh->GetComponent<MeshFilter>();
	}
	assert(m_WeaponMesh != nullptr);

	// 머즐 플래쉬 연결
	m_MuzzleFlash = DLLEngine::FindGameObjectByName("MuzzleFlash");
	assert(m_MuzzleFlash != nullptr);
}

void EquipmentController::Update(float dTime)
{
	// 플레이어가 죽어있으면 갱신하지 않음.
	if (m_PlayerController->m_bIsDead == true)
	{
		return;
	}

	// 부모 트랜스폼(오른손) 위치 연결
	m_PlayerMesh = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));

	// 타겟 오브젝트의 트랜스폼 성분d을 구한다
	m_WeaponParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// 타겟의 성분으로 myObject의 트랜스폼 구성
	m_Transform->SetPosition(m_TargetTranslation);
	m_Transform->SetRotation(m_TargetQuaternion);
	m_Transform->SetScale(m_TargetScale);

	//CA_TRACE("pos - %f, %f, %f", m_Transform->m_Position.x, m_Transform->m_Position.y, m_Transform->m_Position.z);
}

void EquipmentController::OnRender()
{
}

/// <summary>
/// 무기 교체를 했을 때 실제로 무기 메쉬와 트랜스폼을 변경, 조정하는 기능
/// </summary>
/// <param name="type"></param>
void EquipmentController::ChangeWeaponMesh(IItem::Type type)
{
	// 바뀐 플레이어 메쉬를 기준으로 부모 포지션을 다시 갱신
	m_WeaponParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMesh->GetBoneDataWorldTM("RightHand"));
	
	switch (type)
	{
	case IItem::Type::eLongGun:
	{
		// 무기 메쉬 조절
		m_WeaponMesh->SetMesh(CL::ResourcePath::MESH_RIFLE);
		m_WeaponMesh->m_Transform->SetPosition({ 0.07f, 0.0f, 0.2f });
		m_WeaponMesh->m_Transform->SetRotationFromVec({ 10.0f, 100.0f, 80.0f });
		m_WeaponMesh->m_Transform->SetScale({ 0.3f, 0.3f, 0.3f });

		// 머즐플래쉬 조절
		m_MuzzleFlash->m_Transform->SetPosition({ 0.0f, 1.0f, 0.0f });
		break;
	}

	case IItem::Type::eHandGun:
	{
		m_WeaponMesh->SetMesh(CL::ResourcePath::MESH_PISTOL);
		m_WeaponMesh->m_Transform->SetPosition({ 0.03f, 0.05f, 0.12f });
		m_WeaponMesh->m_Transform->SetRotationFromVec({ 180.0f, 270.0f, 180.0f });
		m_WeaponMesh->m_Transform->SetScale({ 0.028f, 0.028f, 0.03f });

		// 머즐플래쉬 조절
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
