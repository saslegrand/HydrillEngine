#pragma once

#include <vector>

#include "Resources/Resource/Resource.hpp"
#include "Resources/Parsers/ParserFlags.hpp"
#include "Tools/Flags.hpp"

#include "Generated/Sound.rfkh.h"

//struct GLSound;
struct ma_sound;

/**
@brief Sound 
*/
class HY_CLASS() Sound : public Resource
{
private:
	HY_METHOD(rfk::Instantiator)
		static rfk::UniquePtr<Sound> defaultInstantiator(const HYGUID & uid);

private:
	std::string m_realPath;
	std::unique_ptr<ma_sound> data;
	bool m_initialized = false;

public:

	ENGINE_API Sound(const HYGUID & uid);
	~Sound();

private:
	bool Load();

public:
	ma_sound* GetSoundData() const;

	virtual bool Import(const std::string& path) override;
	virtual void LoadInGPUMemory() override;
	virtual void UnloadFromGPUMemory() override;

	void EraseSoundData();

	virtual void Serialize() override;
	virtual void Deserialize() override;

	Sound_GENERATED
};

File_Sound_GENERATED
