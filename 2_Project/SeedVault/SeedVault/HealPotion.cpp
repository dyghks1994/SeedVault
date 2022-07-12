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
/// Hp 회복
/// 사용 제어(컨트롤)은 플레이어 컨트롤러에서 한다..
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
            CA_TRACE("[HealPotion] Hp 회복! - Hp = %f", _health->GetHp());

            return true;
        }
    }
    return false;
}

/// <summary>
/// 사용이 완료디고 힐이 적용하는 함수
/// 인벤토리의 주인 오브젝트의 힐을 수행한다.
/// 
/// 주인 오브젝트에 Health 컴포넌트 필요
/// </summary>
void HealPotion::Heal()
{
    GameObject* _Owner = m_MyInventory->GetMyObject();
    Health* _Health = _Owner->GetComponent<Health>();
    assert(_Health != nullptr);

    _Health->IncreaseHp(m_HealPower);

    // 플레이어 힐 애니메이션 중지
    m_MyInventory->GetMyObject()->GetComponent<PlayerController>()->m_bIsHealing = false;
}

/// <summary>
/// 지금 힐링 중인가?
/// 힐 시퀀스 시작후 애니메이션 동작하는 구간
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
