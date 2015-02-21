// Copyright (C) 2015 Jérôme Leclercq
// This file is part of the "Nazara Engine - Audio module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Audio/Loaders/sndfile.hpp>
#include <Nazara/Audio/Algorithm.hpp>
#include <Nazara/Audio/Audio.hpp>
#include <Nazara/Audio/Config.hpp>
#include <Nazara/Audio/Music.hpp>
#include <Nazara/Audio/SoundBuffer.hpp>
#include <Nazara/Audio/SoundStream.hpp>
#include <Nazara/Core/CallOnExit.hpp>
#include <Nazara/Core/Endianness.hpp>
#include <Nazara/Core/Error.hpp>
#include <Nazara/Core/File.hpp>
#include <Nazara/Core/InputStream.hpp>
#include <Nazara/Core/MemoryStream.hpp>
#include <memory>
#include <set>
#include <vector>
#include <sndfile/sndfile.h>
#include <Nazara/Audio/Debug.hpp>

namespace
{
	sf_count_t GetSize(void* user_data)
	{
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
		return stream->GetSize();
	}

    sf_count_t Read(void* ptr, sf_count_t count, void* user_data)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
        return static_cast<sf_count_t>(stream->Read(ptr, static_cast<std::size_t>(count)));
    }

    sf_count_t Seek(sf_count_t offset, int whence, void* user_data)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
        switch (whence)
        {
			case SEEK_CUR:
				stream->Read(nullptr, static_cast<std::size_t>(offset));
				break;

			case SEEK_END:
				stream->SetCursorPos(stream->GetSize() + offset); // L'offset est négatif ici
				break;

			case SEEK_SET:
				stream->SetCursorPos(offset);
				break;

			default:
				NazaraInternalError("Seek mode not handled");
        }

        return stream->GetCursorPos();
    }

    sf_count_t Tell(void* user_data)
    {
        NzInputStream* stream = static_cast<NzInputStream*>(user_data);
        return stream->GetCursorPos();
    }

	static SF_VIRTUAL_IO callbacks = {GetSize, Seek, Read, nullptr, Tell};

	class sndfileStream : public NzSoundStream
	{
		public:
			sndfileStream() :
			m_handle(nullptr)
			{
			}

			~sndfileStream()
			{
				if (m_handle)
					sf_close(m_handle);
			}

			nzUInt32 GetDuration() const
			{
				return m_duration;
			}

			nzAudioFormat GetFormat() const
			{
				// Nous avons besoin du nombre de canaux d'origine pour convertir en mono, nous trichons donc un peu...
				if (m_mixToMono)
					return nzAudioFormat_Mono;
				else
					return m_format;
			}

			unsigned int GetSampleCount() const
			{
				return m_sampleCount;
			}

			unsigned int GetSampleRate() const
			{
				return m_sampleRate;
			}

			bool Open(const NzString& filePath, bool forceMono)
			{
				// Nous devons gérer nous-même le flux car il doit rester ouvert après le passage du loader
				// (les flux automatiquement ouverts par le ResourceLoader étant fermés après celui-ci)
				std::unique_ptr<NzFile> file(new NzFile);
				if (!file->Open(filePath, NzFile::ReadOnly))
				{
					NazaraError("Failed to open stream from file: " + NzError::GetLastError());
					return false;
				}

				m_ownedStream = std::move(file);
				return Open(*m_ownedStream, forceMono);
			}

			bool Open(const void* data, std::size_t size, bool forceMono)
			{
				m_ownedStream.reset(new NzMemoryStream(data, size));
				return Open(*m_ownedStream, forceMono);
			}

			bool Open(NzInputStream& stream, bool forceMono)
			{
				SF_INFO infos;
				infos.format = 0; // Format inconnu

				m_handle = sf_open_virtual(&callbacks, SFM_READ, &infos, &stream);
				if (!m_handle)
				{
					NazaraError("Failed to open sound: " + NzString(sf_strerror(m_handle)));
					return false;
				}

				// Un peu de RAII
				NzCallOnExit onExit([this]
				{
					sf_close(m_handle);
					m_handle = nullptr;
				});

				m_format = NzAudio::GetAudioFormat(infos.channels);
				if (m_format == nzAudioFormat_Unknown)
				{
					NazaraError("Channel count not handled");
					return false;
				}

				m_sampleCount = infos.channels*infos.frames;
				m_sampleRate = infos.samplerate;

				// Durée de la musique (s) = samples / channels*rate
				m_duration = 1000*m_sampleCount / (m_format*m_sampleRate);

				// https://github.com/LaurentGomila/SFML/issues/271
				// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
				///FIXME: Seulement le Vorbis ?
				if (infos.format & SF_FORMAT_VORBIS)
					sf_command(m_handle, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

				// On mixera en mono lors de la lecture
				if (forceMono && m_format != nzAudioFormat_Mono)
				{
					m_mixToMono = true;
					m_sampleCount = infos.frames;
				}
				else
					m_mixToMono = false;

				onExit.Reset();

				return true;
			}

			unsigned int Read(void* buffer, unsigned int sampleCount)
			{
				// Si la musique a été demandée en mono, nous devons la convertir à la volée lors de la lecture
				if (m_mixToMono)
				{
					// On garde un buffer sur le côté pour éviter la réallocation
					m_mixBuffer.resize(m_format*sampleCount);
					unsigned int readSampleCount = sf_read_short(m_handle, m_mixBuffer.data(), m_format*sampleCount);
					NzMixToMono(m_mixBuffer.data(), static_cast<nzInt16*>(buffer), m_format, sampleCount);

					return readSampleCount / m_format;
				}
				else
					return sf_read_short(m_handle, static_cast<nzInt16*>(buffer), sampleCount);
			}

			void Seek(nzUInt32 offset)
			{
				sf_seek(m_handle, offset*m_sampleRate / 1000, SEEK_SET);
			}

		private:
			std::vector<nzInt16> m_mixBuffer;
			std::unique_ptr<NzInputStream> m_ownedStream;
			nzAudioFormat m_format;
			SNDFILE* m_handle;
			bool m_mixToMono;
			unsigned int m_duration;
			unsigned int m_sampleCount;
			unsigned int m_sampleRate;
	};

	bool IsSupported(const NzString& extension)
	{
		static std::set<NzString> supportedExtensions = {
			"aiff", "au", "avr", "caf", "flac", "htk", "ircam", "mat4", "mat5", "mpc2k",
			"nist","ogg", "pvf", "raw", "rf64", "sd2", "sds", "svx", "voc", "w64", "wav", "wve"
		};

		return supportedExtensions.find(extension) != supportedExtensions.end();
	}

	nzTernary CheckMusic(NzInputStream& stream, const NzMusicParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		info.format = 0; // Format inconnu

		// Si on peut ouvrir le flux, c'est qu'il est dans un format compatible
		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (file)
		{
			sf_close(file);
			return nzTernary_True;
		}
		else
			return nzTernary_False;
	}

	bool LoadMusicFile(NzMusic* music, const NzString& filePath, const NzMusicParams& parameters)
	{
		std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
		if (!musicStream->Open(filePath, parameters.forceMono))
		{
			NazaraError("Failed to open music stream");
			return false;
		}

		if (!music->Create(musicStream.get())) // Transfert de propriété
		{
			NazaraError("Failed to create music");
			return false;
		}

		// Le pointeur a été transféré avec succès, nous pouvons laisser tomber la propriété
		musicStream.release();

		return true;
	}

	bool LoadMusicMemory(NzMusic* music, const void* data, std::size_t size, const NzMusicParams& parameters)
	{
		std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
		if (!musicStream->Open(data, size, parameters.forceMono))
		{
			NazaraError("Failed to open music stream");
			return false;
		}

		if (!music->Create(musicStream.get())) // Transfert de propriété
		{
			NazaraError("Failed to create music");
			return false;
		}

		// Le pointeur a été transféré avec succès, nous pouvons laisser tomber la propriété
		musicStream.release();

		return true;
	}

	bool LoadMusicStream(NzMusic* music, NzInputStream& stream, const NzMusicParams& parameters)
	{
		std::unique_ptr<sndfileStream> musicStream(new sndfileStream);
		if (!musicStream->Open(stream, parameters.forceMono))
		{
			NazaraError("Failed to open music stream");
			return false;
		}

		if (!music->Create(musicStream.get())) // Transfert de propriété
		{
			NazaraError("Failed to create music");
			return false;
		}

		// Le pointeur a été transféré avec succès, nous pouvons laisser tomber la propriété
		musicStream.release();

		return true;
	}

	nzTernary CheckSoundBuffer(NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		NazaraUnused(parameters);

		SF_INFO info;
		info.format = 0;

		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (file)
		{
			sf_close(file);
			return nzTernary_True;
		}
		else
			return nzTernary_False;
	}

	bool LoadSoundBuffer(NzSoundBuffer* soundBuffer, NzInputStream& stream, const NzSoundBufferParams& parameters)
	{
		SF_INFO info;
		info.format = 0;

		SNDFILE* file = sf_open_virtual(&callbacks, SFM_READ, &info, &stream);
		if (!file)
		{
			NazaraError("Failed to load sound file: " + NzString(sf_strerror(file)));
			return false;
		}

		// Lynix utilise RAII...
		// C'est très efficace !
		// MemoryLeak est confus...
		NzCallOnExit onExit([file]
		{
			sf_close(file);
		});

		nzAudioFormat format = NzAudio::GetAudioFormat(info.channels);
		if (format == nzAudioFormat_Unknown)
		{
			NazaraError("Channel count not handled");
			return false;
		}

		// https://github.com/LaurentGomila/SFML/issues/271
		// http://www.mega-nerd.com/libsndfile/command.html#SFC_SET_SCALE_FLOAT_INT_READ
		///FIXME: Seulement le Vorbis ?
		if (info.format & SF_FORMAT_VORBIS)
			sf_command(file, SFC_SET_SCALE_FLOAT_INT_READ, nullptr, SF_TRUE);

		unsigned int sampleCount = static_cast<unsigned int>(info.frames * info.channels);
		std::unique_ptr<nzInt16[]> samples(new nzInt16[sampleCount]);

		if (sf_read_short(file, samples.get(), sampleCount) != sampleCount)
		{
			NazaraError("Failed to read samples");
			return false;
		}

		// Une conversion en mono est-elle nécessaire ?
		if (parameters.forceMono && format != nzAudioFormat_Mono)
		{
			// Nous effectuons la conversion en mono dans le même buffer (il va de toute façon être copié)
			NzMixToMono(samples.get(), samples.get(), static_cast<unsigned int>(info.channels), static_cast<unsigned int>(info.frames));

			format = nzAudioFormat_Mono;
			sampleCount = static_cast<unsigned int>(info.frames);
		}

		if (!soundBuffer->Create(format, sampleCount, info.samplerate, samples.get()))
		{
			NazaraError("Failed to create sound buffer");
			return false;
		}

		return true;
	}
}

void NzLoaders_sndfile_Register()
{
	NzMusicLoader::RegisterLoader(IsSupported, CheckMusic, LoadMusicStream, LoadMusicFile, LoadMusicMemory);
	NzSoundBufferLoader::RegisterLoader(IsSupported, CheckSoundBuffer, LoadSoundBuffer);
}

void NzLoaders_sndfile_Unregister()
{
	NzMusicLoader::UnregisterLoader(IsSupported, CheckMusic, LoadMusicStream, LoadMusicFile, LoadMusicMemory);
	NzSoundBufferLoader::UnregisterLoader(IsSupported, CheckSoundBuffer, LoadSoundBuffer);
}
