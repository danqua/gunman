#include "core/platform.h"
#include "core/audio.h"
#include "core/input.h"
#include "core/math.h"
#include "core/memory.h"
#include "core/containers.h"
#include "renderer/rhi.h"
#include "renderer/camera.h"
#include "renderer/image.h"
#include "renderer/renderer_2d.h"
#include "renderer/renderer.h"
#include "renderer/sprite.h"

#include "scene/level.h"

#include "scene/map.h"
#include "game/game.h"
#include "core/assets.h"
#include "scene/camera_controller.h"

#include <glm/gtc/matrix_transform.hpp>
#include "scene/level_builder.h"



#include <vector>
#include <unordered_map>

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>


#define HORZ_DOOR 62
#define VERT_DOOR 63
#define KEY 55
#define LIGHT 54
#define PLAYER_SPAWN 53


Tileset LoadTileset(const char* filename)
{
    using json = nlohmann::json;
    std::ifstream file(filename);

    Tileset tileset = {};

    if (!file)
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        return tileset;
    }

    json data;
    file >> data;

    tileset.tileWidth = data.value("tilewidth", 0);
    tileset.tileHeight = data.value("tileheight", 0);
    tileset.spacing = data.value("spacing", 0);
    tileset.tileCount = data.value("tilecount", 0);
    tileset.columns = data.value("columns", 0);

    std::string imagePath = data.value("image", "");
    if (!imagePath.empty())
    {
        tileset.image = Image_LoadFromFile(imagePath.c_str());
        if (tileset.image.pixels == nullptr)
        {
            std::cerr << "Failed to load image: " << imagePath << std::endl;
            return tileset;
        }
    }
    else
    {
        std::cerr << "No image specified in tileset: " << filename << std::endl;
        return tileset;
    }
    return tileset;
}

std::vector<std::string> LoadTileset(const std::string& filename)
{
    using json = nlohmann::json;

    std::vector<std::string> textures;
    std::ifstream file(filename);

    if (!file)
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        return textures;
    }

    json data;
    file >> data;

    s32 tileWidth = data.value("tilewidth", -1);
    s32 tileHeight = data.value("tileheight", -1);
    s32 tileCount = data.value("tilecount", -1);

    textures.resize(tileCount);

    if (data.contains("tiles") && data["tiles"].is_array())
    {
        for (const auto& tile : data["tiles"])
        {
            int id = tile.value("id", -1);
            std::string imagePath = tile.value("image", "");
            if (id >= 0 && id < tileCount)
            {
                textures[id] = imagePath;
            }
        }
    }

    file.close();
    return textures;
}

struct LevelLayer
{
    s32 width;
    s32 height;
    std::vector<s32> data;
};

enum LevelEntityType
{
    LevelEntityType_None,
    LevelEntityType_PlayerSpawn,
    LevelEntityType_Key,
    LevelEntityType_Light,
    LevelEntityType_Door,
    LevelEntityType_Prop
};

enum Direction
{
    Direction_North,
    Direction_East,
    Direction_South,
    Direction_West
};

enum LevelPropType
{
    LevelProp_VendingMachine,
    LevelProp_TrashBin,
    LevelProp_PotPlant
};

struct LevelProp
{
    LevelPropType type;
    Direction direction;
};

struct LevelKey
{
};

struct LevelEnemy
{

};

struct LevelDoor
{
    s32 keyId;
    DoorAxis axis;
};

struct LevelLight
{
    char color[6];
    f32 intensity;
    f32 range;
};

struct LevelEntity
{
    s32 id;
    s32 x, y;
    v2 position;
    f32 rotation;
    s32 tileId;
    LevelEntityType type;
    union
    {
        LevelKey key;
        LevelDoor door;
        LevelLight light;
        LevelProp prop;
    };
};

struct LevelLoader
{
    s32 width;
    s32 height;
    Tileset tileset;
    std::unordered_map<std::string, LevelLayer> layers;
    std::vector<std::string> textures;
    std::vector<Light> lights;
    std::vector<LevelEntity> entities;
};



LevelLoader LoadLevel(const std::string& filename)
{
    using json = nlohmann::json;

    LevelLoader result = {};
    
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Could not open file: " << filename << std::endl;
        return result;
    }

    json data;
    file >> data;
    file.close();

    s32 levelWidth = data.value("width", -1);
    s32 levelHeight = data.value("height", -1);

    result.width = levelWidth;
    result.height = levelHeight;

    std::string tilesetPath;

    if (data.contains("tilesets") && data["tilesets"].is_array())
    {
        for (const auto& tileset : data["tilesets"])
        {
            int firstgid = tileset.value("firstgid", -1);
            std::string source = tileset.value("source", "");
            tilesetPath = source;
            break;
        }
    }

    result.tileset = LoadTileset(tilesetPath.c_str());


    if (data.contains("layers") && data["layers"].is_array())
    {
        for (const auto& layer : data["layers"])
        {
            LevelLayer levelLayer = {};
            std::string name = layer.value("name", "");
            levelLayer.width = layer.value("width", -1);
            levelLayer.height = layer.value("height", -1);

            if (name == "entities")
            {
                const auto& entityLayers = layer["layers"];
                for (const auto& entityLayer : entityLayers)
                {
                    std::string layerName = entityLayer.value("name", "");

                    if (layerName == "player_spawn")
                    {
                        const auto& object = entityLayer["objects"][0];

                        f32 x = object.value("x", 0) / (f32)result.tileset.tileWidth;
                        f32 y = object.value("y", 0) / (f32)result.tileset.tileHeight;

                        LevelEntity entity = {};
                        entity.type = LevelEntityType_PlayerSpawn;
                        entity.position.x = x;
                        entity.position.y = y;
                        entity.rotation = object.value("rotation", 0.0f);
                        result.entities.push_back(entity);
                    }
                    else if (layerName == "doors")
                    {
                        const auto& objects = entityLayer["objects"];

                        for (const auto& object : objects)
                        {
                            s32 tileX = (u32)object.value("x", 0) / result.tileset.tileWidth;
                            s32 tileY = (u32)object.value("y", 0) / result.tileset.tileHeight;


                            LevelEntity entity = {};
                            entity.type = LevelEntityType_Door;
                            entity.position.x = (f32)tileX;
                            entity.position.y = (f32)tileY;

                            if (object.contains("properties") && object["properties"].is_array())
                            {
                                for (const auto& property : object["properties"])
                                {
                                    std::string name = property.value("name", "");

                                    if (name == "axis")
                                    {
                                        s32 axis = property.value("value", 0);
                                        entity.door.axis = (DoorAxis)axis;
                                        s32 waitHere = 0;
                                    }
                                }
                            }

                            result.entities.push_back(entity);
                        }
                    }
                    else if (layerName == "lights")
                    {
                        const auto& objects = entityLayer["objects"];

                        for (const auto& object : objects)
                        {
                            f32 x = object.value("x", 0) / (f32)result.tileset.tileWidth;
                            f32 y = object.value("y", 0) / (f32)result.tileset.tileHeight;


                            LevelEntity entity = {};
                            entity.type = LevelEntityType_Light;
                            entity.position.x = x;
                            entity.position.y = y;

                            strncpy(entity.light.color, "fff6c7", 6);
                            entity.light.range = 4.0f;
                            entity.light.intensity = 0.7f;
                            

                            if (object.contains("properties") && object["properties"].is_array())
                            {
                                for (const auto& property : object["properties"])
                                {
                                    if (property["name"] == "color")
                                    {
                                        std::string colorValue = property.value("value", "#fffff6c7");
                                        colorValue.copy(entity.light.color, 6, 3);
                                    }
                                    else if (property["name"] == "range")
                                    {
                                        entity.light.range = property.value("value", 4.0f);
                                    }
                                    else if (property["name"] == "intensity")
                                    {
                                        entity.light.intensity = property.value("value", 0.7f);
                                    }
                                }
                            }

                            result.entities.push_back(entity);
                        }
                    }
                    else if (layerName == "props")
                    {
                        const auto& objects = entityLayer["objects"];

                        for (const auto& object : objects)
                        {
                            f32 x = object.value("x", 0) / (f32)result.tileset.tileWidth;
                            f32 y = object.value("y", 0) / (f32)result.tileset.tileHeight;

                            LevelEntity entity = {};
                            entity.type = LevelEntityType_Prop;
                            entity.position.x = glm::floor(x);
                            entity.position.y = glm::floor(y);

                            if (object.contains("properties") && object["properties"].is_array())
                            {
                                for (const auto& property : object["properties"])
                                {
                                    std::string name = property.value("name", "");
                                    if (name == "facing")
                                    {
                                        entity.prop.direction = (Direction)property.value("value", 0);
                                    }
                                    else if (name == "type")
                                    {
                                        entity.prop.type = property.value("value", LevelProp_VendingMachine);
                                    }
                                }
                            }

                            if (entity.prop.type == LevelProp_PotPlant)
                            {
                                entity.position.x = x;
                                entity.position.y = y;
                            }

                            result.entities.push_back(entity);
                        }
                    }
                }
            }


            levelLayer.data.resize(levelLayer.width * levelLayer.height);

            if (layer.contains("data") && layer["data"].is_array())
            {
                for (size_t i = 0; i < layer["data"].size(); ++i)
                {
                    levelLayer.data[i] = layer["data"][i].get<int>();
                }
            }

            result.layers[name] = levelLayer;
        }
    }

    return result;
}

static ShaderId defaultShader;
static Mesh meshes[1024];
static u32 meshCount;
static Material materials[1024];
static u32 materialCount;

struct Renderable
{
    Mesh* mesh;
    Material* material;
};

Renderable ASD(Level* level, LevelLoader& loader, Arena* transientArena)
{
    s32 surfaceCount = 0;
    Surface* surfaces = CreateLevelSurfaces(level, &loader.tileset, transientArena, &surfaceCount);

    // Get lights
    Atlas atlas = CreateAtlas(2048, 2048, 16, 16, 1, transientArena);
    ComputeLightmapCoordinates(surfaces, surfaceCount, level, &atlas);

    b32 useLightmap = false;

    if (useLightmap)
    {
        std::vector<Light> lights;

        for (const LevelEntity& levelEntity : loader.entities)
        {
            if (levelEntity.type == LevelEntityType_Light)
            {
                Light light = {};
                light.position = v3(levelEntity.position, 0.0f);
                light.intensity = levelEntity.light.intensity;
                light.range = levelEntity.light.range;

                std::string hexValue = levelEntity.light.color;
                light.color.r = std::stoi(hexValue.substr(0, 2), nullptr, 16) / 255.0f;
                light.color.g = std::stoi(hexValue.substr(2, 2), nullptr, 16) / 255.0f;
                light.color.b = std::stoi(hexValue.substr(4, 2), nullptr, 16) / 255.0f;

                lights.push_back(light);
            }
        }

        ComputeLightmap(&atlas, level, surfaces, surfaceCount, lights.data(), (s32)lights.size());
        Image_SaveToFile("test.png", &atlas.image);
        atlas.image = Image_LoadFromFile("test.png");
    }
    else
    {
        atlas.image = Image_LoadFromFile("test.png");
    }

    TextureId lightmapTexture = RHI_CreateTexture(atlas.image.pixels, atlas.image.width, atlas.image.height, TextureFilter_Linear);

    Image* tilesetImage = &loader.tileset.image;

    TextureId levelTexture = RHI_CreateTexture(tilesetImage->pixels, tilesetImage->width, tilesetImage->height, TextureFilter_Nearest);

    Material* material = &materials[materialCount++];
    material->diffuseTexture = levelTexture;
    material->diffuseColor = v3(1.0f);
    material->lightmapTexture = lightmapTexture;
    material->useLightmap = useLightmap;
    material->shader = defaultShader;
    
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    for (s32 i = 0; i < surfaceCount; ++i)
    {
        Surface* surface = &surfaces[i];
        u32 indexCount = (u32)(vertices.size());

        for (u32 i = 0; i < 4; i++)
        {
            Vertex vertex = {};
            vertex.position = surface->vertices[i];
            vertex.normal = surface->normal;
            vertex.texCoord0 = surface->texcoords[i];
            vertex.texCoord1 = surface->lightmap[i];
            vertices.push_back(vertex);
        }

        indices.insert(indices.end(), {
            indexCount + 0, indexCount + 1, indexCount + 2,
            indexCount + 0, indexCount + 2, indexCount + 3 });
    }

    meshes[meshCount] = CreateMesh(vertices.data(), (u32)vertices.size(), indices.data(), (u32)indices.size());
        
    Renderable renderable = {};
    renderable.mesh = &meshes[meshCount++];
    renderable.material = material;

    Arena_Clear(transientArena);

    return renderable;
}

#include <fstream>
#include <sstream>

Mesh LoadMesh(const char* filename)
{
    std::ifstream fs(filename);

    if (!fs.is_open())
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }

    std::string line;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;

    std::vector<Vertex> verticesData;
    std::vector<u32> indices;

    while (std::getline(fs, line))
    {
        if (line.find("v ") != std::string::npos)
        {
            std::istringstream iss(line.substr(2));
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            vertices.push_back(vertex);
        }
        else if (line.find("vn ") != std::string::npos)
        {
            std::istringstream iss(line.substr(3));
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (line.find("vt ") != std::string::npos)
        {
            std::istringstream iss(line.substr(3));
            glm::vec2 texcoord;
            iss >> texcoord.x >> texcoord.y;
            texcoords.push_back(texcoord);
        }
        else if (line.find("f ") != std::string::npos)
        {
            // f v/vn/vt v/vn/vt v/vn/vt
            std::istringstream iss(line.substr(2));
            std::string vertexData;
            while (iss >> vertexData)
            {
                std::istringstream vertexStream(vertexData);
                std::string vertexIndex, normalIndex, texcoordIndex;
                std::getline(vertexStream, vertexIndex, '/');
                std::getline(vertexStream, texcoordIndex, '/');
                std::getline(vertexStream, normalIndex, '/');

                u32 vIndex = std::stoi(vertexIndex) - 1;
                u32 vtIndex = std::stoi(texcoordIndex) - 1;
                u32 vnIndex = std::stoi(normalIndex) - 1;

                Vertex vertex = {};
                vertex.position = vertices[vIndex];
                vertex.normal = normals[vnIndex];
                vertex.texCoord0 = texcoords[vtIndex];
                verticesData.push_back(vertex);
                indices.push_back((u32)verticesData.size() - 1);
            }
        }
    }

    fs.close();

    Mesh mesh = CreateMesh(verticesData.data(), (u32)verticesData.size(), indices.data(), (u32)indices.size());
    return mesh;
}

int main(int argc, char** argv)
{
    Platform_InitWindow("Gunman", 1920, 1080);
    //Platform_PlayAudioClip(audio, true);

    void* memory = Platform_Alloc(Megabytes(64));
    Platform_Assert(memory, "Failed to allocate memory.");

    Arena permanentStorage = {};
    Arena_Init(&permanentStorage, Megabytes(64), memory);


    void* tMemory = Platform_Alloc(Megabytes(128));
    Arena transientStorage = {};
    Arena_Init(&transientStorage, Megabytes(128), tMemory);
  

    RHI_Init();
    Renderer_Init(&permanentStorage);
    Renderer2D_Init(&permanentStorage);
    
    Renderer_SetSize(640, 360);


    Audio_Init(&permanentStorage);

    defaultShader = Asset_LoadShader("shaders/default.vs", "shaders/default.fs");

    Material levelMaterial = {};
    levelMaterial.shader = defaultShader;
    levelMaterial.diffuseTexture = Asset_LoadTexture("textures/test.png");
    levelMaterial.diffuseColor = v3(1.0f, 1.0f, 1.0f);

    CameraController cameraController = CreateDefaultCameraController();

    void* gameMemory = Platform_Alloc(Megabytes(16));
    

    Level level = {};

    LevelLoader levelLoader = LoadLevel("dummy.tmj");
    Level_Init(&level, levelLoader.width, levelLoader.height, &permanentStorage);

    
    for (const auto& [name, layer] : levelLoader.layers)
    {
        for (s32 y = 0; y < layer.height; ++y)
        {
            for (s32 x = 0; x < layer.width; ++x)
            {
                s32 data = layer.data[x + y * layer.width];

                Tile* tile = Level_GetTileAt(&level, x, y);
                tile->data |= data;
                if (data == 0) continue;

                if (name == "floor")
                {
                    tile->solid = false;
                    tile->floor = data - 1;
                }
                else if (name == "ceiling")
                {
                    tile->solid = false;
                    tile->ceiling = data - 1;
                }
                else if (name == "walls")
                {
                    tile->solid = true;
                    tile->wall = data - 1;
                }
                else
                {
                    continue; // Skip unknown layers
                }
            }
        }
    }

    Renderable renderable = ASD(&level, levelLoader, &transientStorage);

    Game_Init(gameMemory, Megabytes(16));

    GameContext* context = game->context;

    context->level = &level;
    {
        Entity* levelEntity = SpawnEntity(context, EntityType_None, v3(0.0f));
        levelEntity->meshRenderer.enabled = true;
        levelEntity->meshRenderer.mesh = renderable.mesh;
        levelEntity->meshRenderer.material = renderable.material;
    }

    Mesh keyMesh = LoadMesh("models/key.obj");

    Material* defaultMaterial = renderable.material;
    materialDefault = *defaultMaterial;

    Material* doorMaterial = &materials[materialCount++];
    *doorMaterial = *defaultMaterial;
    doorMaterial->useLightmap = false;

    meshDoorLower = LoadMesh("models/door_lower.obj");
    meshDoorUpper = LoadMesh("models/door_upper.obj");

    Material keyMaterial = levelMaterial;
    
    std::vector<Light> lights;


    // Vending Machine
    Mesh vendingMachineMesh = LoadMesh("models/vending_machine.obj");
    Mesh trashBinMesh = LoadMesh("models/trash_bin.obj");
    Mesh potPlantMesh = LoadMesh("models/pot_plant.obj");
    Material propsMaterial = {};
    propsMaterial.diffuseColor = v3(1.0f, 1.0f, 1.0f);
    propsMaterial.diffuseTexture = Asset_LoadTexture("textures/vending_machine.png");
    propsMaterial.shader = defaultShader;


    for (const LevelEntity& levelEntity : levelLoader.entities)
    {
        switch (levelEntity.type)
        {
            case LevelEntityType_PlayerSpawn: {
                if (context->player != nullptr) continue;

                Entity* entity = SpawnEntity(game->context, EntityType_Player, v3(levelEntity.position.x, 0.6f, levelEntity.position.y));
                entity->transform.rotation.z = levelEntity.rotation;

                entity->movement.enabled = true;
                entity->movement.friction = 0.1f;
                entity->movement.applyFriction = true;
                entity->movement.velocity = v3(0.0f);

                entity->camera.enabled = true;
                entity->camera.fov = 45.0f;
                entity->camera.aspect = 16.0f / 9.0f;
                entity->camera.nearClip = 0.1f;
                entity->camera.farClip = 100.0f;

                entity->collider.enabled = true;
                entity->collider.type = ColliderType_Sphere;
                entity->collider.radius = 0.3f;

                entity->player.speed = 32.0f;
                
                context->player = entity;
            } break;

            case LevelEntityType_Door: {
                CreateDoor(context, v3(levelEntity.position.x, 0.0f, levelEntity.position.y), levelEntity.door.axis);
            } break;

            case LevelEntityType_Key: {
                Entity* entity = SpawnEntity(context, EntityType_Pickup, v3(levelEntity.x, 0.0f, levelEntity.y) + v3(0.5f, 0.25f, 0.5f));
                entity->transform.scale = v3(0.25f);

                entity->meshRenderer.enabled = true;
                entity->meshRenderer.mesh = &keyMesh;
                entity->meshRenderer.material = &keyMaterial;

                entity->collider.enabled = true;
                entity->collider.type = ColliderType_Box;
                entity->collider.aabb = CreateBox3(v3(-0.125f), v3(0.125f));
                entity->collider.isTrigger = true;
                entity->collider.onTriggerEnter = [](GameContext* context, Entity* entity, Entity* other) -> void {
                    if (other->type == EntityType_Player)
                    {
                        printf("Pickup key!\n");
                        DestroyEntity(context, entity);
                    }
                };

                entity->pickup.type = PickupType_Key;
                entity->pickup.data = levelEntity.id;
                entity->pickup.rotationSpeed = 45.0f;
                entity->pickup.defaultZ = entity->transform.position.z;
            } break;
            
            case LevelEntityType_Light: {
                Entity* entity = SpawnEntity(context, EntityType_Light, v3(levelEntity.x, 0.5f, levelEntity.y));
                entity->light.enabled = true;

                std::string hexValue = levelEntity.light.color;
                entity->light.color.r = std::stoi(hexValue.substr(0, 2), nullptr, 16) / 255.0f;
                entity->light.color.g = std::stoi(hexValue.substr(2, 2), nullptr, 16) / 255.0f;
                entity->light.color.b = std::stoi(hexValue.substr(4, 2), nullptr, 16) / 255.0f;

                entity->light.intensity = levelEntity.light.intensity;
                entity->light.range = levelEntity.light.range;
            } break;

            case LevelEntityType_Prop: {
                Mesh* propMeshes[] = {
                    &vendingMachineMesh,
                    &trashBinMesh,
                    &potPlantMesh
                };

                Entity* entity = SpawnEntity(context, EntityType_None, v3(levelEntity.position.x, 0.0f, levelEntity.position.y));
                entity->meshRenderer.enabled = true;
                entity->meshRenderer.mesh = propMeshes[levelEntity.prop.type];
                entity->meshRenderer.material = &propsMaterial;
                entity->collider.enabled = true;
                entity->collider.type = ColliderType_Box;
                entity->collider.aabb = propMeshes[levelEntity.prop.type]->aabb;
                entity->collider.aabb.max.y = 1.0f;
            } break;
        }
    }


    AudioId music = Asset_LoadAudio("audio/demo.wav");

    // The player has a point light attached to them
    {
        Entity* entity = SpawnEntity(context, EntityType_Light, v3(0.0f));
        entity->light.enabled = true;
        entity->light.range = 3.0f;
        entity->light.intensity = 0.8f;
        entity->light.color = v3(1.0f, 0.8f, 0.6f);

        context->player->player.pointLight = entity;
    }

    // Robot textures
    TextureId mutantTextures[] = {
        Asset_LoadTexture("textures/sprites/mutant0.png"),
        Asset_LoadTexture("textures/sprites/mutant1.png"),
        Asset_LoadTexture("textures/sprites/mutant2.png"),
        Asset_LoadTexture("textures/sprites/mutatt0.png"),
        Asset_LoadTexture("textures/sprites/mutatt1.png"),
        Asset_LoadTexture("textures/sprites/muthit0.png"),
        Asset_LoadTexture("textures/sprites/muthit1.png"),
        Asset_LoadTexture("textures/sprites/muthit2.png"),
        Asset_LoadTexture("textures/sprites/muthit3.png"),
    };


    Sprite mutantSprite0 = {};
    mutantSprite0.xOffset = 0;
    mutantSprite0.yOffset = 0;
    mutantSprite0.texture = mutantTextures[0];
    mutantSprite0.width = RHI_GetTextureWidth(mutantTextures[0]);
    mutantSprite0.height = RHI_GetTextureHeight(mutantTextures[0]);

    Sprite mutantSprite1 = {};
    mutantSprite1.xOffset = 0;
    mutantSprite1.yOffset = 0;
    mutantSprite1.texture = mutantTextures[1];
    mutantSprite1.width = RHI_GetTextureWidth(mutantTextures[1]);
    mutantSprite1.height = RHI_GetTextureHeight(mutantTextures[1]);

    Sprite mutantSprite2 = {};
    mutantSprite2.xOffset = 0;
    mutantSprite2.yOffset = 0;
    mutantSprite2.texture = mutantTextures[2];
    mutantSprite2.width = RHI_GetTextureWidth(mutantTextures[2]);
    mutantSprite2.height = RHI_GetTextureHeight(mutantTextures[2]);

    SpriteAnimation mutantSpriteAnimation = {};
    mutantSpriteAnimation.frameTime = 0.4f;
    mutantSpriteAnimation.frameCount = 2;
    mutantSpriteAnimation.loop = true;
    mutantSpriteAnimation.sprites[0] = &mutantSprite0;
    mutantSpriteAnimation.sprites[1] = &mutantSprite1;
    mutantSpriteAnimation.sprites[2] = &mutantSprite2;

    Vertex billboardVertices[4] = {};
    billboardVertices[0].position = v3(-0.5f, 0.0f, 0.0f);
    billboardVertices[1].position = v3( 0.5f, 0.0f, 0.0f);
    billboardVertices[2].position = v3( 0.5f, 1.0f, 0.0f);
    billboardVertices[3].position = v3(-0.5f, 1.0f, 0.0f);

    billboardVertices[0].texCoord0 = v2(0.0f, 0.0f);
    billboardVertices[1].texCoord0 = v2(1.0f, 0.0f);
    billboardVertices[2].texCoord0 = v2(1.0f, 1.0f);
    billboardVertices[3].texCoord0 = v2(0.0f, 1.0f);

    billboardVertices[0].normal = v3(0.0f, 0.0f, -1.0f);
    billboardVertices[1].normal = v3(0.0f, 0.0f, -1.0f);
    billboardVertices[2].normal = v3(0.0f, 0.0f, -1.0f);
    billboardVertices[3].normal = v3(0.0f, 0.0f, -1.0f);

    u32 billboardIndices[6] = {
        0, 1, 2,
        0, 2, 3
    };
    
    Mesh billboardMesh = CreateMesh(billboardVertices, 4, billboardIndices, 6);
    ShaderId billdboardShader = Asset_LoadShader("shaders/billboard.vs", "shaders/billboard.fs");
    
    Material billboardMaterial = {};
    {
        Entity* entity = SpawnEntity(context, EntityType_Enemy, v3(2.5f, 0.0f, 4.0f));
        billboardMaterial.diffuseTexture = mutantTextures[0];
        billboardMaterial.diffuseColor = v3(1.0f, 1.0f, 1.0f);
        billboardMaterial.shader = billdboardShader;

        entity->meshRenderer.enabled = true;
        entity->meshRenderer.mesh = &billboardMesh;
        entity->meshRenderer.material = &billboardMaterial;

        entity->collider.enabled = true;
        entity->collider.type = ColliderType_Box;
        entity->collider.aabb = CreateBox3(v3(-0.25f, 0.0f, -0.25f), v3(0.25f, 1.0f, 0.25f));
        entity->collider.offset = v3(0.0f);

        entity->spriteAnimation.enabled = true;
        entity->spriteAnimation.animation = &mutantSpriteAnimation;
    }


    while (!Platform_WindowShouldClose())
    {
        Platform_PollEvents();
        Audio_Update();

        if (IsKeyPressed(Key_Escape))
        {
            Platform_CloseWindow();
        }

        if (IsKeyPressed(Key_P))
        {
            AudioInstanceId instance = Audio_PlayClip(music);
        }
        if (IsKeyPressed(Key_F1))
        {
            game->settings.debugMode = !game->settings.debugMode;
        }

        static f64 lastTime = Platform_GetTime();
        f64 currentTime = Platform_GetTime();
        f32 deltaTime = (f32)(currentTime - lastTime);
        lastTime = currentTime;

        Game_Update(deltaTime);
        Game_Render();

        Platform_SwapBuffers();
        Input_NextFrame();
    }

    RHI_Shutdown();
    Audio_Shutdown();
    return 0;
}