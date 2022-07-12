#include "pch.h"
#include "BehaviorTree_Base.h"
#include "Zombie_Clicker_Move.h"
#include "Zombie_Clicker_AI.h"

Zombie_Clicker_AI::Zombie_Clicker_AI()
	: Enemy_AI()
	, m_Root(nullptr)
	, m_Selector(nullptr)
	, m_SeqDead(nullptr)

	, m_IsDead(nullptr)

	, m_IsAssassinated(nullptr)

	, m_IsReturn(nullptr)
	, m_SeqReturn(nullptr)
	, m_MoveToReturnPoint(nullptr)
	, m_FinishReturn(nullptr)

	, m_Explore(nullptr)
{
	CA_TRACE("[Zombie_Clicker_AI] Create Zombie_Clicker_AI (Zombie_Clicker_AI())");
}

Zombie_Clicker_AI::~Zombie_Clicker_AI()
{
}

void Zombie_Clicker_AI::Start()
{
	CA_TRACE("[Zombie_Clicker_AI] Start()");

	m_Enemy = m_pMyObject->GetComponent<Zombie_Clicker_Move>();

	m_Root = std::make_unique<BehaviorTree::Sequence>();
	m_Selector = std::make_unique<BehaviorTree::Selector>();

	m_SeqDead = std::make_unique<BehaviorTree::Sequence>();
	m_IsDead = std::make_unique<BehaviorTree::Enemy::IsDead>(m_Enemy);

	m_IsAssassinated = std::make_unique<BehaviorTree::Enemy::IsAssassinated>(m_Enemy);

	m_IsReturn = std::make_unique<BehaviorTree::Enemy::IsReturn>(m_Enemy);
	m_SeqReturn = std::make_unique<BehaviorTree::Sequence>();
	m_MoveToReturnPoint = std::make_unique<BehaviorTree::Enemy::MoveToReturnPoint>(m_Enemy);
	m_FinishReturn = std::make_unique<BehaviorTree::Enemy::FinishReturn>(m_Enemy);

	m_Explore = std::make_unique<BehaviorTree::Enemy::Explore>(m_Enemy);
	m_AwakenSound = std::make_unique<BehaviorTree::Enemy::AwakenSound>(m_Enemy);

	m_IsAttackPlayer = std::make_unique<BehaviorTree::Enemy::IsAttackPlayer>(m_Enemy);
	m_AttackToPlayer = std::make_unique<BehaviorTree::Enemy::AttackToPlayer>(m_Enemy);

	m_IsPatrol = std::make_unique<BehaviorTree::Enemy::IsPartol>(m_Enemy);
	m_SeqPatrol = std::make_unique<BehaviorTree::Sequence>();
	m_MoveForTarget = std::make_unique<BehaviorTree::Enemy::MoveForTargetWayPoint>(m_Enemy);
	m_UpdateTarget = std::make_unique<BehaviorTree::Enemy::UpdateTargetWayPoint>(m_Enemy);

	m_IsWait = std::make_unique<BehaviorTree::Enemy::IsWait>(m_Enemy);
	m_Wait = std::make_unique<BehaviorTree::Enemy::Wait>(m_Enemy);

	Initialize();
}

void Zombie_Clicker_AI::Update(float dTime)
{
	m_Root->Invoke();
}

void Zombie_Clicker_AI::OnRender()
{
}

void Zombie_Clicker_AI::Initialize()
{
	m_Enemy = m_pMyObject->GetComponent<Zombie_Clicker_Move>();

	// Root Node
	m_Root->AddChild(m_Selector.get());

	// Dead
	m_Selector->AddChild(m_SeqDead.get());
	{
		m_SeqDead->AddChild(m_IsDead.get());
	}

	// Assassinated
	m_Selector->AddChild(m_IsAssassinated.get());
	{

	}

	// Return
	m_Selector->AddChild(m_IsReturn.get());
	{
		// SeqReturn
		m_IsReturn->SetChild(m_SeqReturn.get());
		{
			// MoveToReturnPoint
			m_SeqReturn->AddChild(m_MoveToReturnPoint.get());

			// FinishReturn
			m_SeqReturn->AddChild(m_FinishReturn.get());

		}
	}

	// Explore
	m_Selector->AddChild(m_Explore.get());

	// AwakenSound
	m_Selector->AddChild(m_AwakenSound.get());

	// Attack
	m_Selector->AddChild(m_IsAttackPlayer.get());
	{
		m_IsAttackPlayer->SetChild(m_AttackToPlayer.get());
	}						 

	// Patrol
	m_Selector->AddChild(m_IsPatrol.get());
	{
		m_IsPatrol->SetChild(m_SeqPatrol.get());

		m_SeqPatrol->AddChild(m_MoveForTarget.get());
		m_SeqPatrol->AddChild(m_UpdateTarget.get());
	}

	// Wait
	m_Selector->AddChild(m_Wait.get());
}
