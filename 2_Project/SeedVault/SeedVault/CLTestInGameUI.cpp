#include "pch.h"
#include "CLTestInGameUI.h"
#include "ObjectBuilder.h"
#include "CameraController.h"
#include "DialogueManager.h"

CLTestInGameUI::CLTestInGameUI()
	:m_pDirectionalLight(nullptr)
{
}

CLTestInGameUI::~CLTestInGameUI()
{
}

void CLTestInGameUI::Initialize()
{
	/// Directional Light
	m_pDirectionalLight = DLLEngine::CreateObject(this);
	m_pDirectionalLight->SetObjectName("Directional Light");
	Light* _nowLight = new Light();
	m_pDirectionalLight->AddComponent<Light>(_nowLight);

	/// 카메라
	m_pMainCamera = DLLEngine::CreateObject(this);
	m_pMainCamera->m_Transform->SetPosition({ -40.0f, 50.0f, -40.0f });
	m_pMainCamera->SetObjectName("SpriteMainCamera");

	m_pMainCamera->AddComponent<Camera>(new Camera("CLTestScene2D_MainCamera"));
	DLLEngine::SetNowCamera("CLTestScene2D_MainCamera");
	m_pMainCamera->AddComponent<CameraController>(new CameraController());

	/////
	//m_InGameUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::eInGameUI);
	//m_PauseUI = ObjectBuilder::MakeObject(ObjectBuilder::ObjectType::ePauseUI);

	m_DialogueManager = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	m_DialogueManager->SetObjectName("Dialogue Manager");

	m_DialogueManager->AddComponent<DialogueManager>(new DialogueManager());

	m_DialogueText = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	m_DialogueText->SetObjectName("Dialogue Text");

	Text* _DialogueText1 = new Text();
	_DialogueText1->SetSpriteTextInfo(
		L"../Data/Fonts/malgun.ttf",
		0.f, 100.f,	
		1.f, 1.f, 1.f, 1.0f,
		1000.f, 0.f, 45.f,
		eUIAxis::Center,
		eTextPoint::LeftUP);

	m_DialogueText->AddComponent<Text>(_DialogueText1);
	//_DialogueText->SetParent(_DialogueManager);

	///CSV 로더
	m_CSVLoader = DLLEngine::CreateObject(DLLEngine::GetNowScene());
	m_CSVLoader->SetObjectName("CSVLoader");

	m_CSVLoader->AddComponent<CSVLoader>(new CSVLoader);

}

void CLTestInGameUI::Update(float dTime)
{
	//m_DialogueManager->GetComponent<DialogueManager>()->m_DialogueText_UM = m_CSVLoader->GetComponent<CSVLoader>()->m_DialogueText_UM;
	
	//m_questText->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"발전기 수리");
	//m_questText_Sub->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"발전기 수리 부품을 찾는다");
	//
	//m_combetStat_Weapon->GetComponent<Text>()->PrintSpriteText((TCHAR*)L"39");


}

void CLTestInGameUI::Reset()
{
}
