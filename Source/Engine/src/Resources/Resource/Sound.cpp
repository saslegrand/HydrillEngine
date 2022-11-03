
#include <miniaudio/miniaudio.h>

#include "EngineContext.hpp"
#include "Resources/ResourcesManager.hpp"
#include "ECS/Systems/SoundSystem.hpp"
#include "Resources/Loaders/ResourcesLoader.hpp"
#include "Tools/StringHelper.hpp"
#include "Tools/SerializationUtils.hpp"

#include "Generated/Sound.rfks.h"

rfk::UniquePtr<Sound> Sound::defaultInstantiator(const HYGUID& uid)
{
	return rfk::makeUnique<Sound>(uid);
}

Sound::Sound(const HYGUID& uid)
	: Resource(uid, staticGetArchetype().getName())
{
	m_type = RESOURCE_TYPE::TEXTURE;

	data = std::make_unique<ma_sound>();

	SystemManager::GetSoundSystem().RegisterSound(this);
}

Sound::~Sound()
{
}

void Sound::EraseSoundData()
{
	if (m_initialized)
		ma_sound_uninit(data.get());
}

ma_sound* Sound::GetSoundData() const
{
	return data.get();
}

bool Sound::Load()
{
	ma_engine& soundEngine = SystemManager::GetSoundSystem().GetSoundEngine();

	// Load sound from file
	ma_result initResult = ma_sound_init_from_file(&soundEngine,
		m_realPath.c_str(),
		MA_SOUND_FLAG_DECODE, // | MA_SOUND_FLAG_NO_SPATIALIZATION,
		nullptr,
		nullptr,
		data.get());

	return m_initialized = (initResult == MA_SUCCESS);
}

bool Sound::Import(const std::string& path)
{
	Resource::Import(StringHelper::GetFileNameWithoutExtension(path) + ".sound");

	m_realPath = path;

	return Load();
}

void Sound::LoadInGPUMemory()
{
}

void Sound::UnloadFromGPUMemory()
{
	
}

void Sound::Serialize()
{
	json j;

	// Save data
	j["extension"] = StringHelper::GetFileExtensionFromPath(m_realPath);


	WriteJson(m_filepath, j);
}

void Sound::Deserialize()
{
	json j;
	ReadJson(m_filepath, j);

	std::string extension = "";
	m_realPath = Serialization::TryGetValue(j, "extension", extension) ? 
		StringHelper::GetFilePathWithoutExtension(m_filepath) + extension : 
		StringHelper::GetFilePathWithoutExtension(m_filepath);

	// Load settings
	Load();
}
