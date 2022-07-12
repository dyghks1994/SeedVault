#include "pch.h"
#include "Zombie_Runner_Move.h"
#include "CameraController.h"
#include "CLTutorial.h"
#include "GameManager.h"

bool GameManager::s_bDebugMode = false;
bool GameManager::s_bTPSMode = true;

/// <summary>
/// 현재 디버그모드 상태값 리턴
/// </summary>
/// <returns></returns>
bool GameManager::IsDebugMode()
{
	return s_bDebugMode;
}

/// <summary>
/// 디버그 모드 설정
/// (엔진의 모드도 같이 설정한다)
/// </summary>
/// <param name="val"></param>
void GameManager::SetDebugMode(const bool& val)
{
	// 클라이언트 디버그모드 설정
	s_bDebugMode = val;

	// 엔진 디버그모드 설정
	//DLLEngine::SetDebugRenderMode(val);
}

/// <summary>
/// 디버깅 모드 여부를 전환(반전)시키고 결과를 리턴한다.
/// 디버그 모드가 On(true)면 디버깅 정보를 그리도록 하자
/// </summary>
bool GameManager::TogleDebugMode()
{
	s_bDebugMode = !s_bDebugMode;

	return s_bDebugMode;
}

GameManager::GameManager()
	: ComponentBase(ComponentType::Etc)
	, m_GameState(GameState::Play)

	, m_SaveData(new GameManager_Save())
{
	CA_TRACE("[GameManager] Create!");
}

GameManager::~GameManager()
{
	CA_TRACE("[GameManager] Delete!");

	SAFE_DELETE(m_SaveData);


	// 다른 씬에서 혹시 영향을 받을까봐 초기화
	s_bDebugMode = false;
	s_bTPSMode = false;
}

void GameManager::Start()
{
	// 게임 모드(아직 의미 없음)
	m_GameState = GameState::Play;

	// 디버깅모드 설정
	s_bDebugMode = false;

	// TPS 플레이어 컨트롤 모드
	s_bTPSMode = true;
	//ShowCursor(false);
	DLLEngine::ShowMouseCursor(false);
	DLLEngine::SetNowCamera("MainCamera");

}

void GameManager::Update(float dTime)
{
	if (DLLInput::InputKeyDown(CL::KeyMap::DEBUG))
	{
		SetDebugMode(!s_bDebugMode);
	}


	if (DLLInput::InputKeyDown(static_cast<int>(CL::KeyCode::KEY_ESCAPE)))
	{
		s_bTPSMode = !s_bTPSMode;

		if (IsTPSMode() == true)
		{
			// 윈도우 기준 화면 중앙 좌표
			POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

			HWND _hWnd = DLLWindow::GetWindowHandle();
			ClientToScreen(_hWnd, &_centerPos);
			SetCursorPos(_centerPos.x, _centerPos.y);	// 윈도우 내부 좌표가아니라 스크린(주 모니터)좌표계임...


			//ShowCursor(false);
			DLLEngine::ShowMouseCursor(false);
			DLLEngine::SetNowCamera("MainCamera");

			// 플레이어 고정
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(false);
		}

		else
		{
			//ShowCursor(true);
			DLLEngine::ShowMouseCursor(true);

			// 프리캠 포지션 메인캠위치로 이동
			Transform* MainCameraTf = DLLEngine::FindGameObjectByName("MainCamera")->m_Transform;
			Transform* CameraParentTf = DLLEngine::FindGameObjectByName("CameraParent")->m_Transform;

			GameObject* freeCamera = DLLEngine::FindGameObjectByName("FreeCamera");
			CameraController* freeCameraController = freeCamera->GetComponent<CameraController>();

			DLLEngine::SetNowCamera("FreeCamera");
			freeCamera->m_Transform->m_Position = MainCameraTf->GetWorldPosition();				// 메인카메라의 월드 포지션으로 이동
			freeCameraController->SetRotationAngles(CameraParentTf->m_EulerAngles);	// 카메라 페어런트의 Rotation 과 같이 회전

			// 플레이어 고정 해제
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(true);
		}
	}	

}

void GameManager::OnRender()
{

}

bool GameManager::IsTPSMode()
{
	return s_bTPSMode;
}

/// <summary>
/// 플레이어 컨트롤 중일 때 마우스 커서가 윈도우 밖으로 나가는 것을 방지하기 위해
/// 커서 위치를 윈도우의 가운데로 맞춘다.
/// </summary>
void GameManager::LockingMouseCursor()
{
	POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

	HWND _hWnd = DLLWindow::GetWindowHandle();
	// 윈도우 좌표를 스크린 좌표로 변환
	ClientToScreen(_hWnd, &_centerPos);

	// 마우스 커서를 윈도우의 중앙으로 설정
	SetCursorPos(_centerPos.x, _centerPos.y);	// 윈도우 내부 좌표가아니라 스크린(주 모니터)좌표계임...
}


void GameManager::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	m_SaveData->m_GameState = static_cast<int>(m_GameState);

	m_SaveData->s_bDebugMode = s_bDebugMode;
	m_SaveData->s_bTPSMode = s_bTPSMode;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void GameManager::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
	SetEnable(m_SaveData->m_bEnable);

	m_GameState = static_cast<GameState>(m_SaveData->m_GameState);

	s_bDebugMode = m_SaveData->s_bDebugMode;
	s_bTPSMode = m_SaveData->s_bTPSMode;
}