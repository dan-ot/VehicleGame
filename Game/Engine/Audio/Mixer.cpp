//
// Mixer.cpp - XAudio2 implementation of the sound mixer
//

#include "pch.h"
#include "Mixer.h"
#include "../Assets/AssetManager.h"
#include "../Assets/AssetKit.h"
#include "../Assets/AssetRequester.h"
#include "../Assets/MusicResource.h"
#include "../Assets/SoundResource.h"
#include "../Assets/WavLoader.h"

#include <wrl/client.h>
#include <xaudio2.h>

#include <algorithm>
#include <array>
#include <deque>

namespace Engine
{
	namespace
	{
		constexpr size_t kVoiceCount = 32;
		constexpr size_t kMusicBufferSamples = 4096;

		constexpr size_t ChannelIndex(SoundChannel channel) noexcept
		{
			return static_cast<size_t>(channel);
		}

		bool SameKeys(const std::vector<AssetKey>& a, const std::vector<AssetKey>& b)
		{
			return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
		}
	}

	class Mixer::Impl
	{
	public:
		struct Voice
		{
			IXAudio2SourceVoice* source = nullptr;
			SoundChannel channel = SoundChannel::Other;
			const SoundResource* sound = nullptr;
			uint64_t startOrder = 0;
			bool active = false;
			WORD channels = 0;
			DWORD sampleRate = 0;
		};

		explicit Impl(AssetManager& assets)
			: assets(assets), persistentKit(RequesterId(PersistentOwner::Mixer))
		{
		}

		~Impl()
		{
			Shutdown();
		}

		bool Initialize()
		{
			if (engine)
			{
				return true;
			}

			if (FAILED(XAudio2Create(engine.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR)))
			{
				return false;
			}
			if (FAILED(engine->CreateMasteringVoice(&master)))
			{
				Shutdown();
				return false;
			}

			voices.resize(kVoiceCount);
			return true;
		}

		void Shutdown() noexcept
		{
			for (Voice& voice : voices)
			{
				if (voice.source)
				{
					voice.source->DestroyVoice();
					voice.source = nullptr;
				}
			}
			voices.clear();
			StopMusicVoice();

			for (IXAudio2SourceVoice* source : backgroundSources)
			{
				source->Stop();
				source->FlushSourceBuffers();
				source->DestroyVoice();
			}
			backgroundSources.clear();
			backgroundKeys.clear();

			if (master)
			{
				master->DestroyVoice();
				master = nullptr;
			}
			engine.Reset();
		}

		IXAudio2SourceVoice* CreateSource(const WAVEFORMATEX& format)
		{
			IXAudio2SourceVoice* source = nullptr;
			if (FAILED(engine->CreateSourceVoice(&source, &format)))
			{
				return nullptr;
			}
			return source;
		}

		Voice* AcquireVoice(SoundChannel channel)
		{
			for (Voice& voice : voices)
			{
				if (!voice.active)
				{
					voice.channel = channel;
					return &voice;
				}
			}

			Voice* oldest = &voices.front();
			for (Voice& voice : voices)
			{
				if (voice.startOrder < oldest->startOrder)
				{
					oldest = &voice;
				}
			}
			StopVoice(*oldest);
			oldest->channel = channel;
			return oldest;
		}

		void StopVoice(Voice& voice)
		{
			if (voice.source && voice.active)
			{
				voice.source->Stop();
				voice.source->FlushSourceBuffers();
			}
			voice.active = false;
			voice.sound = nullptr;
		}

		void StopChannel(SoundChannel channel)
		{
			for (Voice& voice : voices)
			{
				if (voice.active && voice.channel == channel)
				{
					StopVoice(voice);
				}
			}
		}

		void StopMusicVoice()
		{
			if (musicSource)
			{
				musicSource->Stop();
				musicSource->FlushSourceBuffers();
				musicSource->DestroyVoice();
				musicSource = nullptr;
			}
			music.reset();
			musicKey = AssetKey{};
		}

		AssetManager& assets;
		AssetKit persistentKit;

		Microsoft::WRL::ComPtr<IXAudio2> engine;
		IXAudio2MasteringVoice* master = nullptr;
		std::vector<Voice> voices;
		uint64_t nextOrder = 1;

		IXAudio2SourceVoice* musicSource = nullptr;
		std::unique_ptr<WavStreamReader> music;
		AssetKey musicKey{};
		std::array<std::vector<int16_t>, 3> musicBuffers{};
		size_t nextMusicBuffer = 0;
		std::array<ChannelSettings, kSoundChannelCount> channelSettings{};

		std::vector<AssetKey> backgroundKeys;
		std::vector<IXAudio2SourceVoice*> backgroundSources;
	};

	Mixer::Mixer(AssetManager& assets) : m_impl(std::make_unique<Impl>(assets)) {}

	Mixer::~Mixer() = default;

	bool Mixer::Initialize()
	{
		return m_impl->Initialize();
	}

	void Mixer::Shutdown() noexcept
	{
		m_impl->Shutdown();
	}

	void Mixer::OnModeExit()
	{
		m_impl->StopChannel(SoundChannel::Gameplay);
		m_impl->StopChannel(SoundChannel::Other);
	}

	void Mixer::PlaySound(SoundChannel channel, const AssetKey& key, const SoundParams& params)
	{
		if (!m_impl->engine)
		{
			return;
		}

		m_impl->assets.Request(m_impl->persistentKit, key);
		const SoundResource* sound = m_impl->assets.FindSound(key);
		if (!sound || !sound->IsLoaded())
		{
			return;
		}

		const DecodedSound& decoded = sound->Sound();
		WAVEFORMATEX format{};
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = decoded.channels;
		format.nSamplesPerSec = decoded.sampleRate;
		format.wBitsPerSample = 16;
		format.nBlockAlign = static_cast<WORD>(decoded.channels * 2);
		format.nAvgBytesPerSec = decoded.sampleRate * format.nBlockAlign;

		Impl::Voice* voice = m_impl->AcquireVoice(channel);
		if (voice->source && (voice->channels != format.nChannels || voice->sampleRate != format.nSamplesPerSec))
		{
			voice->source->DestroyVoice();
			voice->source = nullptr;
		}
		if (!voice->source)
		{
			voice->source = m_impl->CreateSource(format);
			voice->channels = format.nChannels;
			voice->sampleRate = format.nSamplesPerSec;
		}
		if (!voice->source)
		{
			voice->active = false;
			return;
		}

		voice->source->Stop();
		voice->source->FlushSourceBuffers();
		voice->source->SetVolume(params.volume);
		voice->source->SetFrequencyRatio(params.pitch);

		XAUDIO2_BUFFER buffer{};
		buffer.AudioBytes = static_cast<UINT32>(decoded.samples.size() * sizeof(int16_t));
		buffer.pAudioData = reinterpret_cast<const BYTE*>(decoded.samples.data());
		buffer.Flags = XAUDIO2_END_OF_STREAM;
		if (FAILED(voice->source->SubmitSourceBuffer(&buffer)) || FAILED(voice->source->Start()))
		{
			voice->active = false;
			return;
		}

		voice->sound = sound;
		voice->startOrder = m_impl->nextOrder++;
		voice->active = true;
	}

	void Mixer::PlayMusic(const AssetKey& key, const SoundParams& params)
	{
		if (!m_impl->engine)
		{
			return;
		}

		m_impl->assets.Request(m_impl->persistentKit, key);
		const MusicResource* resource = m_impl->assets.FindMusic(key);
		if (!resource)
		{
			return;
		}

		m_impl->StopMusicVoice();
		m_impl->music = resource->OpenStream();
		if (!m_impl->music)
		{
			return;
		}

		WAVEFORMATEX format{};
		format.wFormatTag = WAVE_FORMAT_PCM;
		format.nChannels = m_impl->music->Channels();
		format.nSamplesPerSec = m_impl->music->SampleRate();
		format.wBitsPerSample = 16;
		format.nBlockAlign = static_cast<WORD>(format.nChannels * 2);
		format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

		m_impl->musicSource = m_impl->CreateSource(format);
		if (!m_impl->musicSource)
		{
			m_impl->music.reset();
			return;
		}

		m_impl->musicSource->SetVolume(params.volume);
		m_impl->musicSource->SetFrequencyRatio(params.pitch);
		m_impl->musicKey = key;
		m_impl->musicSource->Start();
	}

	void Mixer::StopMusic()
	{
		m_impl->StopMusicVoice();
	}

	void Mixer::SetBackgroundSet(const std::vector<AssetKey>& keys)
	{
		if (SameKeys(keys, m_impl->backgroundKeys))
		{
			return;
		}

		for (IXAudio2SourceVoice* source : m_impl->backgroundSources)
		{
			source->Stop();
			source->FlushSourceBuffers();
			source->DestroyVoice();
		}
		m_impl->backgroundSources.clear();
		m_impl->backgroundKeys = keys;

		for (const AssetKey& key : keys)
		{
			m_impl->assets.Request(m_impl->persistentKit, key);
			const SoundResource* sound = m_impl->assets.FindSound(key);
			if (!sound || !sound->IsLoaded())
			{
				continue;
			}

			WAVEFORMATEX format{};
			format.wFormatTag = WAVE_FORMAT_PCM;
			format.nChannels = sound->Sound().channels;
			format.nSamplesPerSec = sound->Sound().sampleRate;
			format.wBitsPerSample = 16;
			format.nBlockAlign = static_cast<WORD>(format.nChannels * 2);
			format.nAvgBytesPerSec = format.nSamplesPerSec * format.nBlockAlign;

			IXAudio2SourceVoice* source = m_impl->CreateSource(format);
			if (!source)
			{
				continue;
			}

			XAUDIO2_BUFFER buffer{};
			buffer.AudioBytes = static_cast<UINT32>(sound->Sound().samples.size() * sizeof(int16_t));
			buffer.pAudioData = reinterpret_cast<const BYTE*>(sound->Sound().samples.data());
			buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
			source->SubmitSourceBuffer(&buffer);
			source->Start();
			m_impl->backgroundSources.push_back(source);
		}
	}

	void Mixer::SetChannelSettings(SoundChannel channel, const ChannelSettings& settings)
	{
		m_impl->channelSettings[ChannelIndex(channel)] = settings;
	}

	ChannelSettings Mixer::GetChannelSettings(SoundChannel channel) const
	{
		return m_impl->channelSettings[ChannelIndex(channel)];
	}

	void Mixer::Update()
	{
		if (!m_impl->musicSource || !m_impl->music)
		{
			return;
		}

		XAUDIO2_VOICE_STATE state{};
		m_impl->musicSource->GetState(&state);
		if (state.BuffersQueued >= 2)
		{
			return;
		}

		auto& slot = m_impl->musicBuffers[m_impl->nextMusicBuffer];
		m_impl->nextMusicBuffer = (m_impl->nextMusicBuffer + 1) % m_impl->musicBuffers.size();
		slot.resize(kMusicBufferSamples);

		const size_t read = m_impl->music->Read(slot.data(), slot.size());
		if (read == 0)
		{
			m_impl->music->Rewind();
			return;
		}

		XAUDIO2_BUFFER buffer{};
		buffer.AudioBytes = static_cast<UINT32>(read * sizeof(int16_t));
		buffer.pAudioData = reinterpret_cast<const BYTE*>(slot.data());
		m_impl->musicSource->SubmitSourceBuffer(&buffer);
	}

	AssetKit& Mixer::PersistentKit() noexcept
	{
		return m_impl->persistentKit;
	}
}
