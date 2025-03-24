#pragma once
#include "core/types.h"

// Initialize the platform layer
void PlatformInit();

// Shutdown the platform layer
void PlatformShutdown();

// Get the current time in seconds.
f32 PlatformGetTime();

// Swap the front and back buffers.
void PlatformSwapBuffers();

// Polling for events.
void PlatformPollEvents();

// Whether the platform should quit.
bool PlatformShouldQuit();