#include "pch.h"
#include "PlayerController.h"
#include "CameraBehavior.h"

CameraBehavior::CameraBehavior()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerController(nullptr)

	, m_ZoomInPosition(nullptr)
	, m_Camera(nullptr)

	, m_ZoomInFov(0.0f)
	, m_ZoomOutFov(0.0f)
	, m_CurrentFov(0.0f)

	, m_CameraSpeed(7.0f)

	, m_CurrentZoomInPos(Vector3::Zero)
	, m_CurrentCameraDefaultPos(Vector3::Zero)

	, m_StandZoomInPos(nullptr)
	, m_StandCameraDefaultPos(Vector3::Zero)

	, m_CrouchZoomInPos(Vector3::Zero)
	, m_CrouchCameraDefaultPos(Vector3::Zero)

	, m_CameraCollisionPivot(nullptr)

	, m_SaveData(new CameraBehavior_Save())
{
}

CameraBehavior::~CameraBehavior()
{
}

void CameraBehavior::Start()
{
	// 플레이어 컨트롤러 연결
	GameObject* _player = DLLEngine::FindGameObjectByName("Player");
	if (_player != nullptr)
	{
		m_PlayerController = _player->GetComponent<PlayerController>();
	}
	assert(m_PlayerController != nullptr);

	// 메인 카메라 컴포넌트 연결
	GameObject* mainCameraObj = DLLEngine::FindGameObjectByName("MainCamera");
	assert(mainCameraObj != nullptr);

	if (m_ZoomInPosition == nullptr)
	{
		// ZoomInPosition 할당
		GameObject* _ZoomInPosObj = DLLEngine::FindGameObjectByName("ZoomInPosition");
		assert(_ZoomInPosObj != nullptr);

		m_ZoomInPosition = _ZoomInPosObj->m_Transform;

		m_StandZoomInPos = &m_ZoomInPosition->m_Position;
	}

	// 줌인/ 줌아웃 포지션 세팅
	SetZoomInPosition();
	m_StandCameraDefaultPos = m_Transform->GetLocalPosition();

	m_CrouchCameraDefaultPos = m_StandCameraDefaultPos;
	m_CrouchCameraDefaultPos += { 0.0f, -0.3f, 0.0f };

	m_Camera = mainCameraObj->GetComponent<Camera>();

	m_ZoomOutFov = m_Camera->GetFovY();
	m_ZoomInFov = m_ZoomOutFov - 0.1f;
	m_CurrentFov = m_ZoomOutFov;

	// 초기 줌인/ 디폴트 포지션 세팅
	m_CurrentZoomInPos = *m_StandZoomInPos;
	m_CurrentCameraDefaultPos = m_StandCameraDefaultPos;

	m_CameraCollisionPivot = DLLEngine::FindGameObjectByName("CameraCollisionPivot");
	assert(m_CameraCollisionPivot != nullptr);

}

void CameraBehavior::Update(float dTime)
{
	// 플레이어 자세(Stand or Crouch) 체크
	if (m_PlayerController->m_bCrouch)
	{
		m_CurrentZoomInPos = m_CrouchZoomInPos;
		m_CurrentCameraDefaultPos = m_CrouchCameraDefaultPos;
	}
	else
	{
		m_CurrentZoomInPos = *m_StandZoomInPos;
		m_CurrentCameraDefaultPos = m_StandCameraDefaultPos;
	}

	// 플레이어가 조준중이면
	if (m_PlayerController->m_bAim)
	{
		// 카메라 줌인
		CameraZoomIn();
		//CA_TRACE("Zoom In");
	}

	// 비 조준
	else
	{
		CameraZoomOut();
	}
}

void CameraBehavior::OnRender()
{
}

void CameraBehavior::SetZoomInPosition()
{
	// 카메라 줌 인/아웃 변수	
	// 스탠드 줌인/ 디폴트 포지션 세팅
	//m_StandZoomInPos = &m_ZoomInPosition->m_Position; // -> 스탠드 줌인포지션은 포인터로 ZoomInPosition 오브젝트를 가리키므로 매번 갱신할 필요 없음

	// 크라우치 줌인/ 디폴트 포지션 세팅
	m_CrouchZoomInPos = *m_StandZoomInPos;
	m_CrouchZoomInPos += { 0.0f, -0.2f, 0.0f };


}

void CameraBehavior::CameraZoomIn()
{
	// 카메라 포지션을 Zoom In Position 으로 보간
	// 충돌중이지 않을 때만
	if (CameraCollision() == false)
	{
		m_Transform->m_Position = Vector3::Lerp(m_Transform->m_Position, m_CurrentZoomInPos, CL::Input::s_DeltaTime * m_CameraSpeed);
	}

	if (m_CurrentFov > m_ZoomInFov)
	{
		m_CurrentFov -= CL::Input::s_DeltaTime;
	}

	m_Camera->SetFovY(m_CurrentFov);
}

void CameraBehavior::CameraZoomOut()
{
	// 디폴트 포지션으로 보간
	// 충돌중일 때는 포지션은 변화 X, Fov만 변화
	if (CameraCollision() == false)
	{
		m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), m_CurrentCameraDefaultPos, CL::Input::s_DeltaTime * m_CameraSpeed);
		//CA_TRACE("줌아웃");
	}

	if (m_CurrentFov < m_ZoomOutFov)
	{
		m_CurrentFov += CL::Input::s_DeltaTime;
	}

	m_Camera->SetFovY(m_CurrentFov);
}

bool CameraBehavior::CameraCollision()
{
	// 충돌 여부
	bool _bCollision = false;

	// 부모(카메라 페어런트) 트랜스폼
	Transform* _cameraParent = m_Transform->GetParentTM();

	/// 카메라 충돌(Bounding Camera)
	GameObject* _player = m_PlayerController->GetMyObject();

	// 플레이어의 위치에서 살짝 뒤로 _ori 포지션을 잡음
	SimpleMath::Vector3& _ori = _player->m_Transform->m_WorldPosition;
	_ori += -_player->m_Transform->m_RotationTM.Forward() * 0.2f;
	//_ori.y += 0.2f;

	// 카메라 충돌 피벗 포지션
	SimpleMath::Vector3& _ori2 = m_CameraCollisionPivot->m_Transform->m_WorldPosition;

	// _ori 에서 카메라(this)를 향한 방향을 만든다.
	SimpleMath::Vector3 _dirOriToCamera = { m_Transform->m_WorldPosition - _ori };
	_dirOriToCamera.Normalize();

	// _ori2 에서 플레이어를 향한 방향을 만든다.
	SimpleMath::Vector3 _dirCameraToPlayer = { _player->m_Transform->m_WorldPosition - _ori2 };
	_dirCameraToPlayer.Normalize();

	// 플레이어 -> 카메라 충돌 피벗 까지의 거리
	float _distPlayerToCameraCollisionPivot = SimpleMath::Vector3::Distance(_player->m_Transform->m_WorldPosition, m_CameraCollisionPivot->m_Transform->m_WorldPosition);
	
	// _ori -> CameraParent 까지의 거리
	float _distOriToCameraParent = SimpleMath::Vector3::Distance(_ori, _cameraParent->m_WorldPosition);


	GameObject* _colliedObject = nullptr;

	// _ori -> 카메라 방향으로 레이캐스트 체크
	//RaycastHit _hitOriToCamera;
	//_hitOriToCamera = DLLEngine::CheckRaycastHit(_ori, _dirOriToCamera, _distOriToCameraParent);
	//
	//if (_hitOriToCamera.actor != nullptr)
	//{
	//	if (_hitOriToCamera.actor->GetMyObject()->GetTag() == "Wall")
	//	{
	//		_bCollision = true;
	//		_colliedObject = _hitOriToCamera.actor->GetMyObject();
	//
	//		CA_TRACE("카메라 충돌 1 %s", _colliedObject->GetObjectName().c_str());
	//	}
	//}
	
	if (_bCollision == false)
	{
		// 카메라 위치에서 플레이어 방향으로 레이캐스트 체크
		RaycastHit _hitOri2ToCamera;
		_hitOri2ToCamera = DLLEngine::CheckRaycastHit(_ori2, _dirCameraToPlayer, _distPlayerToCameraCollisionPivot);
		if (_hitOri2ToCamera.actor != nullptr)
		{
			if (_hitOri2ToCamera.actor->GetMyObject()->GetTag() == "Wall")
			{
				float _distPlayerToHitPos = SimpleMath::Vector3::Distance(_player->m_Transform->m_WorldPosition, { _hitOri2ToCamera.position.x, _player->m_Transform->m_WorldPosition.y, _hitOri2ToCamera.position.z });
				CA_TRACE("dist = %f", _distPlayerToHitPos);

				//if (_distPlayerToHitPos < 0.9f)
				{
					_bCollision = true;
					_colliedObject = _hitOri2ToCamera.actor->GetMyObject();
				}
				//CA_TRACE("카메라 충돌 2 %s", _colliedObject->GetObjectName().c_str());
			}
		}
	}

	// 충돌
	if (_bCollision == true)
	{
		
		float _distToCameraToCameraParent = Vector3::Distance(m_Transform->m_WorldPosition, _cameraParent->m_WorldPosition);
		//CA_TRACE("dist %f", _distToCameraToCameraParent);
		if (_distToCameraToCameraParent < 1.2f)
		{
			//m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), pos, CL::Input::s_DeltaTime * m_CameraSpeed);
			//m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), m_CurrentZoomInPos, CL::Input::s_DeltaTime * m_CameraSpeed);

			m_Transform->m_Position += m_Transform->m_RotationTM.Forward() * CL::Input::s_DeltaTime * m_CameraSpeed;
		}

		return true;
	}

	return false;
}

void CameraBehavior::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_ZoomInPosition = m_ZoomInPosition->GetComponetId();

	m_SaveData->m_ZoomInFov = m_ZoomInFov;
	m_SaveData->m_ZoomOutFov = m_ZoomOutFov;
	m_SaveData->m_CurrentFov = m_CurrentFov;

	m_SaveData->m_CameraSpeed = m_CameraSpeed;

	m_SaveData->m_CurrentZoomInPos = &m_CurrentZoomInPos;
	m_SaveData->m_CurrentCameraDefaultPos = &m_CurrentCameraDefaultPos;

	//m_SaveData->m_StandZoomInPos = m_StandZoomInPos;
	m_SaveData->m_StandCameraDefaultPos = &m_StandCameraDefaultPos;

	m_SaveData->m_CrouchZoomInPos = &m_CrouchZoomInPos;
	m_SaveData->m_CrouchCameraDefaultPos = &m_CrouchCameraDefaultPos;

	m_SaveData->m_pObject = m_pMyObject->GetGameObjectId();
	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void CameraBehavior::LoadData()
{
	m_SaveData->m_CurrentZoomInPos = &m_CurrentZoomInPos;
	m_SaveData->m_CurrentCameraDefaultPos = &m_CurrentCameraDefaultPos;

	//m_SaveData->m_StandZoomInPos = m_StandZoomInPos;
	m_SaveData->m_StandCameraDefaultPos = &m_StandCameraDefaultPos;

	m_SaveData->m_CrouchZoomInPos = &m_CrouchZoomInPos;
	m_SaveData->m_CrouchCameraDefaultPos = &m_CrouchCameraDefaultPos;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_ZoomInFov = m_SaveData->m_ZoomInFov;
	m_ZoomOutFov = m_SaveData->m_ZoomOutFov;
	m_CurrentFov = m_SaveData->m_CurrentFov;

	m_CameraSpeed = m_SaveData->m_CameraSpeed;

}

void CameraBehavior::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (m_SaveData->m_ZoomInPosition != NULL)
	{
		m_ZoomInPosition = dynamic_cast<Transform*>(component->at(m_SaveData->m_ZoomInPosition));
		m_StandZoomInPos = &m_ZoomInPosition->m_Position;

	}
}
