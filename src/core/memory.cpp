#include "memory.h"
#include "platform.h"

struct PoolNode
{
    PoolNode* next;
};

static u64 AlignForward(u64 ptr, u64 alignment)
{
    u64 mask = alignment - 1;
    return (ptr + mask) & ~mask;
}

void Arena_Init(Arena* arena, u64 size, void* user_buffer)
{
    arena->base = (u8*)user_buffer;
    arena->size = size;
    arena->offset = 0;
    Platform_FillMemory(arena->base, 0, size);
}

void Arena_Clear(Arena* arena)
{
    arena->offset = 0;
}

void* Arena_PushSize(Arena* arena, u64 size, u64 alignment)
{
    Platform_Assert(size > 0, "Size must be greater than 0.");
    Platform_Assert(arena->offset < arena->size, "Arena is full.");

    u64 newOffset = AlignForward(arena->offset, alignment) + size;
    if (newOffset > arena->size)
    {
        return nullptr;
    }

    void* result = arena->base + arena->offset;
    arena->offset = newOffset;
    return result;
}

void* Arena_PushData(Arena* arena, void* data, u64 size, u64 alignment)
{
    void* result = Arena_PushSize(arena, size, alignment);

    if (result)
    {
        Platform_CopyMemory(result, data, size);
    }

    return result;
}

void Pool_Init(Pool* pool, u64 chunkSize, u64 chunkCount, void* buffer, u64 alignment)
{
    pool->chunkSize = chunkSize;
    pool->chunkCount = chunkCount;
    pool->chunksUsed = 0;
    pool->alignment = alignment;
    pool->base = (u8*)buffer;
    pool->freeList = pool->base;

    Pool_Clear(pool);
}

void Pool_Clear(Pool* pool)
{
    PoolNode* node = (PoolNode*)pool->base;

    for (u64 i = 0; i < pool->chunkCount; ++i)
    {
        PoolNode* next = (PoolNode*)(pool->base + i * pool->chunkSize);
        node->next = next;
        node = next;
    }

    node->next = nullptr;
    pool->freeList = pool->base;
    pool->chunksUsed = 0;
}

void* Pool_Alloc(Pool* pool)
{
    if (pool->chunksUsed >= pool->chunkCount)
    {
        return nullptr;
    }

    // TODO: Implement alignment

    PoolNode* node = (PoolNode*)pool->freeList;
    pool->freeList = (u8*)node->next;
    pool->chunksUsed++;
    return Platform_ClearMemory(node, pool->chunkSize);
}

void Pool_Free(Pool* pool, void* ptr)
{
    PoolNode* node = (PoolNode*)ptr;
    node->next = (PoolNode*)pool->freeList;
    pool->freeList = (u8*)node;
    pool->chunksUsed--;
}