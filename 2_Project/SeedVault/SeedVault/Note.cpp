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
	
	// UI에서 만들어놓음
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
	CA_TRACE("[Note] 상호작용");
	
	m_Document->GetComponent<Sprite2D>()->OnEnable();
	isInterect = true;
}
