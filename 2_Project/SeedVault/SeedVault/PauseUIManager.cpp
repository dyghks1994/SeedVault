#include "pch.h"
#include "PauseUIManager.h"

PauseUIManager::PauseUIManager()
	: ComponentBase(ComponentType::GameLogic)
	, m_pIsSceneRun(nullptr)
	, m_isPopUp(false)
	, m_isManual(false)
	, m_CurPopUpMessage(ePopUpMessage::None)
{
}

PauseUIManager::PauseUIManager(bool* isSceneRun)
	: ComponentBase(ComponentType::GameLogic)
	, m_pIsSceneRun(isSceneRun)
	, m_isPopUp(false)
	, m_isManual(false)
	, m_CurPopUpMessage(ePopUpMessage::None)
{
}

PauseUIManager::~PauseUIManager()
{
}

void PauseUIManager::Start()
{
	/// ------------------------------------------
	/// ������ ���
	/// ------------------------------------------
	m_pBackground = DLLEngine::FindGameObjectByName("BackGround01");
	assert(m_pBackground != nullptr);

	/// ------------------------------------------
	/// Continue ��ư
	/// ------------------------------------------
	m_pContinue = DLLEngine::FindGameObjectByName("PauseContinueButton");
	assert(m_pContinue != nullptr);

	/// ------------------------------------------
	/// Chapter ��ư
	/// ------------------------------------------
	m_pChapter = DLLEngine::FindGameObjectByName("PauseChapterButton");
	assert(m_pChapter != nullptr);

	/// ------------------------------------------
	/// Title ��ư
	/// ------------------------------------------
	m_pTitle = DLLEngine::FindGameObjectByName("PauseTitleButton");
	assert(m_pTitle != nullptr);

	/// ------------------------------------------
	/// Manual ��ư
	/// ------------------------------------------
	m_pManual = DLLEngine::FindGameObjectByName("PauseManualButton");
	assert(m_pManual != nullptr);

	/// ------------------------------------------
	/// Exit ��ư
	/// ------------------------------------------
	m_pExit = DLLEngine::FindGameObjectByName("PauseExitButton");
	assert(m_pExit != nullptr);

	/// ------------------------------------------
	/// PopUpâ
	/// ------------------------------------------
	m_pPopUp = DLLEngine::FindGameObjectByName("PopUp");
	assert(m_pPopUp != nullptr);

	/// ------------------------------------------
	/// PopUp ������
	/// ------------------------------------------
	m_pPopUpQuestions = DLLEngine::FindGameObjectByName("PopUpQuestions");
	assert(m_pPopUpQuestions != nullptr);

	/// ------------------------------------------
	/// �޴��� ��������Ʈ
	/// ------------------------------------------
	m_pManualSprite = DLLEngine::FindGameObjectByName("PopUpManual");
	assert(m_pManualSprite != nullptr);

	/// ------------------------------------------
	/// é�� 1 ��ư
	/// ------------------------------------------
	m_pChapter1 = DLLEngine::FindGameObjectByName("Chapter1");
	assert(m_pChapter1 != nullptr);

	/// ------------------------------------------
	/// é�� 2 ��ư
	/// ------------------------------------------
	m_pChapter2 = DLLEngine::FindGameObjectByName("Chapter2");
	assert(m_pChapter2 != nullptr);

	/// ------------------------------------------
	/// Yes��ư
	/// ------------------------------------------
	m_pYesButton = DLLEngine::FindGameObjectByName("PauseYes");
	assert(m_pYesButton != nullptr);

	/// ------------------------------------------
	/// No��ư
	/// ------------------------------------------
	m_pNoButton = DLLEngine::FindGameObjectByName("PauseNo");
	assert(m_pNoButton != nullptr);


	m_pContinue->GetComponent<Button>()->SetFunc([this]() {*m_pIsSceneRun = true; });

	m_pChapter->GetComponent<Button>()->SetFunc([this]() {m_isPopUp = true; m_CurPopUpMessage = ePopUpMessage::Chapter; });
	m_pTitle->GetComponent<Button>()->SetFunc([this]() {m_isPopUp = true; m_CurPopUpMessage = ePopUpMessage::Title; });
	m_pManual->GetComponent<Button>()->SetFunc([this]() {m_isManual = true; m_CurPopUpMessage = ePopUpMessage::Manual; });
	m_pExit->GetComponent<Button>()->SetFunc([this]() {m_isPopUp = true; m_CurPopUpMessage = ePopUpMessage::Exit; });
	m_pChapter1->GetComponent<Button>()->SetFunc([this]() {m_isPopUp = true; m_CurPopUpMessage = ePopUpMessage::Tutorial; });
	m_pChapter2->GetComponent<Button>()->SetFunc([this]() {m_isPopUp = true; m_CurPopUpMessage = ePopUpMessage::Generater; });

	m_pNoButton->GetComponent<Button>()->SetFunc([this]() {m_isPopUp = false; m_CurPopUpMessage = ePopUpMessage::None; });

}

void PauseUIManager::Update(float dTime)
{
	// ���� ����� PauseUI�� ����
	if (m_pIsSceneRun != nullptr)
	{
		m_pBackground->GetComponent<Sprite2D>()->SetEnable(!*m_pIsSceneRun);

		m_pContinue->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun);
		m_pChapter->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun);
		m_pTitle->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun);
		m_pManual->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun);
		m_pExit->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun);

		// ���� ������̸� �˾��� ���� �ʴ´�.
		if (*m_pIsSceneRun == true)
		{
			m_isPopUp = false;
			m_isManual = false;
			m_CurPopUpMessage = ePopUpMessage::None;
		}

		m_pPopUp->GetComponent<Sprite2D>()->SetEnable(!*m_pIsSceneRun && m_isPopUp == true && m_CurPopUpMessage != ePopUpMessage::Manual);
		// é�� ���ÿ����� ���� ��������Ʈ�� ���� �ʴ´�.
		m_pPopUpQuestions->GetComponent<Sprite2D>()->SetEnable(!*m_pIsSceneRun && m_isPopUp == true && m_CurPopUpMessage != ePopUpMessage::Manual && m_CurPopUpMessage != ePopUpMessage::Chapter);

		m_pChapter1->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun && m_isPopUp == true && m_CurPopUpMessage == ePopUpMessage::Chapter);
		m_pChapter2->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun && m_isPopUp == true && m_CurPopUpMessage == ePopUpMessage::Chapter);

		m_pYesButton->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun && m_isPopUp == true && m_CurPopUpMessage != ePopUpMessage::Manual);
		m_pNoButton->GetComponent<Button>()->SetEnable(!*m_pIsSceneRun && m_isPopUp == true && m_CurPopUpMessage != ePopUpMessage::Manual);

		m_pManualSprite->GetComponent<Sprite2D>()->SetEnable(!*m_pIsSceneRun && m_isManual == true && m_CurPopUpMessage == ePopUpMessage::Manual);
	}

	// �˾�â�� �߸� �ٸ� �޴���ư�� �ȴ�����
	if (m_isPopUp == true)
	{
		m_pContinue->GetComponent<Button>()->SetIntersectCheck(false);
		m_pChapter->GetComponent<Button>()->SetIntersectCheck(false);
		m_pTitle->GetComponent<Button>()->SetIntersectCheck(false);
		m_pManual->GetComponent<Button>()->SetIntersectCheck(false);
		m_pExit->GetComponent<Button>()->SetIntersectCheck(false);
	}
	else
	{
		m_pContinue->GetComponent<Button>()->SetIntersectCheck(true);
		m_pChapter->GetComponent<Button>()->SetIntersectCheck(true);
		m_pTitle->GetComponent<Button>()->SetIntersectCheck(true);
		m_pManual->GetComponent<Button>()->SetIntersectCheck(true);
		m_pExit->GetComponent<Button>()->SetIntersectCheck(true);
	}

	// PopUpâ ���� ��������Ʈ
	if (m_CurPopUpMessage != ePopUpMessage::None && m_CurPopUpMessage != ePopUpMessage::Chapter && m_CurPopUpMessage != ePopUpMessage::Manual)
	{
		m_pPopUpQuestions->GetComponent<Sprite2D>()->OnEnable();
		m_pPopUpQuestions->GetComponent<Sprite2D>()->SetspriteIndex((unsigned int)m_CurPopUpMessage - 1);
	}
	else
	{
		m_pPopUpQuestions->GetComponent<Sprite2D>()->OnDisable();
	}

	switch (m_CurPopUpMessage)
	{
	case ePopUpMessage::None:
		m_pYesButton->GetComponent<Button>()->SetFunc([]() {});
		break;

	case ePopUpMessage::Tutorial:
		m_pYesButton->GetComponent<Button>()->SetFunc([this]() 
			{	 
				*m_pIsSceneRun = true; 
				m_isPopUp = false; 
				m_CurPopUpMessage = ePopUpMessage::None; 
				DLLEngine::PickScene("Tutorial_SH");

			});
		break;

	case ePopUpMessage::Generater:
		m_pYesButton->GetComponent<Button>()->SetFunc([this]() 
			{	
				*m_pIsSceneRun = true;
				m_isPopUp = false; 
				m_CurPopUpMessage = ePopUpMessage::None;
				DLLEngine::PickScene("GenRoom");
			});
		break;

	case ePopUpMessage::Title:
		m_pYesButton->GetComponent<Button>()->SetFunc([this]() 
			{	
				*m_pIsSceneRun = true;
				m_isPopUp = false; 
				m_CurPopUpMessage = ePopUpMessage::None;
				DLLEngine::PickScene("CLStartScene");
			});
		break;

	case ePopUpMessage::Manual:
		
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::CLICK_RIGHT)))
		{
			m_isManual = false;
		}
		break;

	case ePopUpMessage::Exit:
		m_pYesButton->GetComponent<Button>()->SetFunc([this]() { DLLEngine::DestroySeedVault(); });
		break;

	case ePopUpMessage::Chapter:
		m_pYesButton->GetComponent<Button>()->SetFunc([]() {});
		m_pYesButton->GetComponent<Button>()->SetEnable(false);
		m_pNoButton->GetComponent<Button>()->SetEnable(false);
		break;
	}

}

void PauseUIManager::OnRender()
{
}


