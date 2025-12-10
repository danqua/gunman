#pragma once
#include "core/types.h"

template <typename type>
struct Array
{
    Array() : data(nullptr), size(0), capacity(0)
    {
    }

    ~Array()
    {
        if (data)
        {
            Platform_Free(data);
        }
    }

    // operator overload
    type& operator[](s32 index)
    {
        return data[index];
    }

    void Add(type value)
    {
        if (size >= capacity)
        {
            s32 newCapacity = capacity == 0 ? 1 : capacity * 2;
            type* newData = (type*)Platform_Alloc(sizeof(type) * newCapacity);
            if (data)
            {
                for (s32 i = 0; i < size; ++i)
                {
                    newData[i] = data[i];
                }
                Platform_Free(data);
            }
            data = newData;
            capacity = newCapacity;
        }
        data[size++] = value;
    }

    void RemoveAt(s32 index)
    {
        if (index < 0 || index >= size)
            return;

        for (s32 i = index; i < size - 1; ++i)
        {
            data[i] = data[i + 1];
        }
        --size;
    }

    void RemoveAndSwap(s32 index)
    {
        if (index < 0 || index >= size)
            return;

        data[index] = data[size - 1];
        --size;
    }

    void Clear()
    {
        size = 0;
    }


    type* data;
    s32 size;
    s32 capacity;
};