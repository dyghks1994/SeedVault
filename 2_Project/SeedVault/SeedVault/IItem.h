#pragma once

/// <summary>
/// ������ ���� Ŭ����
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

class IItem
{
public:
	enum class Type
	{
		eLongGun,
		eHandGun,
		eFlask,
		eFireFlask,
		eAlcohol,
		eHerb,
		eHealPotion,
		eLongGunAmmo,
		eHandGunAmmo,
		eCardKey,
		eFile,
		eMax,
	};

	IItem(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~IItem();

	const Type& GetType() const;
	const std::string& GetName() const;
	void SetName(std::string name);

protected:
	Type m_Type;
	std::string m_Name;

	class Inventory* m_MyInventory;

private:

};

class EquipmentItem : public IItem
{
public:
	EquipmentItem(IItem::Type type, class Inventory* inventory = nullptr);
	virtual ~EquipmentItem();

	virtual bool Use() abstract;
	virtual bool IsFinishCoolTimeTimer() const;		// ��Ÿ���� �� ���ҳ�?
	virtual void CoolDown();						// ��ٿ� ����


protected:
	float m_UseCoolTimeTimer;						// ���� ���� ��Ÿ��

private:

};

