#pragma once
#include <wrl.h>
#include <xaudio2.h>
#include <memory>
#include <filesystem>

#include "AudioSystem.h"

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

namespace RipeGrain
{
    class Audio : private IXAudio2VoiceCallback
    {
    private:
        bool isStopped = true;
    private:
        std::unique_ptr<BYTE[]> audio_data;
        XAUDIO2_BUFFER buffer = { 0 };
        WAVEFORMATEXTENSIBLE wfx = { 0 };
    private:
        IXAudio2SourceVoice* voice = nullptr;
    public:
        Audio(std::filesystem::path path, AudioSystem& system)
        {
            HANDLE hFile = CreateFile(
                path.string().c_str(),
                GENERIC_READ,
                FILE_SHARE_READ,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);
            if (INVALID_HANDLE_VALUE == hFile)
                throw HRESULT_FROM_WIN32(GetLastError());

            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
                throw HRESULT_FROM_WIN32(GetLastError());

            DWORD dwChunkSize;
            DWORD dwChunkPosition;
            //check the file type, should be fourccWAVE or 'XWMA'
            FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
            DWORD filetype;
            ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
            if (filetype != fourccWAVE)
                throw S_FALSE;

            FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
            ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

            //fill out the audio data buffer with the contents of the fourccDATA chunk
            FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
            audio_data = std::make_unique<BYTE[]>(dwChunkSize);
            ReadChunkData(hFile, audio_data.get(), dwChunkSize, dwChunkPosition);

            buffer.AudioBytes = dwChunkSize;  //size of the audio buffer in bytes
            buffer.pAudioData = audio_data.get();  //buffer containing audio data
            buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

            system.engine->CreateSourceVoice(&voice, getFormat(), 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, NULL, NULL);
        }
        ~Audio()
        {
            if (voice != nullptr)
            {
                voice->Stop();
                voice->DestroyVoice();
            }
        }
    public:
        XAUDIO2_BUFFER* getBuffer()
        {
            return &buffer;
        }
        WAVEFORMATEX* getFormat()
        {
            return (WAVEFORMATEX*)&wfx;
        }
    private:
        static HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, DWORD& dwChunkDataPosition)
        {
            HRESULT hr = S_OK;
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
                return HRESULT_FROM_WIN32(GetLastError());

            DWORD dwChunkType;
            DWORD dwChunkDataSize;
            DWORD dwRIFFDataSize = 0;
            DWORD dwFileType;
            DWORD bytesRead = 0;
            DWORD dwOffset = 0;

            while (hr == S_OK)
            {
                DWORD dwRead;
                if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());

                if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
                    hr = HRESULT_FROM_WIN32(GetLastError());

                switch (dwChunkType)
                {
                case fourccRIFF:
                    dwRIFFDataSize = dwChunkDataSize;
                    dwChunkDataSize = 4;
                    if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
                        hr = HRESULT_FROM_WIN32(GetLastError());
                    break;

                default:
                    if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
                        return HRESULT_FROM_WIN32(GetLastError());
                }

                dwOffset += sizeof(DWORD) * 2;

                if (dwChunkType == fourcc)
                {
                    dwChunkSize = dwChunkDataSize;
                    dwChunkDataPosition = dwOffset;
                    return S_OK;
                }

                dwOffset += dwChunkDataSize;

                if (bytesRead >= dwRIFFDataSize) return S_FALSE;

            }

            return S_OK;
        }
        static HRESULT ReadChunkData(HANDLE hFile, void* buffer, DWORD buffersize, DWORD bufferoffset)
        {
            HRESULT hr = S_OK;
            if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
                return HRESULT_FROM_WIN32(GetLastError());
            DWORD dwRead;
            if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
                hr = HRESULT_FROM_WIN32(GetLastError());
            return hr;
        }
    private:
        void OnStreamEnd() override
        {
            isStopped = true;
        }
        void OnVoiceProcessingPassEnd() override {}
        void OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {}
        void OnBufferEnd(void* pBufferContext) override {}
        void OnBufferStart(void* pBufferContext) override {}
        void OnLoopEnd(void* pBufferContext) override {}
        void OnVoiceError(void* pBufferContext, HRESULT Error) override {}
    public:
        void Play()
        {
            voice->SubmitSourceBuffer(getBuffer());
            voice->Start(0);
            isStopped = false;
        }
        void Stop()
        {
            isStopped = true;
            voice->Stop();
        }
        void SetLoop(int loop_count = XAUDIO2_LOOP_INFINITE)
        {
            buffer.LoopBegin = 0;
            buffer.LoopLength = 0;
            buffer.LoopCount = loop_count;
        }
        void SetSpeed(float speed)
        {
            voice->SetFrequencyRatio(speed);
        }
        bool IsStopped() const
        {
            return isStopped;
        }
    };
}