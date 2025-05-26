#include "audio.h"
#include <SDL3/SDL.h>

#define MAX_AUDIO_CLIPS 64
#define MAX_AUDIO_INSTANCES 128

struct AudioClip
{
    u8* buffer;
    u32 length;
    SDL_AudioSpec spec;
};

struct AudioInstance
{
    b32 loop;
    s32 count;
    AudioClip* clip;
    SDL_AudioStream* stream;
};

struct AudioManager
{
    Pool instancePool;
    AudioInstance* instances[MAX_AUDIO_INSTANCES];
    u32 instanceCount;

    Pool clipPool;
    AudioClip* audioClips[MAX_AUDIO_CLIPS];
    u32 clipCount;

    SDL_AudioDeviceID device;
};

static AudioManager manager;

void Audio_Init(Arena* arena)
{
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    manager.device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    manager.clipCount = 1;
    manager.instanceCount = 1;

    void* instancePoolMemory = ArenaPushArray(arena, AudioInstance, MAX_AUDIO_INSTANCES);
    Pool_Init(&manager.instancePool, sizeof(AudioInstance), MAX_AUDIO_INSTANCES, instancePoolMemory);

    void* audioClipMemory = ArenaPushArray(arena, AudioClip, MAX_AUDIO_CLIPS);
    Pool_Init(&manager.clipPool, sizeof(AudioClip), MAX_AUDIO_CLIPS, audioClipMemory);
}

void Audio_Shutdown()
{
    SDL_CloseAudioDevice(manager.device);
}

void Audio_Update()
{
    for (u32 i = 1; i < manager.instanceCount; ++i)
    {
        AudioInstance* instance = manager.instances[i];
        AudioClip* clip = instance->clip;
        if (SDL_GetAudioStreamQueued(instance->stream) < clip->length)
        {
            if (instance->count > 1 && !instance->loop)
            {
                Audio_StopInstance(i);
                continue; // Skip to the next instance if this one is done
            }
            SDL_PutAudioStreamData(instance->stream, clip->buffer, clip->length);
            instance->count++;
        }
    }
}

AudioId Audio_LoadFromFile(const char* filename)
{
    AudioClip* clip = (AudioClip*)Pool_Alloc(&manager.clipPool);
    SDL_LoadWAV(filename, &clip->spec, &clip->buffer, &clip->length);

    if (!clip->buffer)
    {
        return 0;
    }

    manager.audioClips[manager.clipCount] = clip;
    return manager.clipCount++;
}

void Audio_Free(AudioId audio)
{
    if (audio != 0)
    {
        AudioClip* clip = manager.audioClips[audio];
        SDL_free(clip->buffer);
        Pool_Free(&manager.clipPool, clip);

        for (u32 i = 0; i < manager.clipCount; ++i)
        {
            if (manager.audioClips[i] == clip)
            {
                manager.audioClips[i] = manager.audioClips[manager.clipCount - 1];
                manager.clipCount--;
                break;
            }
        }
    }
}

AudioInstanceId Audio_PlayClip(AudioId audio, f32 volume, b32 loop)
{
    if (manager.instanceCount >= MAX_AUDIO_INSTANCES)
    {
        return 0; // No more instances can be created
    }

    if (audio == 0 || audio >= manager.clipCount)
    {
        return 0; // Invalid audio ID
    }

    AudioClip* clip = manager.audioClips[audio];
    AudioInstance* instance = (AudioInstance*)Pool_Alloc(&manager.instancePool);
    instance->clip = clip;
    instance->stream = SDL_CreateAudioStream(&clip->spec, NULL);
    instance->loop = loop;
    SDL_SetAudioStreamGain(instance->stream, volume);
    SDL_BindAudioStream(manager.device, instance->stream);
    manager.instances[manager.instanceCount] = instance;
    return manager.instanceCount++;
}

void Audio_StopInstance(AudioInstanceId instanceId)
{
    if (instanceId >= manager.instanceCount || instanceId == 0)
    {
        return; // Invalid instance ID
    }

    AudioInstance* instance = manager.instances[instanceId];

    SDL_UnbindAudioStream(instance->stream);
    SDL_DestroyAudioStream(instance->stream);
    Pool_Free(&manager.instancePool, instance);

    for (u32 i = 0; i < manager.instanceCount; ++i)
    {
        if (manager.instances[i] == instance)
        {
            manager.instances[i] = manager.instances[manager.instanceCount - 1];
            manager.instanceCount--;
            break;
        }
    }
}