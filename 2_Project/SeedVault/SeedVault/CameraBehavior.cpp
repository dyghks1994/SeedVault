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
	// �÷��̾� ��Ʈ�ѷ� ����
	GameObject* _player = DLLEngine::FindGameObjectByName("Player");
	if (_player != nullptr)
	{
		m_PlayerController = _player->GetComponent<PlayerController>();
	}
	assert(m_PlayerController != nullptr);

	// ���� ī�޶� ������Ʈ ����
	GameObject* mainCameraObj = DLLEngine::FindGameObjectByName("MainCamera");
	assert(mainCameraObj != nullptr);

	if (m_ZoomInPosition == nullptr)
	{
		// ZoomInPosition �Ҵ�
		GameObject* _ZoomInPosObj = DLLEngine::FindGameObjectByName("ZoomInPosition");
		assert(_ZoomInPosObj != nullptr);

		m_ZoomInPosition = _ZoomInPosObj->m_Transform;

		m_StandZoomInPos = &m_ZoomInPosition->m_Position;
	}

	// ����/ �ܾƿ� ������ ����
	SetZoomInPosition();
	m_StandCameraDefaultPos = m_Transform->GetLocalPosition();

	m_CrouchCameraDefaultPos = m_StandCameraDefaultPos;
	m_CrouchCameraDefaultPos += { 0.0f, -0.3f, 0.0f };

	m_Camera = mainCameraObj->GetComponent<Camera>();

	m_ZoomOutFov = m_Camera->GetFovY();
	m_ZoomInFov = m_ZoomOutFov - 0.1f;
	m_CurrentFov = m_ZoomOutFov;

	// �ʱ� ����/ ����Ʈ ������ ����
	m_CurrentZoomInPos = *m_StandZoomInPos;
	m_CurrentCameraDefaultPos = m_StandCameraDefaultPos;

	m_CameraCollisionPivot = DLLEngine::FindGameObjectByName("CameraCollisionPivot");
	assert(m_CameraCollisionPivot != nullptr);

}

void CameraBehavior::Update(float dTime)
{
	// �÷��̾� �ڼ�(Stand or Crouch) üũ
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

	// �÷��̾ �������̸�
	if (m_PlayerController->m_bAim)
	{
		// ī�޶� ����
		CameraZoomIn();
		//CA_TRACE("Zoom In");
	}

	// �� ����
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
	// ī�޶� �� ��/�ƿ� ����	
	// ���ĵ� ����/ ����Ʈ ������ ����
	//m_StandZoomInPos = &m_ZoomInPosition->m_Position; // -> ���ĵ� ������������ �����ͷ� ZoomInPosition ������Ʈ�� ����Ű�Ƿ� �Ź� ������ �ʿ� ����

	// ũ���ġ ����/ ����Ʈ ������ ����
	m_CrouchZoomInPos = *m_StandZoomInPos;
	m_CrouchZoomInPos += { 0.0f, -0.2f, 0.0f };


}

void CameraBehavior::CameraZoomIn()
{
	// ī�޶� �������� Zoom In Position ���� ����
	// �浹������ ���� ����
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
	// ����Ʈ ���������� ����
	// �浹���� ���� �������� ��ȭ X, Fov�� ��ȭ
	if (CameraCollision() == false)
	{
		m_Transform->m_Position = Vector3::Lerp(m_Transform->GetLocalPosition(), m_CurrentCameraDefaultPos, CL::Input::s_DeltaTime * m_CameraSpeed);
		//CA_TRACE("�ܾƿ�");
	}

	if (m_CurrentFov < m_ZoomOutFov)
	{
		m_CurrentFov += CL::Input::s_DeltaTime;
	}

	m_Camera->SetFovY(m_CurrentFov);
}

bool CameraBehavior::CameraCollision()
{
	// �浹 ����
	bool _bCollision = false;

	// �θ�(ī�޶� ��Ʈ) Ʈ������
	Transform* _cameraParent = m_Transform->GetParentTM();

	/// ī�޶� �浹(Bounding Camera)
	GameObject* _player = m_PlayerController->GetMyObject();

	// �÷��̾��� ��ġ���� ��¦ �ڷ� _ori �������� ����
	SimpleMath::Vector3& _ori = _player->m_Transform->m_WorldPosition;
	_ori += -_player->m_Transform->m_RotationTM.Forward() * 0.2f;
	//_ori.y += 0.2f;

	// ī�޶� �浹 �ǹ� ������
	SimpleMath::Vector3& _ori2 = m_CameraCollisionPivot->m_Transform->m_WorldPosition;

	// _ori ���� ī�޶�(this)�� ���� ������ �����.
	SimpleMath::Vector3 _dirOriToCamera = { m_Transform->m_WorldPosition - _ori };
	_dirOriToCamera.Normalize();

	// _ori2 ���� �÷��̾ ���� ������ �����.
	SimpleMath::Vector3 _dirCameraToPlayer = { _player->m_Transform->m_WorldPosition - _ori2 };
	_dirCameraToPlayer.Normalize();

	// �÷��̾� -> ī�޶� �浹 �ǹ� ������ �Ÿ�
	float _distPlayerToCameraCollisionPivot = SimpleMath::Vector3::Distance(_player->m_Transform->m_WorldPosition, m_CameraCollisionPivot->m_Transform->m_WorldPosition);
	
	// _ori -> CameraParent ������ �Ÿ�
	float _distOriToCameraParent = SimpleMath::Vector3::Distance(_ori, _cameraParent->m_WorldPosition);


	GameObject* _colliedObject = nullptr;

	// _ori -> ī�޶� �������� ����ĳ��Ʈ üũ
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
	//		CA_TRACE("ī�޶� �浹 1 %s", _colliedObject->GetObjectName().c_str());
	//	}
	//}
	
	if (_bCollision == false)
	{
		// ī�޶� ��ġ���� �÷��̾� �������� ����ĳ��Ʈ üũ
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
				//CA_TRACE("ī�޶� �浹 2 %s", _colliedObject->GetObjectName().c_str());
			}
		}
	}

	// �浹
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
