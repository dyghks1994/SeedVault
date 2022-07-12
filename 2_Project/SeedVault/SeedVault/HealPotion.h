#pragma once
#include "IItem.h"

/// <summary>
/// 힐 주사기 클래스
/// 
/// 플레이어 캐릭터가 사용하면 Hp를 회복한다.
/// 
/// </summary>
class HealPotion : public EquipmentItem
{
public:
	HealPotion(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~HealPotion();

	virtual bool Use() override;
	void Heal();

	bool IsHealing() const;

protected:
	float m_UseCoolTime;		// 재사용 간격 속도
	float m_HealPower;			// Hp 회복량
private:
};

