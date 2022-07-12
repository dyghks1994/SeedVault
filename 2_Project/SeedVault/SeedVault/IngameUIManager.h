#pragma once
#include "ComponentBase.h"
#include "IItem.h"

/// <summary>
/// UI���� Ŭ����
///  : UI �̹����� ���� ����[0]/����[1], 
/// 
/// 2022.05.25 B.Bunny
/// </summary>
class IngameUIManager : public ComponentBase
{
public:
	IngameUIManager();
	virtual ~IngameUIManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

private:

	// �÷��̾� ����
	class Health* m_health;

	// �÷��̾� �κ��丮
	class Inventory* m_Inventory;
	
	// ��Ʈ�� ����
	class Transform* m_PartnerTranform;
	class Partner_Move* m_PartnerSate;
	class Health* m_PartnerHealth;

	// ��ȭ ��� �Ŵ���
	GameObject* m_pDialogueManager;
	
	// CSVLoader
	GameObject* m_CSVLoader;

	GameObject* m_aimPoint;

	// ����Ʈ �ؽ�Ʈ
	GameObject* m_questText;
	GameObject* m_questText_Sub;
	
	GameObject* m_combetStat_Weapon;
	GameObject* m_combetStat_Weapon_SubText;
	
	GameObject* m_combetStat_Fire;
	GameObject* m_combetStat_Heal;

	GameObject* m_combetStat_AssistState;
	
	GameObject* m_Life;
	
	GameObject* m_Document;

	GameObject* m_Blood;

	// ������
	GameObject* m_AssassinIcon;
	
	GameObject* m_AssistLifeBar;
	GameObject* m_AssistLife;

	GameObject* m_BB_Heal;
	GameObject* m_BB_SeedVault;
	GameObject* m_BB_HandGun;
	GameObject* m_BB_Rifle;
	GameObject* m_BB_CardKeyGet;

	GameObject* m_BB_CardKeyWait;
	GameObject* m_BB_ManualGet;
	GameObject* m_BB_PanelSetting;
	GameObject* m_BB_LiftMove;
	GameObject* m_BB_Read;

	GameObject* m_BB_OpenDoor;


	float m_AssistLifeTime = 0;;

	void InvenoryUISetting(GameObject* UIgameObject, IItem::Type itemType);
};

