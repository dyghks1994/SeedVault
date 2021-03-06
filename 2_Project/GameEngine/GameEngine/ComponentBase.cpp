#include "pch.h"
#include "EngineDLL.h"

#include "IdDll.h"
#include "GameObject.h"
#include "ComponentBase.h"
#include <typeinfo>


ComponentBase::ComponentBase()
	:m_bEnable(true)
	, m_Type(ComponentType::Etc)
	, m_ComponentId(0)
	, m_Transform(nullptr)
	, m_pMyObject(nullptr)
{
}

ComponentBase::ComponentBase(ComponentType type)
	:m_bEnable(true)
	, m_Type(type)
	, m_ComponentId(ID::GetIdByType(IDCOMPONENT, static_cast<unsigned int>(type)))
	, m_Transform(nullptr)
	, m_pMyObject(nullptr)
{

	////컴포넌트는 생성됨과 동시에 등록된다.
	//오브젝트에 등록될때 생성되는 것으로 바꾸었다.
	//DLLEngine::RegisterComponent(this);
}

ComponentBase::~ComponentBase()
{

}

void ComponentBase::OnCollisionEnter(Collision collision)
{
}

void ComponentBase::OnCollisionStay(Collision collision)
{
}

void ComponentBase::OnCollisionExit(Collision collision)
{
}

void ComponentBase::OnPreRender()
{
	// 아무것도 하지 않는다
}

void ComponentBase::OnPostRender()
{
	// 아무것도 하지 않는다
}

void ComponentBase::OnHelperRender()
{
	// 아무것도 하지 않는다
}

void ComponentBase::OnUIRender()
{
	// 아무것도 하지 않는다
}

void ComponentBase::Release()
{

}

void ComponentBase::OnEnable()
{
	m_bEnable = true;
}

void ComponentBase::OnDisable()
{
	m_bEnable = false;
}

void ComponentBase::SetEnable(const bool& enalble)
{
	m_bEnable = enalble;
}

bool ComponentBase::GetIsEnabled()
{
	return m_bEnable;
}

const unsigned int ComponentBase::GetComponetId()
{
	return m_ComponentId;
}

const int ComponentBase::GetComponentType()
{
	return static_cast<int>(m_Type);
}

void ComponentBase::SetMyObject(GameObject* object)
{
	m_pMyObject = object;
}

bool ComponentBase::GetObjectIsEnabled()
{
	return m_pMyObject->GetIsEnabled();
}

bool ComponentBase::GetObjectIsExist()
{
	return (m_pMyObject != nullptr) ? true : false;
}

GameObject* ComponentBase::GetMyObject()
{
	return m_pMyObject;
}

void ComponentBase::Save()
{
	m_Value = new Json::Value();
	
	SaveData();
}

void ComponentBase::Load()
{
	LoadData();
}

void ComponentBase::SaveData()
{
   ///오류방지용 임시
}

void ComponentBase::LoadData()
{
   ///오류방지용 임시
}

void ComponentBase::LoadPtrData(std::map<unsigned int, GameObject*>* gameobject, std::map<unsigned int, ComponentBase*>* component)
{
	///오류방지용 임시
}
