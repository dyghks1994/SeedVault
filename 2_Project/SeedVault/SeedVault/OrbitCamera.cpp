#include "pch.h"
#include "GameManager.h"
#include "PlayerController.h"
#include "OrbitCamera.h"


OrbitCamera::OrbitCamera()
	: ComponentBase(ComponentType::GameLogic)
	, m_PlayerController(nullptr)
	, m_FocusTarget(nullptr)
	, m_Distance(0.0f)
	, m_RotationSpeed(0.0f)
	, m_FocusPosition(Vector3::Zero)

	, m_RotationAngles(0.0f, 0.0f)
	, m_MaxVerticalAngle(0.0f)
	, m_MinVerticalAngle(0.0f)

	, m_bShootRecoil(false)
	, m_ShootReciolOffset(SimpleMath::Vector2::Zero) 
	, m_TotalTime(0.0f)
	, m_RecoilTime(0.0f)
	, m_RecoverTime(0.0f)

	, m_SaveData(new OrbitCamera_Save())
{
}

OrbitCamera::~OrbitCamera()
{
}

void OrbitCamera::Start()
{
	m_RotationSpeed = 100.0f;
	m_Distance = 1.5f;

	m_RotationAngles = { 0.0f, 0.0f };
	m_MaxVerticalAngle = 50.0f;
	m_MinVerticalAngle = -40.0f;

	// 플레이어 컨트롤러 연결
	GameObject* _player = DLLEngine::FindGameObjectByName("Player");
	if (_player != nullptr)
	{
		m_PlayerController = _player->GetComponent<PlayerController>();
		assert(m_PlayerController != nullptr);
	}
	m_RotationAngles.y = m_PlayerController->m_Transform->m_EulerAngles.y;
	m_Transform->SetRotationFromVec({0.0f, m_PlayerController->m_Transform->m_EulerAngles.y, 0.0f});

	// 타겟 지정을 안해줬을 때 임시로 아무거나 넣어줌.
	if (m_FocusTarget == nullptr)
	{
		GameObject* _target = DLLEngine::FindGameObjectByName("WayPoint_1");
		assert(_target != nullptr);

		m_FocusTarget = _target->m_Transform;
	}

	UpdateFocusPoint();

	m_ShootReciolOffset = SimpleMath::Vector2::Zero;
	m_TotalTime = 0.0f;
	m_RecoilTime = 0.0f;
	m_RecoverTime = 0.0f;
}

void OrbitCamera::Update(float dTime)
{
	// GameMager 의 s_bTPSMode 가 true 일 때만 카메라를 회전한다. 
	if (GameManager::IsTPSMode() == false)
	{
		return;
	}

	// 초점(타겟)위치 갱신
	UpdateFocusPoint();


	if (ManualRotation())
	{
		ConstrainAngles();
		m_Transform->SetRotationFromVec(m_RotationAngles);
	}

	// 총기반동
	if (m_bShootRecoil == true)
	{
		if (m_RecoilTime > 0.0f)
		{
			ShootRecoil();
		}
		else
		{
			ShootRecover();
		}
	}

	Vector3 lookDirection = m_Transform->m_RotationTM.Forward();
	Vector3 lookPosition = m_FocusPosition - lookDirection * m_Distance;
	m_Transform->m_Position = lookPosition;
}


void OrbitCamera::OnRender()
{
}

void OrbitCamera::SetFocusTrasform(Transform& targetTf)
{
	// 타겟 트랜스폼 설정
	this->m_FocusTarget = &targetTf;
}

void OrbitCamera::SetFocusTrasform(GameObject& targetObj)
{
	// 타겟 트랜스폼 설정
	this->m_FocusTarget = targetObj.m_Transform;
}

void OrbitCamera::UpdateFocusPoint()
{
	//DirectX::SimpleMath::Matrix _targetWorldTM = m_Focus->GetWorld();
	Vector3 targetPoint = m_FocusTarget->GetLocalPosition();
	//Vector3 targetPoint = Vector3::Zero;
	targetPoint += { 0.0f, 0.3f, 0.0f };   // 카메라 원점 오프셋 조절

	m_FocusPosition = targetPoint;
}

bool OrbitCamera::ManualRotation()
{
	// 마우스 이동 방향 값 확인
	Vector2 _input = { CL::Input::GetAxis("Mouse Y"), CL::Input::GetAxis("Mouse X") };

	// 마우스 이동 감지 상수
	const float _e = 0.0001f;

	// 이동이 있으면
	if (_input.x < -_e || _input.x > _e || _input.y < -_e || _input.y > _e)
	{
		// 윈도우 기준 화면 중앙 좌표
		POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

		// 먼저 현재 좌표를 구한다
		POINT _currentPos;
		GetCursorPos(&_currentPos);

		HWND _hWnd = DLLWindow::GetWindowHandle();
		ScreenToClient(_hWnd, &_currentPos);

		// 중앙 좌표를 기준으로 이동한 변위를 구함.
		int _deltaX = _currentPos.x - _centerPos.x;
		int _deltaY = _currentPos.y - _centerPos.y;
		
		// 회전각 조절
		// 마우스 입력 속도(민감도)가 빨라서 
		// 직접 조정하는 회전각도를 조절할 때 오프셋 만큼 곱해서 회전량을 조절
		m_RotationAngles +=
		{
			_deltaY* CL::KeyMap::MouseSensitivity,
				_deltaX* CL::KeyMap::MouseSensitivity
		};

		// 다시 마우스 커서를 윈도우의 중앙으로 설정
		DLLWindow::MoveCursorToCenter();


		m_PlayerController->m_PitchValue = m_RotationAngles.x + m_MaxVerticalAngle;
		float _denominator = m_MaxVerticalAngle * 2;

		m_PlayerController->m_PitchValue /= _denominator;

		if (m_PlayerController->m_PitchValue > 1.0f) m_PlayerController->m_PitchValue = 1.0f;
		if (m_PlayerController->m_PitchValue < 0.0f) m_PlayerController->m_PitchValue = 0.0f;

		//CA_TRACE("Now Pitch Value : %f", m_PlayerController->m_PitchValue);

		return true;
	}

	return false;
}

void OrbitCamera::ConstrainAngles()
{
	// x 수직 회전 각도 제한
	// https://docs.microsoft.com/ko-kr/dotnet/api/system.math.clamp?f1url=%3FappId%3DDev16IDEF1%26l%3DKO-KR%26k%3Dk(std%253A%253Aclamp);k(clamp);k(DevLang-C%252B%252B);k(TargetOS-Windows)%26rd%3Dtrue&view=net-5.0
	//std::clamp(m_RotationAngles.x, m_MinVerticalAngle, m_MaxVerticalAngle);	// 왜 안될까..

	if (m_RotationAngles.x > m_MaxVerticalAngle)
	{
		m_RotationAngles.x = m_MaxVerticalAngle;
	}

	if (m_RotationAngles.x < m_MinVerticalAngle)
	{
		m_RotationAngles.x = m_MinVerticalAngle;
	}

	//CA_TRACE("x = %f", m_RotationAngles.x);

	// y축 angle 보정
	if (m_RotationAngles.y > 360.0f)
	{
		m_RotationAngles.y -= 360.0f;
	}

	else if (m_RotationAngles.y < 0.0f)
	{
		m_RotationAngles.y += 360.0f;
	}
}

/// <summary>
/// 총기 반동시킬 값 세팅
/// </summary>
void OrbitCamera::SetRecoil()
{
	m_bShootRecoil = true;

	// 해줘야 카메라가 위로 올라 간다.
	m_ShootReciolOffset.x = -20.0f;

	m_TotalTime = 0.5f;
	m_RecoilTime = 0.1f;
	m_RecoverTime = m_TotalTime - m_RecoilTime;
	m_RecoverRatio = m_RecoilTime / m_RecoverTime;
}

/// <summary>
/// 총기반동 중이면 총기반동 시킨다.
/// </summary>
void OrbitCamera::ShootRecoil()
{
	// 남은 반동시간 감소
	m_RecoilTime -= CL::Input::s_DeltaTime;
	
	m_RotationAngles += m_ShootReciolOffset * CL::Input::s_DeltaTime;
	m_Transform->SetRotationFromVec(m_RotationAngles);
	//ConstrainAngles();
	
	CA_TRACE("총기 반동 중");
}

/// <summary>
/// 총기 반동 후 회복 시간동안 반동잡기
/// </summary>
void OrbitCamera::ShootRecover()
{
	// 남은 회복시간 감소
	m_RecoverTime -= CL::Input::s_DeltaTime;

	m_RotationAngles -= m_ShootReciolOffset * m_RecoverRatio * CL::Input::s_DeltaTime;
	m_Transform->SetRotationFromVec(m_RotationAngles);

	// 회복이 끝나면 총기반동 해제
	if (m_RecoverTime <= 0.0f)
	{
		m_bShootRecoil = false;
		CA_TRACE("총기 반동 회복	끝");
	}

	CA_TRACE("총기 반동 회복중");
}


void OrbitCamera::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_FocusTarget = m_FocusTarget->GetComponetId();

	m_SaveData->m_FocusPosition = &m_FocusPosition;

	m_SaveData->m_Distance = m_Distance;
	m_SaveData->m_RotationSpeed = m_RotationSpeed;

	m_SaveData->m_RotationAngles = &m_RotationAngles;

	m_SaveData->m_MaxVerticalAngle = m_MaxVerticalAngle;
	m_SaveData->m_MinVerticalAngle = m_MinVerticalAngle;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void OrbitCamera::LoadData()
{
	m_SaveData->m_FocusPosition = &m_FocusPosition;
	m_SaveData->m_RotationAngles = &m_RotationAngles;

	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_Distance = m_SaveData->m_Distance;
	m_RotationSpeed = m_SaveData->m_RotationSpeed;

	m_SaveData->m_MaxVerticalAngle = m_MaxVerticalAngle;
	m_SaveData->m_MinVerticalAngle = m_MinVerticalAngle;
}

void OrbitCamera::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	if (m_SaveData->m_FocusTarget != NULL)
	{
		m_FocusTarget = dynamic_cast<Transform*>(component->at(m_SaveData->m_FocusTarget));
	}
}

