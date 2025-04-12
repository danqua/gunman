#pragma once
#include "core/types.h"

// Initialize the platform layer
void PlatformInit();

// Shutdown the platform layer
void PlatformShutdown();

// Get the current time in seconds.
f64 PlatformGetTime();

// Swap the front and back buffers.
void PlatformSwapBuffers();

// Polling for events.
void PlatformPollEvents();

// Whether the platform should quit.
bool PlatformShouldQuit();

// Allocate memory.
void* PlatformAllocateMemory(u64 size);

// Free memory.
void PlatformFreeMemory(void* memory);

// Copy memory.
void* PlatformCopyMemory(void* dest, const void* src, u64 size);

// Clear memory.
void* PlatformClearMemory(void* dest, u64 size);

// Log a message to the console.
void PlatformLog(const char* message, ...);