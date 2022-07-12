#pragma once
/// <summary>
/// ���ϴ� ������Ʈ Ÿ���� �޾Ƽ� 
/// �ʱ⼼��(������Ʈ �߰�, �� ����)�� ���ִ� Ŭ����
/// </summary>
#include <windows.h>
class ObjectBuilder
{
public:
	enum class ObjectType
	{
		ePlayer,
		eLeftHand,
		eRightHand,
		ePartner,
		eZombie_Runner,
		eZombie_Clicker,
		eLongGunAmmo,
		eHandGunAmmo,
		eHealPotion,
		eCardKey,
		eFile,
		eWayPoint,
		eInGameUI,
		ePauseUI		
	};

	static GameObject* MakeObject(ObjectType type, bool* isPause);

private:
	static GameObject* MakePlayer(GameObject* obj);
	static GameObject* MakeLeftHand(GameObject* obj);
	static GameObject* MakeRightHand(GameObject* obj);
	static GameObject* MakePartner(GameObject* obj);
	static GameObject* MakeZombie_Runner(GameObject* obj);
	static GameObject* MakeZombie_Clicker(GameObject* obj);
	static GameObject* MakeWayPoint(GameObject* obj);
	static GameObject* MakeLongGunAmmo(GameObject* obj);
	static GameObject* MakeHandGunAmmo(GameObject* obj);
	static GameObject* MakeCardKey(GameObject* obj);
	static GameObject* MakeFile(GameObject* obj);
	static GameObject* MakeHealPotion(GameObject* obj);
	static GameObject* MakeIngameUI(GameObject* obj, bool* isSceneRun);
	static GameObject* MakePauseUI(GameObject* obj, bool* isSceneRun);
};

