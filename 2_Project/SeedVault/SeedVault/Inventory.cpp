#include "pch.h"
#include "IItem.h"
#include "Gun.h"
#include "Flask.h"
#include "ItemBuilder.h"
#include "CLTutorial.h"
#include "Inventory.h"

Inventory::Inventory()
	: ComponentBase(ComponentType::GameLogic)
	, _Owner(nullptr)
	, m_Inventory()
	, m_Slot()
	, m_EquiptedSlotIndex(0)
{
	
}

Inventory::~Inventory()
{
}

void Inventory::Start()
{
	_Owner = m_pMyObject;

	m_Inventory.reserve(static_cast<uint>(IItem::Type::eMax));
	m_Slot.reserve(5);

	/// �κ��丮 ������ �߰�
	{
		// ��� ���� �߰�
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eLongGun, this), 1));
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHandGun, this), 1));

		// �ö�ũ �߰�
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eFlask, this), 0));
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eFireFlask, this), 0));

		// �� ���� �߰�
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHealPotion, this), 0));

		// ź �߰�
		/// Ʃ�丮�� �� ���� ź�� ���� ����
		CLTutorial* _tutorialScene = dynamic_cast<CLTutorial*>(DLLEngine::GetNowScene());
		if (_tutorialScene != nullptr)
		{
			std::shared_ptr<LongGun> longgun = std::dynamic_pointer_cast <LongGun>(m_Inventory.at(0).first);
			if (longgun != nullptr)
			{
				longgun->SetCurrentAmmoCount(0);
			}

			std::shared_ptr<HandGun> handGun = std::dynamic_pointer_cast <HandGun>(m_Inventory.at(1).first);
			if (handGun != nullptr)
			{
				handGun->SetCurrentAmmoCount(0);
			}

			m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eLongGunAmmo, this), 0));
			m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHandGunAmmo, this), 0));
		}
		else
		{
			std::shared_ptr<LongGun> longgun = std::dynamic_pointer_cast <LongGun>(m_Inventory.at(0).first);
			if (longgun != nullptr)
			{
				longgun->SetCurrentAmmoCount(4);
			}

			std::shared_ptr<HandGun> handGun = std::dynamic_pointer_cast <HandGun>(m_Inventory.at(1).first);
			if (handGun != nullptr)
			{
				handGun->SetCurrentAmmoCount(8);
			}

			m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eLongGunAmmo, this), 2));
			m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHandGunAmmo, this), 4));
		}

		// �� ���� �߰�
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHealPotion, this), 10));

		// ���ڿ�, ��� �߰�
		//m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eAlcohol, this), 2));
		//m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eHerb, this), 3));

		// ī�� Ű �߰�
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eCardKey, this), 0));
		// ����
		m_Inventory.emplace_back(std::make_pair(ItemBuilder::CreateItem(IItem::Type::eFile, this), 0));
	}

	/// ���Կ� ��� �߰�
	{
		// �ֹ���, �������� ���Կ� �߰�
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eLongGun)));
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eHandGun)));

		// �ö�ũ, ȭ�� �ö�ũ ���Կ� �߰�
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eFlask)));
		//m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eFireFlask)));

		// �� ���� ���Կ� �߰�
		m_Slot.emplace_back(std::dynamic_pointer_cast<EquipmentItem>(FindItem(IItem::Type::eHealPotion)));
	}

	// ���� ���
	m_EquiptedSlotIndex = 0;
	m_EquipedItem = m_Slot.at(m_EquiptedSlotIndex);




}

void Inventory::Update(float dTime)
{
	std::shared_ptr<EquipmentItem> _currentEquipment = GetCurrentEquipment();
	assert(_currentEquipment != nullptr);

	// ���� ���Ե��� ��� ���Ұ� ���¸� ��Ÿ���� ������.
	for (const auto& equipment : m_Slot)
	{
		if (equipment->IsFinishCoolTimeTimer() == false)
		{
			equipment->CoolDown();
		}

		// ���̶�� ���� Ÿ�̸ӵ� ������.
		auto _gun = std::dynamic_pointer_cast<Gun>(equipment);
		if (_gun != nullptr)
		{
			if (_gun->IsReloading() == true)
			{
				_gun->ReloadCoolDown();
			}
		}
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_I)))
	{
		PrintInfo();
	}
}

void Inventory::OnRender()
{
}

/// <summary>
/// �κ��丮���� type�� �ش��ϴ� �������� ã�Ƽ� ����
/// </summary>
/// <param name="type">ã�� �������� type</param>
/// <returns>������</returns>
std::shared_ptr<class IItem> Inventory::FindItem(IItem::Type type) const
{
	for (const auto& item : m_Inventory)
	{
		if (item.first->GetType() == type)
		{
			return item.first;
		}
	}

	return nullptr;
}

/// <summary>
/// type�� �ش��ϴ� �������� ã�Ƽ� 
/// �κ��丮�� �����ִ� ������ ����
/// </summary>
/// <param name="type">ã�� �������� Ÿ��</param>
/// <returns>���� ����</returns>
uint Inventory::GetItemCount(IItem::Type type) const
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			return (*_iter).second;
		}
	}

	return 0;
}

/// <summary>
/// type�� �ش��ϴ� �������� ã�Ƽ�
/// ������ ����
/// </summary>
/// <param name="type">���� ������ ������ Ÿ��</param>
/// <param name="count">������ ����</param>
void Inventory::SetItemCount(const IItem::Type type, uint&& count)
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			(*_iter).second = count;

			if ((*_iter).second < 0)
			{
				(*_iter).second = 0;
			}
		}
	}
}

/// <summary>
/// type �� �ش��ϴ� �������� ã�Ƽ� 
/// val ��ŭ ������ ����
/// </summary>
/// <param name="type">������ų ������ Ÿ��</param>
/// <param name="val">������ų ��</param>
/// <returns>���� �� ����</returns>
uint Inventory::IncreaseItemCount(const IItem::Type type, const uint&& val)
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			(*_iter).second += val;

			break;
		}
	}

	return (*_iter).second;
}

/// <summary>
/// type �� �ش��ϴ� �������� ã�Ƽ� 
/// val ��ŭ ������ ����
/// </summary>
/// <param name="type">���ҽ�ų ������ Ÿ��</param>
/// <param name="val">���ҽ�ų ��</param>
/// <returns>���� �� ����</returns>
uint Inventory::DecreaseItemCount(const IItem::Type type, const uint&& val)
{
	auto _iter = m_Inventory.begin();

	for (; _iter != m_Inventory.end(); ++_iter)
	{
		if ((*_iter).first->GetType() == type)
		{
			(*_iter).second -= val;

			break;
		}
	}

	return (*_iter).second;
}

/// <summary>
/// ���� ������� ��� ������ ����
/// </summary>
/// <returns></returns>
std::shared_ptr<class EquipmentItem> Inventory::GetCurrentEquipment() const
{
	return m_Slot.at(m_EquiptedSlotIndex);
}

/// <summary>
/// ��� ����
/// ����ϰ� ���� ��ȣ�� ���ڷ� �޾Ƽ�
/// �ش� ������ ���� ������� ��ü
/// </summary>
/// <param name="slotNum">����� ���� �ѹ�</param>
/// <returns>��� ��ü ���� ����</returns>
bool Inventory::ChangeWeapon(const uint& slotNum)
{
	if (slotNum == m_EquiptedSlotIndex)
	{
		CA_TRACE("[Inventory] ChangeWeapon ���� -> (���� ����� ��������)");
		return false;
	}

	// ��� ��ü�� �� ������ Ȯ���ؼ� �������̸�
	// ������ ����Ѵ�.
	std::shared_ptr<Gun> _gun = std::dynamic_pointer_cast<Gun>(m_EquipedItem);
	if (_gun != nullptr)
	{
		if (_gun->IsReloading() == true)
		{
			_gun->SetReloadCoolTimer(0.0f);
			CA_TRACE("[Inventory] ChangeWeapon() ���� ���");
		}
	}

	m_EquiptedSlotIndex = slotNum;
	m_EquipedItem = m_Slot.at(m_EquiptedSlotIndex);

	CA_TRACE("[Inventory] ChangeWeapon -> %d, %s", m_EquiptedSlotIndex + 1, m_EquipedItem->GetName().c_str());

	return true;
}

void Inventory::PrintInfo() const
{
	CA_TRACE("[Inventory] PrintInfo -------------");

	for (size_t i = 0; i < m_Inventory.size(); i++)
	{
		CA_TRACE("[%d] %s - %d", i + 1, m_Inventory.at(i).first->GetName().c_str(), m_Inventory.at(i).second);
	}
	CA_TRACE("-----------------------------------");
}
