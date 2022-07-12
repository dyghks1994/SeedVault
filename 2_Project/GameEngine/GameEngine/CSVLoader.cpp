#include "pch.h"
#include "CSVLoader.h"

#include "EffectUIEnum.h"
#include "IResourceManager.h"

CSVLoader::CSVLoader()
	: ComponentBase(ComponentType::Etc)
{
}

CSVLoader::~CSVLoader()
{
}

void CSVLoader::Start()
{
}

void CSVLoader::OnRender()
{
}

void CSVLoader::ObserverUpdate(std::shared_ptr<IResourceManager> pResourceManager)
{
	m_QuestText_UM = pResourceManager->GetWholeQuestText();
	m_DialogueText_UM = pResourceManager->GetWholeDialogueText();

	switch (m_CurSceneKind)
	{
	case eScene::None:
		break;

	case eScene::Tutorial:
		m_CurQuest = "Q_tutorial_UTF8";
		m_CurDialogue = "D_tutorial_UTF8";
		break;

	case eScene::Generator:
		m_CurQuest = "Q_genroom_UTF8";
		m_CurDialogue = "D_genroom_UTF8";
		break;
	}
}

void CSVLoader::SetSceneKind(eScene sceneKind)
{
	m_CurSceneKind = sceneKind;
}

std::vector<QuestBlock> CSVLoader::GetQuest()
{
	if (m_CurQuest.size() <= 0)
	{
		return std::vector<struct QuestBlock>();
	}

	return m_QuestText_UM.at(m_CurQuest.c_str());
}

std::vector<DialogueBlock> CSVLoader::GetDialogue()
{
	if (m_CurDialogue.size() <= 0)
	{
		return std::vector<struct DialogueBlock>();
	}

	return m_DialogueText_UM.at(m_CurDialogue.c_str());
}

