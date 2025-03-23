#include "memory.h"
#include <string.h>

struct PoolNode
{
    PoolNode* next;
};

static u64 AlignForward(u64 ptr, u64 alignment)
{
    u64 mask = alignment - 1;
    return (ptr + mask) & ~mask;
}

void ArenaInit(Arena* arena, u64 size, void* user_buffer)
{
    arena->base = (u8*)user_buffer;
    arena->size = size;
    arena->offset = 0;
}

void ArenaReset(Arena* arena)
{
    arena->offset = 0;
}

void* ArenaPushSize(Arena* arena, u64 size, u64 alignment)
{
    u64 new_offset = AlignForward(arena->offset, alignment) + size;
    if (new_offset > arena->size)
    {
        return nullptr;
    }

    void* result = arena->base + arena->offset;
    arena->offset = new_offset;
    return result;
}

void* ArenaPushData(Arena* arena, void* data, u64 size, u64 alignment)
{
    void* result = ArenaPushSize(arena, size, alignment);

    if (result)
    {
        memcpy(result, data, size);
    }

    return result;
}

void PoolInit(Pool* pool, u64 chunk_size, u64 chunk_count, void* user_buffer, u64 alignment)
{
    pool->chunk_size = chunk_size;
    pool->chunk_count = chunk_count;
    pool->chunks_used = 0;
    pool->alignment = alignment;
    pool->base = (u8*)user_buffer;
    pool->free_list = pool->base;

    PoolClear(pool);
}

void PoolClear(Pool* pool)
{
    PoolNode* node = (PoolNode*)pool->base;

    for (u64 i = 0; i < pool->chunk_count; ++i)
    {
        PoolNode* next = (PoolNode*)(pool->base + i * pool->chunk_size);
        node->next = next;
        node = next;
    }

    node->next = nullptr;
    pool->free_list = pool->base;
    pool->chunks_used = 0;
}

void* PoolAllocate(Pool* pool)
{
    if (pool->chunks_used >= pool->chunk_count)
    {
        return nullptr;
    }

    // TODO: Implement alignment

    PoolNode* node = (PoolNode*)pool->free_list;
    pool->free_list = (u8*)node->next;
    pool->chunks_used++;
    return memset(node, 0, pool->chunk_size);
}

void PoolFree(Pool* pool, void* ptr)
{
    PoolNode* node = (PoolNode*)ptr;
    node->next = (PoolNode*)pool->free_list;
    pool->free_list = (u8*)node;
    pool->chunks_used--;
}