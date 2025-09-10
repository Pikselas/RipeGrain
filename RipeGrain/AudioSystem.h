#pragma once
#include <mutex>
#include <wrl.h>
#include <xaudio2.h>

#include "Audio.h"
#include "EngineServices.h"
#include "EngineComponent.h"

namespace RipeGrain
{
	class ExpiredVoice : private IXAudio2VoiceCallback
	{
		friend class AudioSystem;
	private:
		std::mutex& mtx;
		std::vector<ExpiredVoice*>& expire_queue;
	private:
		IXAudio2SourceVoice* voice = nullptr;
	public:
		ExpiredVoice(std::mutex& mtx , std::vector<ExpiredVoice*>& expire_queue) : mtx(mtx) , expire_queue(expire_queue){}
		~ExpiredVoice()
		{
			if (voice)
			{
				voice->Stop();
				voice->DestroyVoice();
			}
		}
		void SetVoice(IXAudio2SourceVoice* voice)
		{
			this->voice = voice;
		}
	private:
		void OnStreamEnd() override
		{
			std::lock_guard lock{ mtx };
			expire_queue.push_back(this);
		}
		void OnVoiceProcessingPassEnd() override {}
		void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {}
		void OnBufferEnd(void* pBufferContext) override {}
		void OnBufferStart(void* pBufferContext) override {}
		void OnLoopEnd(void* pBufferContext) override {}
		void OnVoiceError(void* pBufferContext, HRESULT Error) override {}
	};

	class AudioSystem : public EngineEventSubscriber, public AudioService
	{
	private:
		class CoInitializer
		{
		private:
			CoInitializer()
			{
				CoInitializeEx(nullptr, COINIT_MULTITHREADED);
			}
			~CoInitializer()
			{
				CoUninitialize();
			}
		public:
			static CoInitializer co_init;
		};
	private:
		Microsoft::WRL::ComPtr<IXAudio2> engine;
		IXAudio2MasteringVoice* pMasteringVoice = nullptr;
	private:
		std::mutex mtx;
		std::vector<ExpiredVoice*> expire_queue;
	public:
		AudioSystem()
		{
			if (FAILED(XAudio2Create(&engine)))
			{
				throw GetLastError();
			}

			if (FAILED(engine->CreateMasteringVoice(&pMasteringVoice)))
			{
				throw GetLastError();
			}
		}
		~AudioSystem()
		{
			if (pMasteringVoice != nullptr)
				pMasteringVoice->DestroyVoice();
		}
	public:
		void GetPlayBackHandle(Audio& audio , PlayBackHandle& handle) override
		{
			engine->CreateSourceVoice(&handle.voice, audio.getFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, &handle);
			handle.voice->SubmitSourceBuffer(audio.getBuffer());
		}
	public:
		void OnUpdate() override
		{
			std::lock_guard lock{ mtx };
			for (auto ptr : expire_queue)
				delete ptr;
			expire_queue.clear();
		}
		void OnEventReceive(Event& ev) override 
		{
			if (ev.event_type_index == typeid(EventPlayAudio))
			{
				auto audio_data = GetEventData<EventPlayAudio>(ev);
				IXAudio2SourceVoice* voice;

				if (audio_data.handle == nullptr)
				{
					auto cb = new ExpiredVoice(mtx , expire_queue);
					engine->CreateSourceVoice(&voice, audio_data.audio.getFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, cb);
					cb->SetVoice(voice);
				}
				else
				{
					engine->CreateSourceVoice(&voice, audio_data.audio.getFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, audio_data.handle);
					audio_data.handle->voice = voice;
				}
				voice->SubmitSourceBuffer(audio_data.audio.getBuffer());
				voice->SetFrequencyRatio(audio_data.PLAYBACK_SPEED);
				
				if(audio_data.PLAY_IMMEDIATE)
					voice->Start();
			}
		}
	};

	AudioSystem::CoInitializer AudioSystem::CoInitializer::co_init;
}