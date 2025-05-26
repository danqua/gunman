#pragma once
#include "core/types.h"
#include "core/memory.h"

typedef u32 AudioId;
typedef u32 AudioInstanceId;

// Initializes the audio system.
void Audio_Init(Arena* arena);

// Shuts down the audio system.
void Audio_Shutdown();

// Updates the audio system, needs to be called every frame.
void Audio_Update();

// Loads an audio clip from file.
AudioId Audio_LoadFromFile(const char* filename);

// Frees an audio clip from the audio system.
void Audio_Free(AudioId audio);

// Plays an audio clip with the specified volume and loop settings.
AudioInstanceId Audio_PlayClip(AudioId audio, f32 volume = 1.0f, b32 loop = 1);

// Stops an audio instance.
void Audio_StopInstance(AudioInstanceId instance);
