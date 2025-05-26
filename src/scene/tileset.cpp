#include "level.h"

void Tileset_GetTileUVs(const Tileset* tileset, s32 tileId, glm::vec2* outMin, glm::vec2* outMax)
{
    s32 tileWidth = tileset->tileWidth + tileset->spacing * 2;
    s32 tileHeight = tileset->tileHeight + tileset->spacing * 2;
    s32 x = (tileId % tileset->columns) * tileWidth;
    s32 y = (tileId / tileset->columns) * tileHeight;

    f32 tTileWidth = (f32)tileWidth / tileset->image.width;
    f32 tTileHeight = (f32)tileHeight / tileset->image.height;
    f32 u = (f32)x / tileset->image.width;
    f32 v = 1.0f - (f32)y / tileset->image.height;
    f32 s = u + tTileWidth;
    f32 t = v - tTileHeight;

    outMin->x = u;
    outMin->y = t;
    outMax->x = s;
    outMax->y = v;
}