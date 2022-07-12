#include "pch.h"
#include "DecalComponent.h"
#include "IRenderer.h"
#include "IResourceManager.h"
#include "EngineDLL.h"

DecalComponent::DecalComponent()
	: ComponentBase(ComponentType::Rendering)
	, m_DecalTextureIndex(0)
{

}

DecalComponent::~DecalComponent()
{

}

void DecalComponent::Start()
{
	m_pRenderer = DLLEngine::GetEngine()->GetIRenderer();
}

void DecalComponent::Update(float dTime)
{

}

void DecalComponent::OnRender()
{
	m_pRenderer->DecalDraw(m_DecalTextureIndex, m_Transform->GetWorld());
}

void DecalComponent::ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager)
{
	m_DecalTextureIndex = pReosureManager->GetTextureIndex(m_DecalTextureName);
}

