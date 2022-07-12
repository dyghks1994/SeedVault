#include "pch.h"
#include "HeadLightController.h"

HeadLightController::HeadLightController()
	: ComponentBase(ComponentType::GameLogic)
	, m_HeadLightObj(nullptr)
	, m_Light(nullptr)
	, m_HeadLightParent(nullptr)
	, m_PlayerMeshFilter(nullptr)
{
}

HeadLightController::~HeadLightController()
{
}

void HeadLightController::Start()
{
	// Light ������Ʈ ����
	m_HeadLightObj = DLLEngine::FindGameObjectByName("HeadLight");
	m_Light = m_HeadLightObj->GetComponent<Light>();
	assert(m_Light != nullptr);

	// ����Ʈ �� ����
	m_Light->m_eLightType = eLIGHT_TYPE::SPOT;
	m_Light->SetColor({ 1.0f, 1.0f, 1.0f });
	m_Light->SetIntensity(0.5f);
	m_Light->SetFalloffStart(20.0f);
	m_Light->SetFalloffEnd(30.0f);
	m_Light->SetSpotPower(100.0f);
	m_Light->SetCastShadow(true);

	// �θ� Ʈ������(�Ӹ�) ��ġ ����
	m_PlayerMeshFilter = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	assert(m_PlayerMeshFilter != nullptr);
	
}

void HeadLightController::Update(float dTime)
{
	// �θ� Ʈ������(�Ӹ�) ��ġ ����
	m_PlayerMeshFilter = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_HeadLightParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMeshFilter->GetBoneDataWorldTM("Head"));

	// Ÿ�� ������Ʈ�� Ʈ������ ����d�� ���Ѵ�
	m_HeadLightParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// Ÿ���� �������� myObject�� Ʈ������ ����
	m_Transform->SetPosition(m_TargetTranslation);

	// rot�� ī�޶� ���� ������
	//m_Transform->SetRotation(m_TargetQuaternion);
	Transform* _cameraParentTf = DLLEngine::FindGameObjectByName("CameraParent")->m_Transform;
	m_Transform->SetRotationFromVec(
		{ 
			-_cameraParentTf->m_EulerAngles.x,			// x�� ���� ���ϸ� ������ ���Ʒ��� ���� �� ����Ʈ�� �����Ǵ� ������ ���ܼ� ����...
			_cameraParentTf->m_EulerAngles.y - 180.0f, // - 180 ���ϴϱ� �ݴ븦 ���� �Ǽ� ����...
			_cameraParentTf->m_EulerAngles.z 
		}
	);

	m_Transform->SetScale(m_TargetScale);
}

void HeadLightController::OnRender()
{
	
}
