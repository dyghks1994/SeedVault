#include "pch.h"
#include "Note.h"

Note::Note()
{
}

Note::~Note()
{
}

void Note::Start()
{
	CInteraction::Start();
	
	// UI���� ��������
	m_Document = DLLEngine::FindGameObjectByName("BB_Document");
	assert(m_Document != nullptr);
}

void Note::Update(float dTime)
{
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_TAB)))
	{
		m_Document->GetComponent<Sprite2D>()->OnDisable();
		isInterect = false;
	}
}

void Note::OnRender()
{
}

void Note::Run()
{
	CA_TRACE("[Note] ��ȣ�ۿ�");
	
	m_Document->GetComponent<Sprite2D>()->OnEnable();
	isInterect = true;
}
