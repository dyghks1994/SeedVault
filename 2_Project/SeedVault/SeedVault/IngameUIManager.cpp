#include "pch.h"
#include "IngameUIManager.h"
#include "DialogueManager.h"
#include "QuestManager.h"
#include "Inventory.h"
#include "Health.h"
#include "Gun.h"
#include "PlayerController.h"
#include "Enemy_Move.h"
#include "EffectUIEnum.h"
#include "Partner_Move.h"
#include "CInteraction.h"


IngameUIManager::IngameUIManager()
	: ComponentBase(ComponentType::GameLogic)
{
}

IngameUIManager::~IngameUIManager()
{
}

void IngameUIManager::Start()
{
	m_health = DLLEngine::FindGameObjectByName("Player")->GetComponent<Health>();
	assert(m_health != nullptr);

	m_Inventory = DLLEngine::FindGameObjectByName("Player")->GetComponent<Inventory>();
	assert(m_Inventory != nullptr);

	m_PartnerTranform = DLLEngine::FindGameObjectByName("Partner")->GetComponent<Transform>();
	assert(m_PartnerTranform != nullptr);

	m_PartnerSate = DLLEngine::FindGameObjectByName("Partner")->GetComponent<Partner_Move>();
	assert(m_PartnerSate != nullptr);

	m_PartnerHealth = DLLEngine::FindGameObjectByName("Partner")->GetComponent<Health>();
	assert(m_PartnerHealth != nullptr);

	/// ------------------------------------------
	/// SCV �δ�
	/// ------------------------------------------
	m_CSVLoader = DLLEngine::FindGameObjectByName("CSVLoader");
	assert(m_CSVLoader != nullptr);

	/// ------------------------------------------
	/// ��� �Ŵ���
	/// ------------------------------------------
	m_pDialogueManager = DLLEngine::FindGameObjectByName("Dialogue Manager");
	assert(m_pDialogueManager != nullptr);

	m_pDialogueManager->GetComponent<DialogueManager>()->m_DialogueText_V = m_CSVLoader->GetComponent<CSVLoader>()->GetDialogue();
	m_pDialogueManager->GetComponent<DialogueManager>()->SetTriggerPass();

	/// ------------------------------------------
	/// ���� ����Ʈ
	/// ------------------------------------------
	m_aimPoint = DLLEngine::FindGameObjectByName("Aim Point");
	assert(m_aimPoint != nullptr);

	/// ------------------------------------------
	/// ����Ʈ �ؽ�Ʈ
	/// ------------------------------------------
	m_questText = DLLEngine::FindGameObjectByName("Quest Text");
	assert(m_questText != nullptr);

	m_questText_Sub = DLLEngine::FindGameObjectByName("Quest Text Sub");
	assert(m_questText_Sub != nullptr);

	m_questText->GetComponent<QuestManager>()->m_QuestText_V = m_CSVLoader->GetComponent<CSVLoader>()->GetQuest();

	/// ------------------------------------------
	/// ���ΰ� �����
	/// ------------------------------------------
	m_Life = DLLEngine::FindGameObjectByName("Player Life Stat");
	assert(m_Life != nullptr);

	/// ------------------------------------------
	/// ����
	/// ------------------------------------------
	m_combetStat_Weapon = DLLEngine::FindGameObjectByName("Combet Weapon Stat");
	assert(m_combetStat_Weapon != nullptr);

	m_combetStat_Weapon_SubText = DLLEngine::FindGameObjectByName("Combet Weapon Stat Sub Text");
	assert(m_combetStat_Weapon_SubText != nullptr);

	/// ------------------------------------------
	/// ������
	/// ------------------------------------------
	m_combetStat_Fire = DLLEngine::FindGameObjectByName("Combet Fire Stat");
	assert(m_combetStat_Fire != nullptr);

	m_combetStat_Heal = DLLEngine::FindGameObjectByName("Combet Heal");
	assert(m_combetStat_Heal != nullptr);

	m_combetStat_AssistState = DLLEngine::FindGameObjectByName("Combet AssistState");
	assert(m_combetStat_AssistState != nullptr);

	/// ------------------------------------------
	/// �ϻ�
	/// ------------------------------------------
	m_AssassinIcon = DLLEngine::FindGameObjectByName("AssassinIcon");
	assert(m_AssassinIcon != nullptr);

	/// ------------------------------------------
	/// ���� �����
	/// ------------------------------------------
	m_AssistLifeBar = DLLEngine::FindGameObjectByName("Assist Life Bar");
	assert(m_AssistLifeBar != nullptr);

	m_AssistLife = DLLEngine::FindGameObjectByName("Assist Life");
	assert(m_AssistLife != nullptr);

	/// ------------------------------------------
	/// ����
	/// ------------------------------------------
	m_Document = DLLEngine::FindGameObjectByName("BB_Document");
	assert(m_Document != nullptr);

	/// ------------------------------------------
	/// ��ȿ��
	/// ------------------------------------------
	m_Blood = DLLEngine::FindGameObjectByName("Combet Blood");
	assert(m_Blood != nullptr);

	/// ------------------------------------------
	/// 1.����
	/// ------------------------------------------
	m_BB_Heal = DLLEngine::FindGameObjectByName("BB_Heal");
	assert(m_BB_Heal != nullptr);

	/// ------------------------------------------
	/// 2.�õ庼Ʈ ȹ��
	/// ------------------------------------------
	m_BB_SeedVault = DLLEngine::FindGameObjectByName("BB_SeedVault");
	assert(m_BB_SeedVault != nullptr);

	/// ------------------------------------------
	/// 3.�ڵ��
	/// ------------------------------------------
	m_BB_HandGun = DLLEngine::FindGameObjectByName("BB_HandGun");
	assert(m_BB_HandGun != nullptr);

	/// ------------------------------------------
	///4. ������
	/// ------------------------------------------
	m_BB_Rifle = DLLEngine::FindGameObjectByName("BB_Rifle");
	assert(m_BB_Rifle != nullptr);

	/// ------------------------------------------
	/// 5.ũ���� ī�� ȹ��
	/// ------------------------------------------
	m_BB_CardKeyGet = DLLEngine::FindGameObjectByName("BB_CardKeyGet");
	assert(m_BB_CardKeyGet != nullptr);

	/// ------------------------------------------
	/// 6.ũ���� ī�� ���
	/// ------------------------------------------
	m_BB_CardKeyWait = DLLEngine::FindGameObjectByName("BB_CardKeyWait");
	assert(m_BB_CardKeyWait != nullptr);

	/// ------------------------------------------
	/// 7.�Ŵ��� ȹ��
	/// ------------------------------------------
	m_BB_ManualGet = DLLEngine::FindGameObjectByName("BB_ManualGet");
	assert(m_BB_ManualGet != nullptr);

	/// ------------------------------------------
	/// 8.�г� ����
	/// ------------------------------------------
	m_BB_PanelSetting = DLLEngine::FindGameObjectByName("BB_PanelSetting");
	assert(m_BB_PanelSetting != nullptr);

	/// ------------------------------------------
	/// 9.����Ʈ �۵�
	/// ------------------------------------------
	m_BB_LiftMove = DLLEngine::FindGameObjectByName("BB_PanelSetting");
	assert(m_BB_LiftMove != nullptr);

	/// ------------------------------------------
	/// 10.�б�
	/// ------------------------------------------
	m_BB_Read = DLLEngine::FindGameObjectByName("BB_Read");
	assert(m_BB_Read != nullptr);

	/// ------------------------------------------
	/// 11.�� ����
	/// ------------------------------------------
	m_BB_OpenDoor = DLLEngine::FindGameObjectByName("BB_OpenDoor");
	assert(m_BB_OpenDoor != nullptr);

}

void IngameUIManager::Update(float dTime)
{
	/// --------------------------------------
	/// 1. ��������Ʈ UI �̹��� ��ȯ
	/// --------------------
	// ������ ���� ���ؽ� : [0]
	// ���� ���� ���ؽ� : [1]
	// ���� ���� ���ؽ� : [2]
	m_aimPoint->GetComponent<Sprite2D>()->SetspriteIndex(0);

	// ������Ŭ�� ���ο����� �ε����� �ٲ��ָ� �ɰŰ���
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::CLICK_RIGHT))
		&& DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::CLICK_LEFT))
		&& true/* ������ �Ѿ��� �¾�����*/)
	{
		m_aimPoint->GetComponent<Sprite2D>()->SetspriteIndex(2);
	}
	else if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::CLICK_RIGHT)))
	{
		m_aimPoint->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}

	/// --------------------------------------
	/// 2. HP UI �̹��� ��ȯ
	/// --------------------------------------
	unsigned int _HPIndex = static_cast<int>(m_health->GetHp());

	if (m_health->GetHp() < 0)
	{
		_HPIndex = 0;
	}

	if (m_health->GetHp() > 6)
	{
		_HPIndex = 6;
	}

	if (_HPIndex >= 0 || _HPIndex <= 6)
	{
		m_Life->GetComponent<Sprite2D>()->SetspriteIndex(static_cast<int>(_HPIndex));
	}

	/// --------------------------------------
	/// 3 - 1. ���� UI �̹��� ��ȯ
	/// --------------------------------------
	auto _equipment = m_Inventory->GetCurrentEquipment();

	if (_equipment->GetType() == IItem::Type::eHandGun)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(0);
	}
	else if (_equipment->GetType() == IItem::Type::eLongGun)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}
	else if (_equipment->GetType() == IItem::Type::eFlask)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(2);
	}
	else if (_equipment->GetType() == IItem::Type::eFireFlask)
	{
		m_combetStat_Weapon->GetComponent<Sprite2D>()->SetspriteIndex(3);
	}

	/// --------------------------------------
	/// 3 - 2. ���� UI ���� ��ȯ
	/// --------------------------------------
	// ���� �������ִ� �Ѿ��� ����
	uint _CurrentGunBulletCount = 0;
	if (_equipment->GetType() == IItem::Type::eLongGun || _equipment->GetType() == IItem::Type::eHandGun)
	{
		_CurrentGunBulletCount = static_cast<Gun*>(_equipment.get())->GetCurrentAmmoCount();
	}
	m_combetStat_Weapon->GetComponent<Text>()->PrintSpriteText((TCHAR*)std::to_wstring(_CurrentGunBulletCount).c_str());

	// �κ��丮�� �������ִ� źâ�� �� ����
	uint _AmoCount = 0;
	if (_equipment->GetType() == IItem::Type::eLongGun)
	{
		m_combetStat_Weapon_SubText->GetComponent<Text>()->OnEnable();
		_AmoCount = m_Inventory->GetItemCount(IItem::Type::eLongGunAmmo);
	}
	else if (_equipment->GetType() == IItem::Type::eHandGun)
	{
		m_combetStat_Weapon_SubText->GetComponent<Text>()->OnEnable();
		_AmoCount = m_Inventory->GetItemCount(IItem::Type::eHandGunAmmo);
	}
	else
	{
		m_combetStat_Weapon_SubText->GetComponent<Text>()->OnDisable();
	}

	std::wstring __AmoCountString = L"/" + std::to_wstring(_AmoCount);

	m_combetStat_Weapon_SubText->GetComponent<Text>()->PrintSpriteText((TCHAR*)__AmoCountString.c_str());

	/// --------------------------------------
	/// 4 - 1. ȭ���� UI �̹��� ��ȯ �� ���� ��ȯ
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Fire, IItem::Type::eFireFlask);

	/// --------------------------------------
	/// 5 - 1. ȸ��Ŷ UI �̹��� ��ȯ �� ���� ��ȯ
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Heal, IItem::Type::eHealPotion);

	/// --------------------------------------
	/// 6 - 1. ���ڿ� UI �̹��� ��ȯ �� ���� ��ȯ
	/// --------------------------------------
	if (m_PartnerSate->IsHit() == true)// ������ ������� ����
	{
		m_combetStat_AssistState->GetComponent<Sprite2D>()->SetspriteIndex(3);
	}
	else
	{
		m_combetStat_AssistState->GetComponent<Sprite2D>()->SetspriteIndex(2);
	}

	/// --------------------------------------
	/// 7 - 1. �ϻ� UI
	/// --------------------------------------	
	//�÷��̾��� �ϻ���°� true�ϋ� 
	if (DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_bCanAssassinate == true)
	{
		m_AssassinIcon->GetComponent<Billboard>()->OnEnable();

		//�÷��̾ �ν��� ���� pos�� ���ͼ� billboard�� �������� �������ش�
		Vector3 _AssassinTargetPos = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->GetAssassinateTarget()->m_Transform->GetWorldPosition();

		SimpleMath::Vector3 _pos = _AssassinTargetPos;
		_pos += DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->GetAssassinateTarget()->m_Transform->m_RotationTM.Right() * 0.3f;
		_pos += DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->GetAssassinateTarget()->m_Transform->m_RotationTM.Up() * 0.7f;

		m_AssassinIcon->GetComponent<Transform>()->SetPosition(_pos);
	}
	else
	{
		m_AssassinIcon->GetComponent<Billboard>()->OnDisable();
	}

	/// --------------------------------------
	/// 8 - 1. ���� ���� ������
	/// --------------------------------------	

	DirectX::SimpleMath::Vector3 _PartnerPos = m_PartnerTranform->GetWorldPosition();

	_PartnerPos += m_PartnerTranform->m_RotationTM.Up() * 0.6f;
	_PartnerPos += m_PartnerTranform->m_RotationTM.Right() * 0.3f;

	// ������ ���ݴ��Ҷ��� ���.
	m_AssistLifeBar->GetComponent<Billboard>()->SetEnable(m_PartnerSate->IsHit());
	m_AssistLife->GetComponent<Billboard>()->SetEnable(m_PartnerSate->IsHit());

	if (m_PartnerSate->IsHit())
	{
		m_AssistLifeTime += dTime;

		m_AssistLifeBar->GetComponent<Transform>()->SetPosition(_PartnerPos);
		m_AssistLife->GetComponent<Transform>()->SetPosition(_PartnerPos);

		m_AssistLife->GetComponent<Billboard>()->DecreaseX(m_AssistLifeTime, 10.f);

		if (m_AssistLifeTime > 10.f)
		{
			m_AssistLifeBar->GetComponent<Billboard>()->OnDisable();
			m_AssistLife->GetComponent<Billboard>()->OnDisable();
		}
	}
	else
	{
		m_AssistLifeTime = 0;
	}

	/// --------------------------------------
	/// 9 - 1. ����
	/// --------------------------------------	
	//���� ������Ʈ���� ���� ��ȣ�ۿ� �Ѵ�

	/// --------------------------------------
	/// 10 - 1. �ǰݽ� ��ȿ��
	/// --------------------------------------	
	//HP�� �����ؼ� �������ִٰ� HP������ �������� �� 
	if (m_health->GetHp() > 5)
	{
		m_Blood->GetComponent<Sprite2D>()->OnDisable();
	}
	else if (m_health->GetHp() > 3)
	{
		m_Blood->GetComponent<Sprite2D>()->OnEnable();
		m_Blood->GetComponent<Sprite2D>()->SetspriteIndex(0);
	}
	else if (m_health->GetHp() > 1)
	{
		m_Blood->GetComponent<Sprite2D>()->OnEnable();
		m_Blood->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}
	else if (m_health->GetHp() > 0)
	{
		m_Blood->GetComponent<Sprite2D>()->OnDisable();
	}

	/// ------------------------------------------
	/// ������ �������
	/// ------------------------------------------

	if (DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->CanItemRouting())
	{
		// �������� ������
		GameObject* _RoutItem = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_RoutingItem;
		Vector3 _pos = _RoutItem->GetComponent<Transform>()->GetWorldPosition();
		_pos.y += 0.3;

		/// 1. �����϶�
		if (_RoutItem->GetObjectName() == "HealPotion")
		{
			m_BB_Heal->GetComponent<Billboard>()->OnEnable();
			m_BB_Heal->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 3. �ڵ�� źâ�϶�
		else if (_RoutItem->GetObjectName() == "HandGunAmmo")
		{
			m_BB_HandGun->GetComponent<Billboard>()->OnEnable();
			m_BB_HandGun->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 4. ������ źâ�϶�
		else if (_RoutItem->GetObjectName() == "LongGunAmmo")
		{
			m_BB_Rifle->GetComponent<Billboard>()->OnEnable();
			m_BB_Rifle->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 5. ī��Ű ȹ�� : ������(����)
		else if (_RoutItem->GetObjectName() == "CardKey")
		{
			m_BB_CardKeyGet->GetComponent<Billboard>()->OnEnable();
			m_BB_CardKeyGet->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 7.�Ŵ��� ȹ�� : ������(����)
		else if (_RoutItem->GetObjectName() == "File")
		{
			m_BB_ManualGet->GetComponent<Billboard>()->OnEnable();
			m_BB_ManualGet->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 2.�õ庼Ʈ ȹ�� : ������(����)
		//else if (_RoutItem->GetObjectName() == "LongGunAmmo")
		//{
		//	m_BB_SeedVault->GetComponent<Billboard>()->OnEnable();
		//	m_BB_SeedVault->GetComponent<Transform>()->SetPosition(_pos);
		//}
	}
	else
	{
		m_BB_Heal->GetComponent<Billboard>()->OnDisable();
		m_BB_HandGun->GetComponent<Billboard>()->OnDisable();
		m_BB_Rifle->GetComponent<Billboard>()->OnDisable();
		m_BB_CardKeyGet->GetComponent<Billboard>()->OnDisable();

		m_BB_ManualGet->GetComponent<Billboard>()->OnDisable();
		m_BB_SeedVault->GetComponent<Billboard>()->OnDisable();
	}

	if (DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_InteractionObj != nullptr)
	{
		CInteraction* _InterectionItem = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_InteractionObj;
		Vector3 _pos = _InterectionItem->GetMyObject()->GetComponent<Transform>()->GetWorldPosition();
		_pos.y += 0.3;
		
		std::string temp = _InterectionItem->GetMyObject()->GetObjectName();
		
		/// 10.�б� : Ʃ��(����) ��Ʈ
		if (_InterectionItem->GetMyObject()->GetObjectName() == "Note")
		{
			if (_InterectionItem->isInterect == false)
			{
				m_BB_Read->GetComponent<Transform>()->SetPosition(_pos);
				m_BB_Read->GetComponent<Billboard>()->OnEnable();
			}
			else
			{
				m_BB_Read->GetComponent<Billboard>()->OnDisable();
			}
			
		}
		/// 8. (���ʷ�����)ī��Ű �г� ���� : ������(����)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "Generator")
		{
			m_BB_PanelSetting->GetComponent<Billboard>()->OnEnable();
			m_BB_PanelSetting->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 9.����Ʈ �۵�  : ������(����)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "Lift")
		{
			m_BB_LiftMove->GetComponent<Billboard>()->OnEnable();
			m_BB_LiftMove->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 11.�� ���� : Ʃ��(����)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "Door")
		{
			m_BB_OpenDoor->GetComponent<Billboard>()->OnEnable();
			m_BB_OpenDoor->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 6. ī��Ű ��� : ������(����)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "CardKeyPad")
		{
			m_BB_CardKeyWait->GetComponent<Billboard>()->OnEnable();
			m_BB_CardKeyWait->GetComponent<Transform>()->SetPosition(_pos);
		}
	}
	else
	{
		m_BB_CardKeyWait->GetComponent<Billboard>()->OnDisable();
		m_BB_PanelSetting->GetComponent<Billboard>()->OnDisable();
		m_BB_LiftMove->GetComponent<Billboard>()->OnDisable();
		m_BB_Read->GetComponent<Billboard>()->OnDisable();
		m_BB_OpenDoor->GetComponent<Billboard>()->OnDisable();
	}

}

void IngameUIManager::OnRender()
{
}

/// <summary>
/// �������� ������ ���� ������ 0�� ��� �̹����� �ٲ㳢����
/// </summary>
/// <param name="UIgameObject"></param>
/// <param name="itemType"></param>
void IngameUIManager::InvenoryUISetting(GameObject* UIgameObject, IItem::Type itemType)
{
	auto _ItemCount = m_Inventory->GetItemCount(itemType);
	if (_ItemCount < 1)
	{
		//������ 1 �̸��̸� off �̹��� ���
		UIgameObject->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}
	else
	{
		UIgameObject->GetComponent<Sprite2D>()->SetspriteIndex(0);
	}

	UIgameObject->GetComponent<Text>()->PrintSpriteText((TCHAR*)std::to_wstring(_ItemCount).c_str());
}

