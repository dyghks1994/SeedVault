#include "pch.h"
#include "Audio.h"
#include "AudioEngineDLL.h"
#include "SoundEvent.h"
#include "Transform.h"

_DLL Audio::Audio()
	:ComponentBase(ComponentType::Etc)
	, m_SaveData(new Audio_Save())
{

}

_DLL Audio::~Audio()
{
	AllEventStop();
}

void Audio::Start()
{

}

void Audio::Update(float dTime)
{
	for (int index = 0; index < m_3DEvent_V.size();)
	{
		//유효하지 않으면
		if (!m_3DEvent_V.at(index)->IsValid())
		{
			m_3DEvent_V.erase(m_3DEvent_V.begin() + index);
		}
		else
		{
			m_3DEvent_V.at(index)->Set3DAttribute(m_Transform->m_WorldTM);
			index++;
		}
	}

	for (int index = 0; index < m_2DEvent_V.size();)
	{
		//유효하지 않으면
		if (!m_2DEvent_V.at(index)->IsValid())
		{
			m_2DEvent_V.erase(m_2DEvent_V.begin() + index);
		}
		else
		{
			index++;
		}
	}

}

void Audio::OnPreRender()
{

}

void Audio::OnRender()
{

}

_DLL SoundEvent* Audio::PlayEvent(const char* path, bool duplicatePlay)
{
	SoundEvent* _event = nullptr;
	//중복재생 비허용이면
	if (duplicatePlay == false)
	{
		if (GetIsAlreadyPlaying(path))
		{
			return _event;
		}
	}

	_event = DLLAudio::PlayEvent(std::string(path));

	//존재하는 sound 경로일때 
	if (_event != nullptr)
	{
		if (_event->Is3D())
		{
			m_3DEvent_V.emplace_back(_event);
			_event->Set3DAttribute(m_Transform->m_WorldTM);
		}
		else
		{
			m_2DEvent_V.emplace_back(_event);
		}
	}

	return _event;
}

_DLL SoundEvent* Audio::PlayEvent(const char* path, const char* parameter, float parameterValue, bool duplicatePlay)
{
	SoundEvent* _event = Audio::PlayEvent(path, duplicatePlay);
	if (_event != nullptr)
	{
		_event->SetParameter(parameter, parameterValue);
	}
	return _event;
}

_DLL float Audio::GetEventDistance(const char* path)
{
	return DLLAudio::GetEventDistance(path);
}

_DLL void Audio::AllEventStop(bool fadeout)
{
	// Stop all sounds
	for (auto& event : m_2DEvent_V)
	{
		event->Stop(fadeout);
	}
	for (auto& event : m_3DEvent_V)
	{
		event->Stop(fadeout);
	}

	// Clear events
	m_2DEvent_V.clear();
	m_3DEvent_V.clear();
}

bool Audio::GetIsAlreadyPlaying(const char* path)
{
	if (DLLAudio::GetIs3D(path))
	{
		for (int index = 0; index < m_3DEvent_V.size(); index++)
		{
			if (m_3DEvent_V[index]->m_Path == std::string(path))
			{
				//이미 재생중입니다.
				return true;
			}
		}

	}
	else
	{
		for (int index = 0; index < m_2DEvent_V.size(); index++)
		{
			if (m_2DEvent_V[index]->m_Path == std::string(path))
			{
				//이미 재생중입니다.
				return true;
			}
		}
	}

	//재생중이지 않습니다.
	return false;
}

void Audio::SaveData()
{
	m_SaveData->m_bEnable = GetIsEnabled();
	m_SaveData->m_ComponentId = m_ComponentId;

	save::WriteValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);
}

void Audio::LoadData()
{
	save::ReadValue(m_Value, EraseClass(typeid(*this).name()), *m_SaveData);

	SetEnable(m_SaveData->m_bEnable);
}
