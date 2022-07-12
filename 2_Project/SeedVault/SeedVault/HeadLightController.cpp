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
	// Light 컴포넌트 연결
	m_HeadLightObj = DLLEngine::FindGameObjectByName("HeadLight");
	m_Light = m_HeadLightObj->GetComponent<Light>();
	assert(m_Light != nullptr);

	// 라이트 값 세팅
	m_Light->m_eLightType = eLIGHT_TYPE::SPOT;
	m_Light->SetColor({ 1.0f, 1.0f, 1.0f });
	m_Light->SetIntensity(0.5f);
	m_Light->SetFalloffStart(20.0f);
	m_Light->SetFalloffEnd(30.0f);
	m_Light->SetSpotPower(100.0f);
	m_Light->SetCastShadow(true);

	// 부모 트랜스폼(머리) 위치 연결
	m_PlayerMeshFilter = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	assert(m_PlayerMeshFilter != nullptr);
	
}

void HeadLightController::Update(float dTime)
{
	// 부모 트랜스폼(머리) 위치 연결
	m_PlayerMeshFilter = DLLEngine::FindGameObjectByName("Player_Mesh")->GetComponent<MeshFilter>();
	m_HeadLightParent = reinterpret_cast<SimpleMath::Matrix*>(m_PlayerMeshFilter->GetBoneDataWorldTM("Head"));

	// 타겟 오브젝트의 트랜스폼 성분d을 구한다
	m_HeadLightParent->Decompose(m_TargetScale, m_TargetQuaternion, m_TargetTranslation);

	// 타겟의 성분으로 myObject의 트랜스폼 구성
	m_Transform->SetPosition(m_TargetTranslation);

	// rot은 카메라를 따라 가도록
	//m_Transform->SetRotation(m_TargetQuaternion);
	Transform* _cameraParentTf = DLLEngine::FindGameObjectByName("CameraParent")->m_Transform;
	m_Transform->SetRotationFromVec(
		{ 
			-_cameraParentTf->m_EulerAngles.x,			// x를 반전 안하면 시점을 위아래로 돌릴 때 라이트가 반전되는 문제가 생겨서 보정...
			_cameraParentTf->m_EulerAngles.y - 180.0f, // - 180 안하니까 반대를 보게 되서 보정...
			_cameraParentTf->m_EulerAngles.z 
		}
	);

	m_Transform->SetScale(m_TargetScale);
}

void HeadLightController::OnRender()
{
	
}
