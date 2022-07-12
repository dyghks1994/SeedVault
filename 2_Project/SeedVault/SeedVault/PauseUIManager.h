#pragma once
#include "ComponentBase.h"
#include "IItem.h"

enum class ePopUpMessage
{
	None,		//�˾� ���� ���� 
	Tutorial,	
	Generater,
	Title,
	Exit,
	Manual,
	
	Chapter,
};

enum class eChapter
{
	None,
	C1,
	C2,
};

/// <summary>
///	PauseUI ���� Ŭ����
///  : UI �̹����� ���� ����[0]/����[1], 
/// 
/// 2022.05.25 B.Bunny
/// </summary>
class PauseUIManager : public ComponentBase
{
public:
	PauseUIManager();
	PauseUIManager(bool* isSceneRun);
	virtual ~PauseUIManager();

	// ComponentBase Override
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

private:

	// ��ȭ ��� �Ŵ���
	GameObject* m_pBackground;

	GameObject* m_pContinue;
	GameObject* m_pChapter;
	GameObject* m_pTitle;
	GameObject* m_pManual;
	GameObject* m_pExit;

	GameObject* m_pPopUp;
	GameObject* m_pPopUpQuestions;
	GameObject* m_pManualSprite;

	GameObject* m_pChapter1;
	GameObject* m_pChapter2;

	GameObject* m_pYesButton;
	GameObject* m_pNoButton;

	ePopUpMessage m_CurPopUpMessage;

	bool* m_pIsSceneRun;
	bool m_isPopUp;
	bool m_isManual;
};

