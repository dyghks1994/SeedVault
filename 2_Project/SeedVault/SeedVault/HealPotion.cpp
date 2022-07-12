#include "pch.h"
#include "Inventory.h"
#include "PlayerController.h"
#include "Health.h"
#include "HealPotion.h"



HealPotion::HealPotion(IItem::Type type, Inventory* inventory)
    : EquipmentItem(type, inventory)
    , m_UseCoolTime(0.0f)
    , m_HealPower(0)
{
    m_Name = "HealPotion";
    m_UseCoolTime = 3.0f;
    m_HealPower = 5.0f;
}

HealPotion::~HealPotion()
{
}

/// <summary>
/// Hp ȸ��
/// ��� ����(��Ʈ��)�� �÷��̾� ��Ʈ�ѷ����� �Ѵ�..
/// </summary>
/// <returns></returns>
bool HealPotion::Use()
{
    if (IsFinishCoolTimeTimer() == true)
    {
        Health* _health = m_MyInventory->GetMyObject()->GetComponent<Health>();
        if (_health != nullptr)
        {
            _health->IncreaseHp(m_HealPower);
            CA_TRACE("[HealPotion] Hp ȸ��! - Hp = %f", _health->GetHp());

            return true;
        }
    }
    return false;
}

/// <summary>
/// ����� �Ϸ��� ���� �����ϴ� �Լ�
/// �κ��丮�� ���� ������Ʈ�� ���� �����Ѵ�.
/// 
/// ���� ������Ʈ�� Health ������Ʈ �ʿ�
/// </summary>
void HealPotion::Heal()
{
    GameObject* _Owner = m_MyInventory->GetMyObject();
    Health* _Health = _Owner->GetComponent<Health>();
    assert(_Health != nullptr);

    _Health->IncreaseHp(m_HealPower);

    // �÷��̾� �� �ִϸ��̼� ����
    m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsHealing = false;
}

/// <summary>
/// ���� ���� ���ΰ�?
/// �� ������ ������ �ִϸ��̼� �����ϴ� ����
/// </summary>
/// <returns></returns>
bool HealPotion::IsHealing() const
{
    if (m_UseCoolTimeTimer > 0.0f)
    {
        return true;
    }

    else
    {
        return false;
    }
}
