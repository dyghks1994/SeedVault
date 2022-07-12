#include "pch.h"
#include "CLEndScene.h"

CLEndScene::CLEndScene()
	: m_CreditTime(0)
{
}

CLEndScene::~CLEndScene()
{
}

void CLEndScene::Initialize()
{
	/// 农饭调 扑诀
	m_CreditImage = DLLEngine::CreateObject(this);
	m_CreditImage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_CreditImage->SetObjectName("Credit");


	Sprite2D* _CreditImage = new Sprite2D();
	_CreditImage->m_SpriteName_V.push_back("Credit2.png");
	_CreditImage->SetProportion(1.f, 1.f);
	_CreditImage->SetPivot(1.0f, 1.0f);
	_CreditImage->SetUIAxis(eUIAxis::Credit);

	m_CreditImage->AddComponent<Sprite2D>(_CreditImage);


	/// 农饭调 扑诀
	m_EscImage = DLLEngine::CreateObject(this);
	m_EscImage->m_Transform->SetPosition({ 0.0f, 0.0f, 0.5f });
	m_EscImage->SetObjectName("CreditEescTitle");


	Sprite2D* _EscImageImage = new Sprite2D();
	_EscImageImage->m_SpriteName_V.push_back("escTitle.png");
	_EscImageImage->SetProportion(1.f, 1.f);
	_EscImageImage->SetPivot(1.0f, 0.0f);
	_EscImageImage->SetUIAxis(eUIAxis::LeftDown);

	m_EscImage->AddComponent<Sprite2D>(_EscImageImage);
}

void CLEndScene::Update(float dTime)
{
	m_CreditTime += dTime;
	if (m_CreditTime > 2.f)
	{
		m_CreditImage->m_Transform->m_Position.y -= 1.5f;
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)))
	{
		DLLEngine::PickScene("CLStartScene");
	}

}

void CLEndScene::Reset()
{
}
