#pragma once

/// <summary>
/// �÷��̾ ��Ʈ�� �ϴ� Ŭ����
/// 
/// �⺻���� Ű �Է����� �̵��� �ϰ�
/// ���콺 �̵����� ���� �̵�(ȸ��)�� �Ѵ�.
/// 
/// �÷��̾�� ����, �ɱ� ���� ���¿�
/// �ȱ�, �޸��� ��� ���� �ִ�.
/// 
/// �ۼ��� : �� �� ȯ
/// 
/// </summary>

#include "ComponentBase.h"
struct PlayerController_Save;

enum class PoseMode
{
	Stand,		// ��   ����
	Crouch,		// ���� ����
	Max,
};

enum class MovingMode
{
	Idle,		// idle
	Walking,	// �ȱ�   ���
	Sprint,		// �޸��� ���
	Max,
};
class Audio;
class PlayerController : public ComponentBase
{
friend class ObjectBuilder;

public:
	/// ���� �Է� ����
	// Axis ����
	// �ִϸ����Ϳ����� ������ ����.(�ϴ� ������ ���ϰ� �θ������� public���� ����...)
	float m_HForAnim;			// Horizontal Axis	�ִϸ��̼� ��ȯ�� ���
	float m_VForAnim;			// Vertical Axis	
	float m_H;					// Horizontal Axis	������ �̵��� ���(�ִϸ����Ϳ��� idle ���� �����ؼ� ��뵵 ��)
	float m_V;					// Vertical Axis

	bool m_bCrouch;				// �ɱ�
	bool m_bSprint;				// �޸���
	bool m_bAim;				// ����(����)
	bool m_bShoot;				// ��� ��

	// �÷��̾� �ִϸ��̼� Ʈ�������� ����
	bool m_bIsReloading;		// ���� ���� �����ΰ� �Ǵ�
	bool m_bIsThrowing;			// ��ô ���� �����ΰ� �Ǵ�
	bool m_bIsHealing;			// ��(�ֻ�) ���� �����ΰ� �Ǵ�
	bool m_bIsAssassinate;		// �ϻ����� �����ΰ� �Ǵ�
	bool m_bCanAssassinate;		// �ϻ� ���� �����ΰ� �Ǵ�
	bool m_bIsRoutingItem;		// ������ �������� �����ΰ� �Ǵ�
	bool m_bCanRoutingItem;		// ������ ���� ���� �Ѱ� �Ǵ�
	bool m_bCanInteraction;		// ��ȣ�ۿ� �����ΰ� �Ǵ�
	bool m_bIsHeal;				// ȸ�� �����ΰ� �Ǵ�


	bool  m_bIsSwaping;			// ��� ��ü ���� �����ΰ� �Ǵ�

	float m_PitchValue;			// �÷��̾� �Ž��� ��ü ������ ���ϱ� ���� ��(�ѱ�, ���� ����� ����)

	bool m_bIsDie;				// �÷��̾� ��� ����(�������� �ִ� ����)
	bool m_bIsDead;				// ������ ���� �ִ� ����
	bool m_bIsBited;			// Ŭ��Ŀ���� ���� ���� ����

	// �÷��̾� ���� ���
	PoseMode m_PoseMode;
	MovingMode m_MovingMode;

	// �ִϸ��̼� �̺�Ʈ �Լ�
	void PostDie();
	void PostAssassinate();
	bool IsDead();


public:
	PlayerController();
	virtual ~PlayerController();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;


	// ���� �Է� ������Ʈ
	void UpdateUserInput();

	// ���� ���� ������Ʈ
	void UpdateAlive();

	// WASD �Է��� ���� �̵�
	void Move();
	void MoveByTransform();
	void MoveByPhysX();

	// �ڼ��� �����ϰ� ����� �ڼ��� ����
	PoseMode SetPoseMode(PoseMode pose);

	// ������ ���
	bool UseEquipedItem() const;

	// �ֹ��� (1��) ����
	bool PostHeal();

	// ���ü ��Ÿ�� ��ٿ�
	void SwapCoolDown();

	// ����
	void Reload(std::shared_ptr<class Gun> gun = nullptr) const;

	// ȸ��
	void Heal();

	// FŰ�Է�(�ϻ�, ����, ��ȣ�ۿ�)
	bool FFunction();
	bool CanAssassinate();
	bool Assassinate();
	bool CanItemRouting();
	bool RoutingItem();
	bool CanInteraction();
	bool Interaction();

	// Getter
	float GetStandSpeed() const;
	float GetCrouchSpeed() const;
	float GetSprintSpeed() const;

	Animator* GetAnimator() const;

	class Enemy_Move* GetAssassinateTarget() const;

private:
	// ���� �����Ѱ�?
	bool CanAim() const;

	// ��� ���� �����Ѱ�?
	bool CanSwap() const;
	bool Swap();

	// �ƹ� �׼�(����)�� ���ϰ� �ִ°�?
	bool DoNothing() const;

	// ���� ��ü(����)�� ���� �޽� ����
	void ChangeWeaponMesh();

	void DebugRender();			// ��������� �� ������ ������ �����Ѵ�.


private:
	class Health* m_Health;			// Health Component
	class Inventory* m_Inventory;	// Inventory
	class PhysicsActor* m_MyPhysicsActor;

	class GameObject* m_PlayerMesh;
	class Animator* m_Animator;
	class MeshFilter* m_Meshfilter;

	class EquipmentController* m_EquipmentController;
	class Audio* m_pAudio;
	

	// �ӵ�
	float m_StandSpeed;				// ���ִ� ������ �̵��ӵ�
	float m_CrouchSpeed;			// �ɾ� ���� �� �̵� �ӵ�
	float m_SprintSpeed;			// �޸��� �ӵ�
	float m_MoveSpeed;				// ���� �̵��ӵ�
	float m_SwapSpeed;				// ��� ��ü �ӵ�
	float m_SwapCoolTimer;			// ��� ��ü Ÿ�̸�

	Transform* m_CameraParent;		// ī�޶� �θ�(�ǹ�)

	int m_SwapInput;				// ��ü �Է� ���� Ű
	int m_NextSlotNum;				// ��ü�� ���� ����

	bool m_FInput;					// FŰ �Է� ���� Ű

	class Enemy_Move* m_AssassinateTarget;			// �ϻ� Ÿ�� ����
public:
	class CInteraction* m_InteractionObj;			// ��ȣ�ۿ� ������ ������Ʈ

public:
	GameObject* m_RoutingItem;						// ���� ������ ������
	virtual void SaveData();
	virtual void LoadData();
	virtual void LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component);

	PlayerController_Save* m_SaveData;

};

struct PlayerController_Save
{
	bool			m_bEnable;
	unsigned int	m_ComponentId;

	float m_HForAnim;
	float m_VForAnim;
	float m_H;
	float m_V;
	bool  m_bCrouch;
	bool  m_bSprint;
	bool  m_bAim;

	uint  m_Animator;
	int   m_PoseMode;
	int	  m_MovingMode;

	float m_StandSpeed;				// ���ִ� ������ �̵��ӵ�
	float m_CrouchSpeed;			// �ɾ� ���� �� �̵� �ӵ�
	float m_SprintSpeed;			// �޸��� �ӵ�
	float m_MoveSpeed;				// ���� �̵��ӵ�

	uint m_CameraParent;
	uint m_LastHitTarget;

};

BOOST_DESCRIBE_STRUCT(PlayerController_Save, (), (
	m_bEnable,
	m_ComponentId,

	m_HForAnim,
	m_VForAnim,
	m_H,
	m_V,

	m_bCrouch,
	m_bSprint,
	m_bAim,
	
	m_Animator,
	m_PoseMode,
	m_MovingMode,
	
	m_StandSpeed,			// ���ִ� ������ �̵��ӵ�
	m_CrouchSpeed,			// �ɾ� ���� �� �̵� �ӵ�
	m_SprintSpeed,			// �޸��� �ӵ�
	m_MoveSpeed,			// ���� �̵��ӵ�
	
	m_CameraParent,
	m_LastHitTarget
	))
