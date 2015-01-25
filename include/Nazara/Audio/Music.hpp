// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#pragma once

#ifndef NAZARA_MUSIC_HPP
#define NAZARA_MUSIC_HPP

#include <Nazara/Prerequesites.hpp>
#include <Nazara/Audio/Enums.hpp>
#include <Nazara/Audio/SoundEmitter.hpp>
#include <Nazara/Core/Resource.hpp>
#include <Nazara/Core/ResourceLoader.hpp>

struct NzMusicParams
{
	bool forceMono = false;

	bool IsValid() const;
};

class NzMusic;
class NzSoundStream;

using NzMusicLoader = NzResourceLoader<NzMusic, NzMusicParams>;

struct NzMusicImpl;

class NAZARA_API NzMusic : public NzResource, public NzSoundEmitter, NzNonCopyable
{
	friend NzMusicLoader;

	public:
		NzMusic() = default;
		~NzMusic();

		bool Create(NzSoundStream* soundStream);
		void Destroy();

		void EnableLooping(bool loop);

		nzUInt32 GetDuration() const;
		nzAudioFormat GetFormat() const;
		nzUInt32 GetPlayingOffset() const;
		unsigned int GetSampleCount() const;
		unsigned int GetSampleRate() const;
		nzSoundStatus GetStatus() const;

		bool IsLooping() const;

		bool OpenFromFile(const NzString& filePath, const NzMusicParams& params = NzMusicParams());
		bool OpenFromMemory(const void* data, std::size_t size, const NzMusicParams& params = NzMusicParams());
		bool OpenFromStream(NzInputStream& stream, const NzMusicParams& params = NzMusicParams());

		void Pause();
		void Play();

		void SetPlayingOffset(nzUInt32 offset);

		void Stop();

	private:
		NzMusicImpl* m_impl = nullptr;

		bool FillAndQueueBuffer(unsigned int buffer);
		void MusicThread();

		static NzMusicLoader::LoaderList s_loaders;
};

#endif // NAZARA_MUSIC_HPP
