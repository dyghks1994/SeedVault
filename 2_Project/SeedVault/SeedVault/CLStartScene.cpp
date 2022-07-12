#include "pch.h"
#include "CameraController.h"
#include "CLStartScene.h"
#include <random>

CLStartScene::CLStartScene()
	: m_isPopUp(false)
	, m_CurPopupState(StartPopUp::None)
	, m_randTime(0.f)
{
	m_pAudioPlayer = DLLEngine::CreateObject(this, true);
	m_pAudioPlayer->SetObjectName("GlobalAudioPlayer");
	Audio* _pAudio = new Audio();
	m_pAudioPlayer->AddComponent<Audio>(_pAudio);
}

CLStartScene::~CLStartScene()
{
}

void CLStartScene::Initialize()
{
	m_bBGMPlaying = false;
	Audio* _pAudio = m_pAudioPlayer->GetComponent<Audio>();

	/// 바탕
	m_StartImage = DLLEngine::CreateObject(this);
	m_StartImage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_StartImage->SetObjectName("Title");

	Sprite2D* _StartImage = new Sprite2D();
	_StartImage->m_SpriteName_V.push_back("TitleBackground.png");
	_StartImage->SetProportion(1.f, 1.f);
	_StartImage->SetPivot(0.5f, 0.5f);
	_StartImage->SetUIAxis(eUIAxis::BackGround);

	m_StartImage->AddComponent<Sprite2D>(_StartImage);

	/// 바탕 Light
	m_StartLightImsage = DLLEngine::CreateObject(this);
	m_StartLightImsage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_StartLightImsage->SetObjectName("Title");

	Sprite2D* _StartLightImsage = new Sprite2D();
	_StartLightImsage->m_SpriteName_V.push_back("Title_Light_01.png");
	_StartLightImsage->SetProportion(1.f, 1.f);
	_StartLightImsage->SetPivot(0.5f, 0.5f);
	_StartLightImsage->SetUIAxis(eUIAxis::BackGround);

	m_StartLightImsage->AddComponent<Sprite2D>(_StartLightImsage);

	/// 타이틀
	m_TitleName = DLLEngine::CreateObject(this);
	m_TitleName->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_TitleName->SetObjectName("Title");

	Sprite2D* _titleNameImage = new Sprite2D();
	_titleNameImage->m_SpriteName_V.push_back("Title_Logo_FitSize.png");
	_titleNameImage->SetProportion(1.f, 1.f);
	_titleNameImage->SetPivot(0.5f, 0.5f);
	_titleNameImage->SetUIAxis(eUIAxis::BackGround);

	m_TitleName->AddComponent<Sprite2D>(_titleNameImage);

	/// --------------------------------------------------
	/// 버튼들
	/// --------------------------------------------------

	m_width = static_cast<float>(DLLWindow::GetScreenWidth());
	m_height = static_cast<float>(DLLWindow::GetScreenHeight());

	float _x = m_width / 4;
	float _y = -(m_height / 22);
	float _yGap = m_height / 14;

	/// 시작 버튼 (완)
	m_StartButton = DLLEngine::CreateObject(this);
	m_StartButton->m_Transform->SetPosition({ _x, _y, 1.0f });
	m_StartButton->SetObjectName("TitleStartButton");

	Button* _startButton = new Button();
	_startButton->m_OffSprite = "UI_Main_New_Off.png";
	_startButton->m_OnSprite = "UI_Main_New_On.png";

	_startButton->SetProportion(1.f, 1.f);
	_startButton->SetPivot(0.5f, 0.5f);
	_startButton->SetAxis(eUIAxis::Center);
	_startButton->SetFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); DLLEngine::PickScene("Tutorial_SH"); });

	_startButton->SetClickFunc([_pAudio, this]() { m_BGM->Stop();  _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_startButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	
	m_StartButton->AddComponent<Button>(_startButton);

	/// 챕터선택 버튼 
	m_ChapterButton = DLLEngine::CreateObject(this);
	m_ChapterButton->m_Transform->SetPosition({ _x, _y + _yGap, 1.0f });
	m_ChapterButton->SetObjectName("TitleChapterButton");

	Button* _chapterButton = new Button();
	_chapterButton->m_OffSprite = "UI_Main_Chapter_Off.png";
	_chapterButton->m_OnSprite = "UI_Main_Chapter_On.png";

	_chapterButton->SetProportion(1.f, 1.f);
	_chapterButton->SetPivot(0.5f, 0.5f);
	_chapterButton->SetAxis(eUIAxis::Center);
	_chapterButton->SetFunc([this]()
		{	
			m_isPopUp = true;
			m_CurPopupState = StartPopUp::Chapter;
		});
	_chapterButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_chapterButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_chapterButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	m_ChapterButton->AddComponent<Button>(_chapterButton);

	/// 조작법 버튼 (완)
	m_ManualButton = DLLEngine::CreateObject(this);
	m_ManualButton->m_Transform->SetPosition({ _x, _y + 2 * _yGap, 1.0f });
	m_ManualButton->SetObjectName("TitleManualButton");

	Button* _manualButton = new Button();
	_manualButton->m_OffSprite = "UI_Button_HtControl_Off.png";
	_manualButton->m_OnSprite = "UI_Button_HtControl_On.png";

	_manualButton->SetProportion(1.f, 1.f);
	_manualButton->SetPivot(0.5f, 0.5f);
	_manualButton->SetAxis(eUIAxis::Center);
	_manualButton->SetFunc([this]()
		{	
			m_isPopUp = true;
			m_CurPopupState = StartPopUp::Manual;
		});
	_manualButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_manualButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });

	m_ManualButton->AddComponent<Button>(_manualButton);

	/// 크레딧 버튼 (완)
	m_CreditButton = DLLEngine::CreateObject(this);
	m_CreditButton->m_Transform->SetPosition({ _x, _y + 3 * _yGap, 0.0f });
	m_CreditButton->SetObjectName("TitleExitButton");

	Button* _creditButton = new Button();
	_creditButton->m_OffSprite = "UI_Button_Credit_Off.png";
	_creditButton->m_OnSprite = "UI_Button_Credit_On.png";

	_creditButton->SetProportion(1.f, 1.f);
	_creditButton->SetPivot(0.5f, 0.5f);
	_creditButton->SetAxis(eUIAxis::Center);
	_creditButton->SetFunc([this]()
		{	
			//m_isPopUp = true;
			//m_CurPopupState = StartPopUp::Credit;
			DLLEngine::PickScene("CLEndScene");
		});
	_creditButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_creditButton->SetClickFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Click"); });

	m_CreditButton->AddComponent<Button>(_creditButton);

	/// 나가기 버튼 (완)
	m_ExitButton = DLLEngine::CreateObject(this);
	m_ExitButton->m_Transform->SetPosition({ _x, _y + 4 * _yGap, 0.0f });
	m_ExitButton->SetObjectName("TitleExitButton");

	Button* _exitButton = new Button();
	_exitButton->m_OffSprite = "UI_Main_Exit_Off.png";
	_exitButton->m_OnSprite = "UI_Main_Exit_On.png";

	_exitButton->SetProportion(1.f, 1.f);
	_exitButton->SetPivot(0.5f, 0.5f);
	_exitButton->SetAxis(eUIAxis::Center);
	_exitButton->SetFunc([]() { DLLEngine::DestroySeedVault(); });
	_exitButton->SetClickFunc([_pAudio, this]() { m_BGM->Stop(); _pAudio->PlayEvent("event:/UI_Button_Click"); });
	_exitButton->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });

	m_ExitButton->AddComponent<Button>(_exitButton);

	/// --------------------------------------------------
	/// 팝업들
	/// --------------------------------------------------

	/// 팝업 창
	m_PopUP = DLLEngine::CreateObject(this);
	m_PopUP->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_PopUP->SetObjectName("TitlePopUp");

	Sprite2D* _PopUpImage = new Sprite2D();
	_PopUpImage->m_SpriteName_V.push_back("UI_Popup_Main.png");
	_PopUpImage->SetProportion(1.f, 1.f);
	_PopUpImage->SetPivot(0.5f, 0.5f);
	_PopUpImage->SetUIAxis(eUIAxis::Center);

	m_PopUP->AddComponent<Sprite2D>(_PopUpImage);

	/// 챕터1
	m_Chapter1 = DLLEngine::CreateObject(this);
	m_Chapter1->m_Transform->SetPosition({ -100.0f, 0.0f, 0.5f });
	m_Chapter1->SetObjectName("TitleChapter1");

	Button* _Chapter1Button = new Button();
	_Chapter1Button->m_OffSprite = "UI_Chapter_Tutorial_Off.png";
	_Chapter1Button->m_OnSprite = "UI_Chapter_Tutorial_On.png";

	_Chapter1Button->SetProportion(1.f, 1.f);
	_Chapter1Button->SetPivot(0.5f, 0.5f);
	_Chapter1Button->SetAxis(eUIAxis::Center);
	_Chapter1Button->SetFunc([ this]() 
		{	
			m_isPopUp = false;
			m_CurPopupState = StartPopUp::None; 
			DLLEngine::PickScene("Tutorial_SH"); 
		});
	_Chapter1Button->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_Chapter1Button->SetClickFunc([_pAudio, this]() { m_BGM->Stop(); _pAudio->PlayEvent("event:/UI_Button_Click"); });

	m_Chapter1->AddComponent<Button>(_Chapter1Button);

	/// 챕터2
	m_Chapter2 = DLLEngine::CreateObject(this);
	m_Chapter2->m_Transform->SetPosition({ 100.0f, 0.0f, 0.5f });
	m_Chapter2->SetObjectName("TitleChapter2");

	Button* _Chapter2Button = new Button();
	_Chapter2Button->m_OffSprite = "UI_Chapter_Generator_Off.png";
	_Chapter2Button->m_OnSprite = "UI_Chapter_Generator_On.png";

	_Chapter2Button->SetProportion(1.f, 1.f);
	_Chapter2Button->SetPivot(0.5f, 0.5f);
	_Chapter2Button->SetAxis(eUIAxis::Center);
	_Chapter2Button->SetFunc([this]() 
		{	
			m_isPopUp = false;
			m_CurPopupState = StartPopUp::None;
			DLLEngine::PickScene("GenRoom");
		});
	_Chapter2Button->SetOverFunc([_pAudio]() { _pAudio->PlayEvent("event:/UI_Button_Over"); });
	_Chapter2Button->SetClickFunc([_pAudio, this]() { m_BGM->Stop(); _pAudio->PlayEvent("event:/UI_Button_Click"); });

	m_Chapter2->AddComponent<Button>(_Chapter2Button);

	/// 조작법 팝업(완)
	m_ManualImage = DLLEngine::CreateObject(this);
	m_ManualImage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_ManualImage->SetObjectName("TitleManualPopUp");

	Sprite2D* _ManualImage = new Sprite2D();
	_ManualImage->m_SpriteName_V.push_back("Keysetting.png");
	_ManualImage->SetProportion(0.7f, 0.7f);
	_ManualImage->SetPivot(0.5f, 0.5f);
	_ManualImage->SetUIAxis(eUIAxis::Center);

	m_ManualImage->AddComponent<Sprite2D>(_ManualImage);

	/// 크레딧 팝업 배경
	m_CreditBackGrond = DLLEngine::CreateObject(this);
	m_CreditBackGrond->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_CreditBackGrond->SetObjectName("TitleCreditBackGround");

	Sprite2D* _CreditBackGround = new Sprite2D();
	_CreditBackGround->m_SpriteName_V.push_back("CreditBackGround.png");
	_CreditBackGround->SetProportion(1.f, 1.f);
	_CreditBackGround->SetPivot(0.5f, 0.5f);
	_CreditBackGround->SetUIAxis(eUIAxis::BackGround);

	m_CreditBackGrond->AddComponent<Sprite2D>(_CreditBackGround);

	/// 크레딧 팝업
	m_CreditImage = DLLEngine::CreateObject(this);
	m_CreditImage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_CreditImage->SetObjectName("TitleCreditPopUp");
	

	Sprite2D* _CreditImage = new Sprite2D();
	_CreditImage->m_SpriteName_V.push_back("Credit.png");
	_CreditImage->SetProportion(1.f, 1.f);
	_CreditImage->SetPivot(1.0f, 1.0f);
	_CreditImage->SetUIAxis(eUIAxis::Credit);

	m_CreditImage->AddComponent<Sprite2D>(_CreditImage);

	/// 카메라
	m_pMainCamera = DLLEngine::CreateObject(this);
	m_pMainCamera->m_Transform->SetPosition({ -40.0f, 50.0f, -40.0f });
	m_pMainCamera->SetObjectName("SpriteMainCamera");

	m_pMainCamera->AddComponent<Camera>(new Camera("CLTestScene2D_MainCamera"));
	DLLEngine::SetNowCamera("CLTestScene2D_MainCamera");
	m_pMainCamera->AddComponent<CameraController>(new CameraController());
}

void CLStartScene::Update(float dTime)
{
	if (m_bBGMPlaying == false)
	{
		m_BGM = m_pAudioPlayer->GetComponent<Audio>()->PlayEvent("event:/AMB & BGM/BGM_Main", false);
		m_bBGMPlaying = true;
	}

	// 메인화면 라이트 깜박임
	m_time += dTime;
	bool _light = false;

	int _randNum = rand();
	if (_randNum >= RAND_MAX / 2)
	{
		_light = true;
	}
	else
	{
		_light = false;
	}

	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(0.1f, 0.75f);

	if (m_time > m_randTime || m_randTime == 0.f)
	{
		m_randTime = distr(eng);

		m_StartLightImsage->GetComponent<Sprite2D>()->SetEnable(_light);
		m_time = 0.f;
	}

	m_PopUP->GetComponent<Sprite2D>()->SetEnable(m_isPopUp && m_CurPopupState == StartPopUp::Chapter);
	m_Chapter1->GetComponent<Button>()->SetEnable(m_isPopUp && m_CurPopupState == StartPopUp::Chapter);
	m_Chapter2->GetComponent<Button>()->SetEnable(m_isPopUp && m_CurPopupState == StartPopUp::Chapter);

	m_ManualImage->GetComponent<Sprite2D>()->SetEnable(m_isPopUp && m_CurPopupState == StartPopUp::Manual);

	m_CreditBackGrond->GetComponent<Sprite2D>()->SetEnable(m_isPopUp && m_CurPopupState == StartPopUp::Credit);
	m_CreditImage->GetComponent<Sprite2D>()->SetEnable(m_isPopUp && m_CurPopupState == StartPopUp::Credit);

	switch (m_CurPopupState)
	{
	case StartPopUp::None:
		break;

	case StartPopUp::Chapter:
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)) || DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::CLICK_RIGHT)))
		{
			m_isPopUp = false;
			m_CurPopupState = StartPopUp::None;
		}
		break;

	case StartPopUp::Manual:
		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)) || DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::CLICK_RIGHT)))
		{
			m_isPopUp = false;
			m_CurPopupState = StartPopUp::None;
		}
		break;

	case StartPopUp::Credit:
		m_CreditTime += dTime;
		if (m_CreditTime > 2.f)
		{
			m_CreditImage->m_Transform->m_Position.y -= 1.f;
		}

		if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)) || DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::CLICK_RIGHT)))
		{
			m_isPopUp = false;
			m_CurPopupState = StartPopUp::None;
			m_CreditImage->m_Transform->m_Position.y = 0.f;
			m_CreditTime = 0;
		}
		break;

	}

	// 팝업창이 떴을때 다른 버튼들은 작동하지 않는다.
	if (m_isPopUp == true)
	{
		m_StartButton->GetComponent<Button>()->SetIntersectCheck(false);
		m_ChapterButton->GetComponent<Button>()->SetIntersectCheck(false);
		m_ManualButton->GetComponent<Button>()->SetIntersectCheck(false);
		m_CreditButton->GetComponent<Button>()->SetIntersectCheck(false);
		m_ExitButton->GetComponent<Button>()->SetIntersectCheck(false);
	}
	else
	{
		m_StartButton->GetComponent<Button>()->SetIntersectCheck(true);
		m_ChapterButton->GetComponent<Button>()->SetIntersectCheck(true);
		m_ManualButton->GetComponent<Button>()->SetIntersectCheck(true);
		m_CreditButton->GetComponent<Button>()->SetIntersectCheck(true);
		m_ExitButton->GetComponent<Button>()->SetIntersectCheck(true);
	}
}

void CLStartScene::Reset()
{
}
