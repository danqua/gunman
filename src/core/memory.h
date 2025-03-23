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
void ArenaInit(Arena* arena, u64 size, void* user_buffer);

// Resets the offset of the arena back to zero.
void ArenaReset(Arena* arena);

// Allocates a block of memory from the arena.
void* ArenaPushSize(Arena* arena, u64 size, u64 alignment = MEMORY_DEFAULT_ALIGNMENT);

// Allocates a block of memory from the arena and copies the data from the source buffer.
void* ArenaPushData(Arena* arena, void* data, u64 size, u64 alignment = MEMORY_DEFAULT_ALIGNMENT);

struct Pool
{
    u64 chunk_size;     // Size of each chunk in bytes.
    u64 chunk_count;    // Number of chunks in the pool.
    u64 chunks_used;    // Number of chunks currently in use.
    u64 alignment;      // Alignment of each chunk.
    u8* base;           // Base address of the pool.
    u8* free_list;      // Pointer to the first free chunk in the pool.
};

// Initializes a pool allocator with a user-provided buffer.
void PoolInit(Pool* pool, u64 chunk_size, u64 chunk_count, void* user_buffer, u64 alignment = MEMORY_DEFAULT_ALIGNMENT);

// Clears the pool allocator.
void PoolClear(Pool* pool);

// Allocates a chunk of memory from the pool allocator.
void* PoolAllocate(Pool* pool);

// Frees a chunk of memory from the pool allocator.
void PoolFree(Pool* pool, void* ptr);