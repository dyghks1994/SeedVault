#include "pch.h"
#include "Zombie_Runner_Move.h"
#include "CameraController.h"
#include "CLTutorial.h"
#include "GameManager.h"

bool GameManager::s_bDebugMode = false;
bool GameManager::s_bTPSMode = true;

/// <summary>
/// ���� ����׸�� ���°� ����
/// </summary>
/// <returns></returns>
bool GameManager::IsDebugMode()
{
	return s_bDebugMode;
}

/// <summary>
/// ����� ��� ����
/// (������ ��嵵 ���� �����Ѵ�)
/// </summary>
/// <param name="val"></param>
void GameManager::SetDebugMode(const bool& val)
{
	// Ŭ���̾�Ʈ ����׸�� ����
	s_bDebugMode = val;

	// ���� ����׸�� ����
	//DLLEngine::SetDebugRenderMode(val);
}

/// <summary>
/// ����� ��� ���θ� ��ȯ(����)��Ű�� ����� �����Ѵ�.
/// ����� ��尡 On(true)�� ����� ������ �׸����� ����
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


	// �ٸ� ������ Ȥ�� ������ ������� �ʱ�ȭ
	s_bDebugMode = false;
	s_bTPSMode = false;
}

void GameManager::Start()
{
	// ���� ���(���� �ǹ� ����)
	m_GameState = GameState::Play;

	// ������� ����
	s_bDebugMode = false;

	// TPS �÷��̾� ��Ʈ�� ���
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
			// ������ ���� ȭ�� �߾� ��ǥ
			POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

			HWND _hWnd = DLLWindow::GetWindowHandle();
			ClientToScreen(_hWnd, &_centerPos);
			SetCursorPos(_centerPos.x, _centerPos.y);	// ������ ���� ��ǥ���ƴ϶� ��ũ��(�� �����)��ǥ����...


			//ShowCursor(false);
			DLLEngine::ShowMouseCursor(false);
			DLLEngine::SetNowCamera("MainCamera");

			// �÷��̾� ����
			GameObject* _player = DLLEngine::FindGameObjectByName("Player");
			_player->GetComponent<PhysicsActor>()->SetKinematic(false);
		}

		else
		{
			//ShowCursor(true);
			DLLEngine::ShowMouseCursor(true);

			// ����ķ ������ ����ķ��ġ�� �̵�
			Transform* MainCameraTf = DLLEngine::FindGameObjectByName("MainCamera")->m_Transform;
			Transform* CameraParentTf = DLLEngine::FindGameObjectByName("CameraParent")->m_Transform;

			GameObject* freeCamera = DLLEngine::FindGameObjectByName("FreeCamera");
			CameraController* freeCameraController = freeCamera->GetComponent<CameraController>();

			DLLEngine::SetNowCamera("FreeCamera");
			freeCamera->m_Transform->m_Position = MainCameraTf->GetWorldPosition();				// ����ī�޶��� ���� ���������� �̵�
			freeCameraController->SetRotationAngles(CameraParentTf->m_EulerAngles);	// ī�޶� ��Ʈ�� Rotation �� ���� ȸ��

			// �÷��̾� ���� ����
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
/// �÷��̾� ��Ʈ�� ���� �� ���콺 Ŀ���� ������ ������ ������ ���� �����ϱ� ����
/// Ŀ�� ��ġ�� �������� ����� �����.
/// </summary>
void GameManager::LockingMouseCursor()
{
	POINT _centerPos = { DLLWindow::GetScreenWidth() / 2, DLLWindow::GetScreenHeight() / 2 };

	HWND _hWnd = DLLWindow::GetWindowHandle();
	// ������ ��ǥ�� ��ũ�� ��ǥ�� ��ȯ
	ClientToScreen(_hWnd, &_centerPos);

	// ���콺 Ŀ���� �������� �߾����� ����
	SetCursorPos(_centerPos.x, _centerPos.y);	// ������ ���� ��ǥ���ƴ϶� ��ũ��(�� �����)��ǥ����...
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