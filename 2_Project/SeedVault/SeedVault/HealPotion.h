#pragma once
#include "IItem.h"

/// <summary>
/// �� �ֻ�� Ŭ����
/// 
/// �÷��̾� ĳ���Ͱ� ����ϸ� Hp�� ȸ���Ѵ�.
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
	float m_UseCoolTime;		// ���� ���� �ӵ�
	float m_HealPower;			// Hp ȸ����
private:
};

