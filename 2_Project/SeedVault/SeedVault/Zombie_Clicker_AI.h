#pragma once

/// <summary>
/// Zombie_Clicker_AI �� �����ϴ� ��� Ŭ������
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>

#include "Enemy_AI.h"
#include "BehaviorTree_Base.h"


/// <summary>
/// Zombie_Runner �� A.I �� �����ϴ� Ŭ����
/// 
/// BehaviorTree ����� ��带 �����ؼ� A.I�� �����Ѵ�.
/// 
/// Zombie_Runner �� �þ�(��)�� ������ �ְ� �÷��̾(�뺴)���Դ� �ֵθ��� ������
/// �������Դ� ������ ������ �Ѵ�
/// 
/// �ۼ��� : �� �� ȯ
/// </summary>
class Zombie_Clicker_AI : public Enemy_AI
{
public:
	Zombie_Clicker_AI();
	virtual ~Zombie_Clicker_AI();

	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	void Initialize();

private:
	class std::unique_ptr<BehaviorTree::Sequence> m_Root;
	class std::unique_ptr<BehaviorTree::Selector> m_Selector;
	//class BehaviorTree::Sequence* m_SeqMovingAttack;

	class std::unique_ptr<BehaviorTree::Sequence> m_SeqDead;
	class std::unique_ptr<BehaviorTree::Enemy::IsDead> m_IsDead;

	class std::unique_ptr<BehaviorTree::Enemy::IsAssassinated> m_IsAssassinated;

	class std::unique_ptr<BehaviorTree::Enemy::IsReturn> m_IsReturn;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqReturn;
	class std::unique_ptr<BehaviorTree::Enemy::MoveToReturnPoint> m_MoveToReturnPoint;
	class std::unique_ptr<BehaviorTree::Enemy::FinishReturn> m_FinishReturn;

	class std::unique_ptr<BehaviorTree::Enemy::Explore> m_Explore;
	class std::unique_ptr<BehaviorTree::Enemy::AwakenSound> m_AwakenSound;

	class std::unique_ptr<BehaviorTree::Enemy::IsAttackPlayer> m_IsAttackPlayer;
	class std::unique_ptr<BehaviorTree::Enemy::AttackToPlayer> m_AttackToPlayer;

	class std::unique_ptr<BehaviorTree::Enemy::IsPartol> m_IsPatrol;
	class std::unique_ptr<BehaviorTree::Sequence> m_SeqPatrol;
	class std::unique_ptr<BehaviorTree::Enemy::UpdateTargetWayPoint> m_UpdateTarget;
	class std::unique_ptr<BehaviorTree::Enemy::MoveForTargetWayPoint> m_MoveForTarget;


	class std::unique_ptr<BehaviorTree::Enemy::IsWait> m_IsWait;
	class std::unique_ptr<BehaviorTree::Enemy::Wait> m_Wait;
};

