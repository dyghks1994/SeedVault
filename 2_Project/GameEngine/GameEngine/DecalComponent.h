#pragma once

#include "DLLDefine.h"
#include "ComponentBase.h"
#include "ResourceObserver.h"

#include <string>

__interface IRenderer;

class DecalComponent : public ComponentBase, public ResourceObserver
{
public:
	_DLL DecalComponent();
	_DLL virtual ~DecalComponent();

public:
	virtual void Start() override;
	virtual void Update(float dTime) override;
	virtual void OnRender() override;

	std::string m_DecalTextureName;
	unsigned int m_DecalTextureIndex;

public:
	virtual void ObserverUpdate(std::shared_ptr<IResourceManager> pReosureManager) override;
	std::shared_ptr<IRenderer> m_pRenderer;


};

