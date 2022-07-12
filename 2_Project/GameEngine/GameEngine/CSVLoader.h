#pragma once
#include <memory>
#include <vector>
#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"
#include "SceneBase.h"

class CSVLoader : public ComponentBase, public ResourceObserver
{
public:
	_DLL CSVLoader();
	_DLL virtual ~CSVLoader();

	virtual void Start() override;
	virtual void Update(float dTime) override {};
	virtual void OnRender() override;

	virtual void ObserverUpdate(std::shared_ptr<__interface IResourceManager> pResourceManager) override;

	_DLL void SetSceneKind(eScene sceneKind);

	_DLL std::vector<struct QuestBlock> GetQuest();
	_DLL std::vector<struct DialogueBlock> GetDialogue();

private:
	eScene m_CurSceneKind = eScene::None;

	std::unordered_map<std::string, std::vector<struct QuestBlock>> m_QuestText_UM;
	std::unordered_map<std::string, std::vector<struct DialogueBlock>> m_DialogueText_UM;
		
	std::string m_CurQuest;
	std::string m_CurDialogue;
};

