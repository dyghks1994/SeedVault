#include "pch.h"
#include "Generator.h"
#include "Lift.h"

Lift::Lift()
	: m_Generator(nullptr)
	, m_time(0)
{
}

Lift::~Lift()
{
}

void Lift::Start()
{
	CInteraction::Start();

	m_Generator = DLLEngine::FindGameObjectByName("Generator")->GetComponent<Generator>();

	m_pMyObject->AddComponent<Audio>(new Audio());

	assert(m_Generator != nullptr);

}

void Lift::Update(float dTime)
{
	if (isInterect == true)
	{
		m_time += dTime;
	}

	if (m_time > 3.0f)
	{
		DLLEngine::PickScene("CLEndScene");
	}
}

void Lift::OnRender()
{
}

/// <summary>
/// 발전기가 가동되면 리프트를 사용할 수 있다.
/// 리프트를 사용하면 게임 끝
/// </summary>
void Lift::Run()
{
	if (m_Generator->m_bGeneratorPowerOn == false)
	{
		CA_TRACE("[Lift] 리프트 가동 불가.. 발전기를 먼저 가동 시키자!");

		return;
	}

	m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_Lift", false);
	CA_TRACE("[Lift] 리프트 가동");

	isInterect = true;

}
