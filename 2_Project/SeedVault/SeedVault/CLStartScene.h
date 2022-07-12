#pragma once
#include "Scene.h"

enum class StartPopUp
{
	None,
	Chapter,
	Manual,
	Credit,
};

class SoundEvent;
class CLStartScene final : public Scene
{
public:
	CLStartScene();
	virtual ~CLStartScene();
	CLStartScene(const CLStartScene& other) = default;
	
	virtual void Initialize() override;
	virtual void Update(float dTime) override;
	virtual void Reset() override;

private:
	float m_width;
	float m_height;

	GameObject* m_StartImage;
	GameObject* m_StartLightImsage;
	GameObject* m_TitleName;

	GameObject* m_StartButton;
	GameObject* m_ChapterButton;
	GameObject* m_ManualButton;
	GameObject* m_CreditButton;
	GameObject* m_ExitButton;
	
	// 팝업 오브젝트
	GameObject* m_PopUP;
	GameObject* m_Chapter1;
	GameObject* m_Chapter2;

	GameObject* m_ManualImage;
	GameObject* m_CreditBackGrond;
	GameObject* m_CreditImage;

	GameObject* m_pMainCamera;
	GameObject* m_pAudioPlayer;


	StartPopUp m_CurPopupState;

	bool m_isPopUp;
	bool m_bBGMPlaying = false;
	SoundEvent* m_BGM;

	float m_randTime;
	float m_time;

	float m_CreditTime;
};

