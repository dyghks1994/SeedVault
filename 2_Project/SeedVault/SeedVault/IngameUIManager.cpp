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
	/// SCV 로더
	/// ------------------------------------------
	m_CSVLoader = DLLEngine::FindGameObjectByName("CSVLoader");
	assert(m_CSVLoader != nullptr);

	/// ------------------------------------------
	/// 대사 매니저
	/// ------------------------------------------
	m_pDialogueManager = DLLEngine::FindGameObjectByName("Dialogue Manager");
	assert(m_pDialogueManager != nullptr);

	m_pDialogueManager->GetComponent<DialogueManager>()->m_DialogueText_V = m_CSVLoader->GetComponent<CSVLoader>()->GetDialogue();
	m_pDialogueManager->GetComponent<DialogueManager>()->SetTriggerPass();

	/// ------------------------------------------
	/// 에임 포인트
	/// ------------------------------------------
	m_aimPoint = DLLEngine::FindGameObjectByName("Aim Point");
	assert(m_aimPoint != nullptr);

	/// ------------------------------------------
	/// 퀘스트 텍스트
	/// ------------------------------------------
	m_questText = DLLEngine::FindGameObjectByName("Quest Text");
	assert(m_questText != nullptr);

	m_questText_Sub = DLLEngine::FindGameObjectByName("Quest Text Sub");
	assert(m_questText_Sub != nullptr);

	m_questText->GetComponent<QuestManager>()->m_QuestText_V = m_CSVLoader->GetComponent<CSVLoader>()->GetQuest();

	/// ------------------------------------------
	/// 주인공 생명바
	/// ------------------------------------------
	m_Life = DLLEngine::FindGameObjectByName("Player Life Stat");
	assert(m_Life != nullptr);

	/// ------------------------------------------
	/// 무기
	/// ------------------------------------------
	m_combetStat_Weapon = DLLEngine::FindGameObjectByName("Combet Weapon Stat");
	assert(m_combetStat_Weapon != nullptr);

	m_combetStat_Weapon_SubText = DLLEngine::FindGameObjectByName("Combet Weapon Stat Sub Text");
	assert(m_combetStat_Weapon_SubText != nullptr);

	/// ------------------------------------------
	/// 아이템
	/// ------------------------------------------
	m_combetStat_Fire = DLLEngine::FindGameObjectByName("Combet Fire Stat");
	assert(m_combetStat_Fire != nullptr);

	m_combetStat_Heal = DLLEngine::FindGameObjectByName("Combet Heal");
	assert(m_combetStat_Heal != nullptr);

	m_combetStat_AssistState = DLLEngine::FindGameObjectByName("Combet AssistState");
	assert(m_combetStat_AssistState != nullptr);

	/// ------------------------------------------
	/// 암살
	/// ------------------------------------------
	m_AssassinIcon = DLLEngine::FindGameObjectByName("AssassinIcon");
	assert(m_AssassinIcon != nullptr);

	/// ------------------------------------------
	/// 조수 생명바
	/// ------------------------------------------
	m_AssistLifeBar = DLLEngine::FindGameObjectByName("Assist Life Bar");
	assert(m_AssistLifeBar != nullptr);

	m_AssistLife = DLLEngine::FindGameObjectByName("Assist Life");
	assert(m_AssistLife != nullptr);

	/// ------------------------------------------
	/// 문서
	/// ------------------------------------------
	m_Document = DLLEngine::FindGameObjectByName("BB_Document");
	assert(m_Document != nullptr);

	/// ------------------------------------------
	/// 피효과
	/// ------------------------------------------
	m_Blood = DLLEngine::FindGameObjectByName("Combet Blood");
	assert(m_Blood != nullptr);

	/// ------------------------------------------
	/// 1.힐템
	/// ------------------------------------------
	m_BB_Heal = DLLEngine::FindGameObjectByName("BB_Heal");
	assert(m_BB_Heal != nullptr);

	/// ------------------------------------------
	/// 2.시드볼트 획득
	/// ------------------------------------------
	m_BB_SeedVault = DLLEngine::FindGameObjectByName("BB_SeedVault");
	assert(m_BB_SeedVault != nullptr);

	/// ------------------------------------------
	/// 3.핸드건
	/// ------------------------------------------
	m_BB_HandGun = DLLEngine::FindGameObjectByName("BB_HandGun");
	assert(m_BB_HandGun != nullptr);

	/// ------------------------------------------
	///4. 라이플
	/// ------------------------------------------
	m_BB_Rifle = DLLEngine::FindGameObjectByName("BB_Rifle");
	assert(m_BB_Rifle != nullptr);

	/// ------------------------------------------
	/// 5.크레딧 카드 획득
	/// ------------------------------------------
	m_BB_CardKeyGet = DLLEngine::FindGameObjectByName("BB_CardKeyGet");
	assert(m_BB_CardKeyGet != nullptr);

	/// ------------------------------------------
	/// 6.크레딧 카드 대기
	/// ------------------------------------------
	m_BB_CardKeyWait = DLLEngine::FindGameObjectByName("BB_CardKeyWait");
	assert(m_BB_CardKeyWait != nullptr);

	/// ------------------------------------------
	/// 7.매뉴얼 획득
	/// ------------------------------------------
	m_BB_ManualGet = DLLEngine::FindGameObjectByName("BB_ManualGet");
	assert(m_BB_ManualGet != nullptr);

	/// ------------------------------------------
	/// 8.패널 조정
	/// ------------------------------------------
	m_BB_PanelSetting = DLLEngine::FindGameObjectByName("BB_PanelSetting");
	assert(m_BB_PanelSetting != nullptr);

	/// ------------------------------------------
	/// 9.리프트 작동
	/// ------------------------------------------
	m_BB_LiftMove = DLLEngine::FindGameObjectByName("BB_PanelSetting");
	assert(m_BB_LiftMove != nullptr);

	/// ------------------------------------------
	/// 10.읽기
	/// ------------------------------------------
	m_BB_Read = DLLEngine::FindGameObjectByName("BB_Read");
	assert(m_BB_Read != nullptr);

	/// ------------------------------------------
	/// 11.문 열기
	/// ------------------------------------------
	m_BB_OpenDoor = DLLEngine::FindGameObjectByName("BB_OpenDoor");
	assert(m_BB_OpenDoor != nullptr);

}

void IngameUIManager::Update(float dTime)
{
	/// --------------------------------------
	/// 1. 에임포인트 UI 이미지 전환
	/// --------------------
	// 비조준 상태 인텍스 : [0]
	// 조준 상태 인텍스 : [1]
	// 맞은 상태 인텍스 : [2]
	m_aimPoint->GetComponent<Sprite2D>()->SetspriteIndex(0);

	// 오른쪽클릭 여부에따라서 인덱스를 바꿔주면 될거같다
	if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::CLICK_RIGHT))
		&& DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::CLICK_LEFT))
		&& true/* 적에게 총알이 맞았을때*/)
	{
		m_aimPoint->GetComponent<Sprite2D>()->SetspriteIndex(2);
	}
	else if (DLLInput::InputKey(static_cast<int>(CL::KeyCode::CLICK_RIGHT)))
	{
		m_aimPoint->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}

	/// --------------------------------------
	/// 2. HP UI 이미지 전환
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
	/// 3 - 1. 무기 UI 이미지 전환
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
	/// 3 - 2. 무기 UI 수량 전환
	/// --------------------------------------
	// 현재 가지고있는 총알의 개수
	uint _CurrentGunBulletCount = 0;
	if (_equipment->GetType() == IItem::Type::eLongGun || _equipment->GetType() == IItem::Type::eHandGun)
	{
		_CurrentGunBulletCount = static_cast<Gun*>(_equipment.get())->GetCurrentAmmoCount();
	}
	m_combetStat_Weapon->GetComponent<Text>()->PrintSpriteText((TCHAR*)std::to_wstring(_CurrentGunBulletCount).c_str());

	// 인벤토리에 가지고있는 탄창의 총 개수
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
	/// 4 - 1. 화염병 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Fire, IItem::Type::eFireFlask);

	/// --------------------------------------
	/// 5 - 1. 회복킷 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	InvenoryUISetting(m_combetStat_Heal, IItem::Type::eHealPotion);

	/// --------------------------------------
	/// 6 - 1. 알코올 UI 이미지 전환 및 수량 전환
	/// --------------------------------------
	if (m_PartnerSate->IsHit() == true)// 조수가 따라오는 상태
	{
		m_combetStat_AssistState->GetComponent<Sprite2D>()->SetspriteIndex(3);
	}
	else
	{
		m_combetStat_AssistState->GetComponent<Sprite2D>()->SetspriteIndex(2);
	}

	/// --------------------------------------
	/// 7 - 1. 암살 UI
	/// --------------------------------------	
	//플레이어의 암살상태가 true일떄 
	if (DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_bCanAssassinate == true)
	{
		m_AssassinIcon->GetComponent<Billboard>()->OnEnable();

		//플레이어가 인식한 적의 pos를 얻어와서 billboard의 포지션을 설정해준다
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
	/// 8 - 1. 조수 공격 게이지
	/// --------------------------------------	

	DirectX::SimpleMath::Vector3 _PartnerPos = m_PartnerTranform->GetWorldPosition();

	_PartnerPos += m_PartnerTranform->m_RotationTM.Up() * 0.6f;
	_PartnerPos += m_PartnerTranform->m_RotationTM.Right() * 0.3f;

	// 조수가 공격당할때만 뜬다.
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
	/// 9 - 1. 문서
	/// --------------------------------------	
	//문서 오브젝트에서 직접 상호작용 한다

	/// --------------------------------------
	/// 10 - 1. 피격시 피효과
	/// --------------------------------------	
	//HP를 저장해서 가지고있다가 HP수량이 떨어지면 피 
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
	/// 아이템 빌보드들
	/// ------------------------------------------

	if (DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->CanItemRouting())
	{
		// 아이템의 포지션
		GameObject* _RoutItem = DLLEngine::FindGameObjectByName("Player")->GetComponent<PlayerController>()->m_RoutingItem;
		Vector3 _pos = _RoutItem->GetComponent<Transform>()->GetWorldPosition();
		_pos.y += 0.3;

		/// 1. 힐팩일때
		if (_RoutItem->GetObjectName() == "HealPotion")
		{
			m_BB_Heal->GetComponent<Billboard>()->OnEnable();
			m_BB_Heal->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 3. 핸드건 탄창일때
		else if (_RoutItem->GetObjectName() == "HandGunAmmo")
		{
			m_BB_HandGun->GetComponent<Billboard>()->OnEnable();
			m_BB_HandGun->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 4. 라이플 탄창일때
		else if (_RoutItem->GetObjectName() == "LongGunAmmo")
		{
			m_BB_Rifle->GetComponent<Billboard>()->OnEnable();
			m_BB_Rifle->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 5. 카드키 획득 : 발전실(유일)
		else if (_RoutItem->GetObjectName() == "CardKey")
		{
			m_BB_CardKeyGet->GetComponent<Billboard>()->OnEnable();
			m_BB_CardKeyGet->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 7.매뉴얼 획득 : 발전실(유일)
		else if (_RoutItem->GetObjectName() == "File")
		{
			m_BB_ManualGet->GetComponent<Billboard>()->OnEnable();
			m_BB_ManualGet->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 2.시드볼트 획득 : 발전실(유일)
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
		
		/// 10.읽기 : 튜토(유일) 노트
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
		/// 8. (제너레이터)카드키 패널 조정 : 발전실(유일)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "Generator")
		{
			m_BB_PanelSetting->GetComponent<Billboard>()->OnEnable();
			m_BB_PanelSetting->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 9.리프트 작동  : 발전실(유일)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "Lift")
		{
			m_BB_LiftMove->GetComponent<Billboard>()->OnEnable();
			m_BB_LiftMove->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 11.문 열기 : 튜토(유일)
		else if (_InterectionItem->GetMyObject()->GetObjectName() == "Door")
		{
			m_BB_OpenDoor->GetComponent<Billboard>()->OnEnable();
			m_BB_OpenDoor->GetComponent<Transform>()->SetPosition(_pos);
		}
		/// 6. 카드키 대기 : 발전실(유일)
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
/// 아이템의 갯수를 세고 갯수가 0일 경우 이미지를 바꿔끼워줌
/// </summary>
/// <param name="UIgameObject"></param>
/// <param name="itemType"></param>
void IngameUIManager::InvenoryUISetting(GameObject* UIgameObject, IItem::Type itemType)
{
	auto _ItemCount = m_Inventory->GetItemCount(itemType);
	if (_ItemCount < 1)
	{
		//수량이 1 미만이면 off 이미지 출력
		UIgameObject->GetComponent<Sprite2D>()->SetspriteIndex(1);
	}
	else
	{
		UIgameObject->GetComponent<Sprite2D>()->SetspriteIndex(0);
	}

	UIgameObject->GetComponent<Text>()->PrintSpriteText((TCHAR*)std::to_wstring(_ItemCount).c_str());
}

