#include "pch.h"
#include "Inventory.h"
#include "SoundEvent.h"
#include "Generator.h"

Generator::Generator()
	: m_bGeneratorPowerOn(false)
{
}

Generator::~Generator()
{
}

void Generator::Start()
{
	CInteraction::Start();
	m_pMyObject->AddComponent<Audio>(new Audio());
	m_EgenSound =  m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/AMB & BGM/Amb_Gen", "Amb_Gen_ONOFF", 0, false);

	m_bGeneratorPowerOn = false;
}

void Generator::Update(float dTime)
{
	// �׽�Ʈ�� �������� ����Ʈ On/Off ��Ű��
	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_N)))
	{
		TurnOnGenRoomLight();
		CA_TRACE("[Generator] ���� ����Ʈ On");
	}

	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_M)))
	{
		TurnOffGenRoomLight();
		CA_TRACE("[Generator] ���� ����Ʈ Off");
	}
	
	if (m_bGeneratorPowerOn)
	{
		m_EgenSound->SetPaused(false);
		m_EgenSound->SetParameter("Amb_Gen_ONOFF", 1);
	}

}

void Generator::OnRender()
{
}

/// <summary>
/// �÷��̾ ������ ���� ���¿��� �����⸦ ���۽�Ű�� ���� �Ҵ�.
/// </summary>
void Generator::Run()
{
	// �����Ⱑ ���� �����Ǳ� ���� ��
	if (m_bGeneratorPowerOn == false)
	{
		// �÷��̾ ������ ������ �ִ��� Ȯ��
		if (m_PlayerInventory->GetItemCount(IItem::Type::eFile) <= 0)
		{
			CA_TRACE("[Generator] ������ ����..");

			return;
		}

		// ������ ����Ʈ On
		TurnOnGenRoomLight();

		// ������ ������ ���� ��ȯ
		m_bGeneratorPowerOn = true; 
		m_EgenSound->SetPaused(true);
		m_pMyObject->GetComponent<Audio>()->PlayEvent("event:/SFX/SFX_GenON");
	}

	CA_TRACE("[Generator] already Power On...");
}


/// <summary>
/// ������ ������ �����⸦ ������ �� ų ����Ʈ��
/// </summary>
void Generator::TurnOnGenRoomLight()
{
	GameObject* _LightObj = nullptr;
	Light* _LightComp = nullptr;

	// Light_01_Light
	_LightObj = DLLEngine::FindGameObjectByName("Light_01_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.823f, 0.466f));
	_LightComp->SetIntensity(7.0f);
	_LightComp->SetFalloffStart(3.1f);
	_LightComp->SetFalloffEnd(6.2f);


	// Light_02_Light
	_LightObj = DLLEngine::FindGameObjectByName("Light_02_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.823f, 0.466f));
	_LightComp->SetIntensity(7.0f);
	_LightComp->SetFalloffStart(3.1f);
	_LightComp->SetFalloffEnd(6.2f);

	// Light_03_Light
	_LightObj = DLLEngine::FindGameObjectByName("Light_03_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.823f, 0.466f));
	_LightComp->SetIntensity(7.0f);
	_LightComp->SetFalloffStart(3.1f);
	_LightComp->SetFalloffEnd(6.2f);

	// Light_Strg_Light
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Strg_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.823f, 0.466f));
	_LightComp->SetIntensity(5.0f);
	_LightComp->SetFalloffStart(1.5f);
	_LightComp->SetFalloffEnd(6.1f);


	// Light_Strg_Light_01
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_01");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(true);
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(0.941f, 1.0f, 1.0f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(1.0f);
	_LightComp->SetFalloffEnd(2.0f);


	// Light_Strg_Light_02
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_02");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(true);
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(0.941f, 1.0f, 1.0f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(1.0f);
	_LightComp->SetFalloffEnd(2.0f);


	// Light_Strg_Light_03
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_03");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(true);
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(0.941f, 1.0f, 1.0f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(1.0f);
	_LightComp->SetFalloffEnd(2.0f);


	// Light_Strg_Light_04
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_04");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(true);
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(0.941f, 1.0f, 1.0f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(1.0f);
	_LightComp->SetFalloffEnd(2.0f);


	// Light_Strg_Light_05
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_05");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(true);
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(0.941f, 1.0f, 1.0f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(1.0f);
	_LightComp->SetFalloffEnd(2.0f);


	// Light_Strg_Light_06
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_06");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(true);
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(0.941f, 1.0f, 1.0f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(1.0f);
	_LightComp->SetFalloffEnd(2.0f);

	CA_TRACE("[Generator] Light On !!");
}

/// <summary>
/// ������ �Ŀ� Off ��ų �� ��(����)�� ����Ʈ
/// </summary>
void Generator::TurnOffGenRoomLight()
{
	GameObject* _LightObj = nullptr;
	Light* _LightComp = nullptr;

	// Light_01_Light
	_LightObj = DLLEngine::FindGameObjectByName("Light_01_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.376f, 0.078f));
	_LightComp->SetIntensity(4.0f);
	_LightComp->SetFalloffStart(2.1f);
	_LightComp->SetFalloffEnd(5.5f);


	// Light_02_Light
	_LightObj = DLLEngine::FindGameObjectByName("Light_02_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.376f, 0.078f));
	_LightComp->SetIntensity(4.0f);
	_LightComp->SetFalloffStart(2.1f);
	_LightComp->SetFalloffEnd(5.5f);

	// Light_03_Light
	_LightObj = DLLEngine::FindGameObjectByName("Light_03_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.376f, 0.078f));
	_LightComp->SetIntensity(4.0f);
	_LightComp->SetFalloffStart(2.1f);
	_LightComp->SetFalloffEnd(5.5f);

	// Light_Strg_Light
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Strg_Light");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetColor(DirectX::SimpleMath::Vector3(1.0f, 0.376f, 0.078f));
	_LightComp->SetIntensity(3.0f);
	_LightComp->SetFalloffStart(0.0f);
	_LightComp->SetFalloffEnd(4.0f);


	// Light_Strg_Light_01
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_01");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(false);


	// Light_Strg_Light_02
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_02");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(false);


	// Light_Strg_Light_03
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_03");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(false);


	// Light_Strg_Light_04
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_04");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(false);


	// Light_Strg_Light_05
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_05");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(false);


	// Light_Strg_Light_06
	_LightObj = DLLEngine::FindGameObjectByName("Gen_Ctrl_Light_06");
	_LightComp = _LightObj->GetComponent<Light>();
	_LightComp->SetEnable(false);

	CA_TRACE("[Generator] Light Off !!");
}