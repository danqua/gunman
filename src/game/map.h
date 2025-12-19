#pragma once
#include "core/types.h"
#include <vector>

struct LineSegment {
    glm::vec2 v1;
    glm::vec2 v2;
    s32 frontSector;
    s32 backSector;
};

struct Edge {
    s32 seg;
    bool reversed;

    bool operator==(const Edge& other) const {
        return (seg == other.seg) && (reversed == other.reversed);
    }
};

struct SectorGroup {
    s32 firstEdge;
    s32 edgeCount;
};

struct Sector {
    s32 firstGroup;
    s32 groupCount;
    f32 floorHeight;
    f32 ceilingHeight;
};