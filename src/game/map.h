#pragma once
#include "core/types.h"
#include <vector>

struct LineSegment {
    glm::vec2 v1;
    glm::vec2 v2;
    s32 frontSector;
    s32 backSector;
    u32 flags;
};

struct Edge {
    s32 seg;
    bool reversed;

    bool operator==(const Edge& other) const {
        return (seg == other.seg) && (reversed == other.reversed);
    }
};

struct Sector {
    s32 firstEdge;
    s32 edgeCount;
    f32 floorHeight;
    f32 ceilingHeight;
};