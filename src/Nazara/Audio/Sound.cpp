// Copyright (C) 2013 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Sound.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Core/Error.hpp>
#include <cstring>
#include <stdexcept>
#include <AL/al.h>
#include <Nazara/Audio/Debug.hpp>

NzSound::NzSound(const NzSoundBuffer* soundBuffer)
{
	SetBuffer(soundBuffer);
}

NzSound::NzSound(const NzSound& sound) :
NzSoundEmitter(sound)
{
	SetBuffer(sound.m_buffer);
}

NzSound::~NzSound()
{
	Stop();
}

void NzSound::EnableLooping(bool loop)
{
	alSourcei(m_source, AL_LOOPING, loop);
}

const NzSoundBuffer* NzSound::GetBuffer() const
{
	return m_buffer;
}

nzUInt32 NzSound::GetDuration() const
{
	#if NAZARA_AUDIO_SAFE
	if (!m_buffer)
	{
		NazaraError("No sound buffer");
		return 0;
	}
	#endif

	return m_buffer->GetDuration();
}

nzUInt32 NzSound::GetPlayingOffset() const
{
	ALfloat seconds = -1.f;
	alGetSourcef(m_source, AL_SEC_OFFSET, &seconds);

	return static_cast<nzUInt32>(seconds*1000);
}

nzSoundStatus NzSound::GetStatus() const
{
	return GetInternalStatus();
}

bool NzSound::IsLooping() const
{
	ALint loop;
	alGetSourcei(m_source, AL_LOOPING, &loop);

	return loop != AL_FALSE;
}

bool NzSound::LoadFromFile(const NzString& filePath, const NzSoundBufferParams& params)
{
	NzSoundBuffer* buffer = new NzSoundBuffer;
	if (!buffer->LoadFromFile(filePath, params))
	{
		NazaraError("Failed to load buffer from file (" + filePath + ')');
		delete buffer;

		return false;
	}

	SetBuffer(buffer);

	buffer->SetPersistent(false);

	return true;
}

bool NzSound::LoadFromMemory(const void* data, std::size_t size, const NzSoundBufferParams& params)
{
	NzSoundBuffer* buffer = new NzSoundBuffer;
	if (!buffer->LoadFromMemory(data, size, params))
	{
		NazaraError("Failed to load buffer from memory (" + NzString::Pointer(data) + ')');
		delete buffer;

		return false;
	}

	SetBuffer(buffer);

	buffer->SetPersistent(false);

	return true;
}

bool NzSound::LoadFromStream(NzInputStream& stream, const NzSoundBufferParams& params)
{
	NzSoundBuffer* buffer = new NzSoundBuffer;
	if (!buffer->LoadFromStream(stream, params))
	{
		NazaraError("Failed to load buffer from stream");
		delete buffer;

		return false;
	}

	SetBuffer(buffer);

	buffer->SetPersistent(false);

	return true;
}

void NzSound::Pause()
{
	alSourcePause(m_source);
}

bool NzSound::Play()
{
	#if NAZARA_AUDIO_SAFE
	if (!m_buffer)
	{
		NazaraError("No sound buffer to play");
		return false;
	}
	#endif

	alSourcePlay(m_source);

	return true;
}

void NzSound::SetBuffer(const NzSoundBuffer* buffer)
{
	if (m_buffer == buffer)
		return;

	Stop();

	m_buffer = buffer;

	if (m_buffer)
		alSourcei(m_source, AL_BUFFER, m_buffer->GetOpenALBuffer());
	else
		alSourcei(m_source, AL_BUFFER, AL_NONE);
}

void NzSound::SetPlayingOffset(nzUInt32 offset)
{
	alSourcef(m_source, AL_SEC_OFFSET, offset/1000.f);
}

void NzSound::Stop()
{
	alSourceStop(m_source);
}
