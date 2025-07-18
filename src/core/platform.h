#pragma once
#include "core/types.h"

#define Kilobytes(x) ((x) * 1024)
#define Megabytes(x) (Kilobytes(x) * 1024)
#define Gigabytes(x) (Megabytes(x) * 1024)

// Assert function for debugging.
void Platform_Assert(b32 condition, const char* message, ...);

// Allocate memory for a given size.
void* Platform_Alloc(u64 size);

// Free allocated memory.
void Platform_Free(void* memory);

// Clears the memory to zero.
void* Platform_ClearMemory(void* memory, u64 size);

void* ClearMemory(void* ptr, u64 size);

// Fills the memory with a given value.
void* Platform_FillMemory(void* memory, s32 value, u64 size);

// Copies memory from source to destination.
void* Platform_CopyMemory(void* dest, const void* src, u64 size);

// Initializes the window with a title and dimensions.
b32 Platform_InitWindow(const char* title, s32 width, s32 height);

// Closes the window and cleans up resources.
void Platform_CloseWindow();

// Returns the width of the window.
s32 Platform_GetWindowWidth();

// Returns the height of the window.
s32 Platform_GetWindowHeight();

// Returns true if the window should close.
b32 Platform_WindowShouldClose();

// Processes input events.
void Platform_PollEvents();

// Swaps the front and back buffers.
void Platform_SwapBuffers();

// Returns the time in seconds.
f64 Platform_GetTime();

// Returns a formatted string.
const char* TextFormat(const char* format, ...);

struct FileHandle
{
    void* handle;
    u64 size;
};

enum FileMode
{
    FileMode_Read,
    FileMode_Write
};

// Opens a file from disk.
FileHandle Platform_OpenFile(const char* filename, FileMode mode);

// Closes a file handle.
void Platform_CloseFile(FileHandle* fileHandle);

// Reads some amount of data.
void Platform_ReadDataFromFile(FileHandle* fileHandle, void* buffer, u64 size);

// Logging
void Platform_LogInfo(const char* message, ...);
void Platform_LogError(const char* message, ...);
void Platform_LogWarning(const char* message, ...);
