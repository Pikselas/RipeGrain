#pragma once
#include <wrl.h>
#include <xaudio2.h>

namespace RipeGrain
{
	class AudioSystem
	{
		friend class Audio;
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
	};

	AudioSystem::CoInitializer AudioSystem::CoInitializer::co_init;
}