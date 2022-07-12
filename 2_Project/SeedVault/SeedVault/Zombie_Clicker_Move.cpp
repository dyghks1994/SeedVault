#include "pch.h"
#include "MathHelper.h"
#include "PlayerController.h"
#include "Partner_Move.h"
#include "Health.h"
#include "NavMeshAgent.h"
#include "Inventory.h"
#include "Zombie_Clicker_Move.h"

Zombie_Clicker_Move::Zombie_Clicker_Move()
{
    CA_TRACE("[Zombie_Clicker_Move] Create!");
}

Zombie_Clicker_Move::~Zombie_Clicker_Move()
{
    CA_TRACE("[Zombie_Clickerr_Move] Create!");
}

void Zombie_Clicker_Move::Start()
{
    Enemy_Move::Start();

    /// 초기 상태 지정
    SetWait();

    /// Health 세팅
    m_Health->SetMaxHp(4.0f);
    m_Health->SetHp(4.0f);

    /// 속도 설정
    m_WaitTime = 5.0f;
    m_Timer = m_WaitTime;

    m_NavMeshAgent->m_MoveSpeed = m_MoveSpeed;

    /// 공격력, 공격범위 설정
    float _playerBoundingOffset = 0.0f;
    if (m_Player->GetComponent<PhysicsActor>() != nullptr)
    {
        _playerBoundingOffset = m_Player->GetComponent<PhysicsActor>()->GetBoundingOffset().Forward;
    }

    float _myBoundingOFfset = m_PhysicsActor->GetBoundingOffset().Forward;

    // 공격범위
    // 바운딩 볼륨 + offset
    m_AttackRange = _myBoundingOFfset + _playerBoundingOffset + 0.6f;

    // 공격력
    m_AttackPower = 10000.0f;
}

void Zombie_Clicker_Move::Update(float dTime)
{
}

void Zombie_Clicker_Move::OnRender()
{
}

void Zombie_Clicker_Move::OnCollisionEnter(Collision collision)
{
    // 플레이어랑 부딪힌 경우
    if (collision.m_GameObject->GetTag() == "Player")
    {
        // 플레이어 공격 사태로 전환
        m_State |= State::eAttackPlayer;

        m_State &= ~State::ePatrol;
        m_State &= ~State::eExplore;
        m_State &= ~State::eAwakenSound;
        m_State &= ~State::eReturn;

        // 플레이어의 앞에 살짝 떨어지게(자연스럽게) 위치 조정
        Transform* _playerTf = collision.m_GameObject->m_Transform;
        SimpleMath::Vector3 _playerPos = _playerTf->m_Position;
        SimpleMath::Vector3 _newPos = _playerPos + _playerTf->m_RotationTM.Forward() * 0.7f;
        m_Transform->SetPosition(_newPos);

        // 플레이어를 바라보도록 회전
        m_Transform->LookAtYaw(collision.m_GameObject->m_Transform->m_Position);

        // 안 움직이게 고정
        m_PhysicsActor->SetFreezePosition(true, true, true);
        m_PhysicsActor->SetFreezeRotation(true, true, true);


        // 플레이어쪽 상태 변경
        PlayerController* _playerController = collision.m_GameObject->GetComponent<PlayerController>();
        CA_TRACE("[Zombie_Clicker_Move] 물어 뜯는다~");
        _playerController->m_bIsBited = true;
        _playerController->m_bShoot = false;
        _playerController->m_bIsReloading = false;
        _playerController->m_PoseMode = PoseMode::Stand;
        _playerController->m_MovingMode = MovingMode::Idle;


        // 플레이어 무기에 따른 애니메이션 세팅
        Animator* _playerAnimator = collision.m_GameObject->m_Transform->GetChildV().at(0)->GetComponent<Animator>();
        _playerAnimator->SetOverrideAnimLayer("");
        IItem::Type _currType = collision.m_GameObject->GetComponent<Inventory>()->GetCurrentEquipment()->GetType();
        switch (_currType)
        {
            case IItem::Type::eLongGun:
                _playerAnimator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_RIFLE_BITED_DIE);
                break;

            case IItem::Type::eHandGun:
                _playerAnimator->SetNoneAnimLayer(CL::ResourcePath::ANIM_LAYER_PLAYER_PISTOL_BITED_DIE);
                break;

            default:
                break;
        }
       
        collision.m_GameObject->GetComponent<PhysicsActor>()->SetFreezePosition(true, true, true);
        collision.m_GameObject->GetComponent<PhysicsActor>()->SetFreezeRotation(true, true, true);
    }
}

void Zombie_Clicker_Move::OnCollisionStay(Collision collision)
{
}

void Zombie_Clicker_Move::OnCollisionExit(Collision collision)
{
}

bool Zombie_Clicker_Move::IsDead()
{
    if (m_Health->IsDead())
    {
        m_NavMeshAgent->SetDestinationObj(nullptr);
        m_Animator->SetNoneAnimLayer("Die");

        m_State = State::eDead;

        return true;
    }

#ifdef _DEBUG
    //CA_TRACE("[EnemyMove] %s is Dead", m_pMyObject->GetObjectName().c_str());
#endif
    return false;
}

/// <summary>
/// 플레이어 공격
/// 애니메이션 재생
/// </summary>
/// <returns></returns>
bool Zombie_Clicker_Move::AttackToPlayer()
{
    m_Animator->SetNoneAnimLayer("AttackPlayer");

    return true;
}

/// <summary>
/// 플레이어에게 대미지 적용
/// 클리커의 경우 플레이어를 충돌해서 감지하면 
/// 무조건 플레이어 사망 처리 후 게임 오버
/// </summary>
/// <returns></returns>
bool Zombie_Clicker_Move::DamageToPlayer()
{
    Health* targetHealth = m_Player->GetComponent<Health>();
    if (targetHealth != nullptr)
    {
        targetHealth->Damage(
            m_AttackPower,
            SimpleMath::Vector3::Zero,
            m_Transform->m_Position - m_Player->m_Transform->m_Position,
            0.0f,			// 넉백시킬 힘
            m_pMyObject
        );

        return true;
    }
}

void Zombie_Clicker_Move::PostAttackPlayer()
{
   // 공격 상태 해제
   m_State &= ~State::eAttackPlayer;

   // 피직스 고정 해제
   m_PhysicsActor->SetFreezePosition(false, true, false);
   m_PhysicsActor->SetFreezeRotation(true, false, true);

   return;
}
