#pragma once
#include "core/types.h"

#define MEMORY_DEFAULT_ALIGNMENT 8

struct Arena
{
    u8* base;   // Base address of the arena.
    u64 size;   // Size of the arena in bytes.
    u64 offset; // Current offset in the arena.
};

// Initializes an arena with a user-provided buffer.
void Arena_Init(Arena* arena, u64 size, void* buffer);

// Resets the offset of the arena back to zero.
void Arena_Clear(Arena* arena);

// Allocates a block of memory from the arena.
void* Arena_PushSize(Arena* arena, u64 size, u64 alignment = MEMORY_DEFAULT_ALIGNMENT);

// Allocates a block of memory with given type.
#define ArenaPushType(arena, type) (type*)Arena_PushSize(arena, sizeof(type), MEMORY_DEFAULT_ALIGNMENT)

// Allocates a block of memory with given type and count.
#define ArenaPushArray(arena, type, count) (type*)Arena_PushSize(arena, sizeof(type) * (count), MEMORY_DEFAULT_ALIGNMENT)

struct Pool
{
    u64 chunkSize;     // Size of each chunk in bytes.
    u64 chunkCount;    // Number of chunks in the pool.
    u64 chunksUsed;    // Number of chunks currently in use.
    u64 alignment;     // Alignment of each chunk.
    u8* base;          // Base address of the pool.
    u8* freeList;      // Pointer to the first free chunk in the pool.
};

// Initializes a pool allocator with a user-provided buffer.
void Pool_Init(Pool* pool, u64 chunkSize, u64 chunkCount, void* buffer, u64 alignment = MEMORY_DEFAULT_ALIGNMENT);

// Clears the pool allocator.
void Pool_Clear(Pool* pool);

// Allocates a chunk of memory from the pool allocator.
void* Pool_Alloc(Pool* pool);

// Frees a chunk of memory from the pool allocator.
void Pool_Free(Pool* pool, void* ptr);