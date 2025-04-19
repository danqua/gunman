#include "render.h"
#include <glad/glad.h>

#define MAX_LINE_VERTICES 1024 * 16


static Vec4 ColorToVec4(Color color)
{
    Vec4 result;
    result.x = color.r / 255.0f;
    result.y = color.g / 255.0f;
    result.z = color.b / 255.0f;
    result.w = color.a / 255.0f;
    return result;
}

struct LineVertex
{
    Vec3 position;
    Vec4 color;

    static VertexLayout layout;
};

VertexLayout LineVertex::layout = {
    2, { 3, 4 }
};

enum PrimitiveType
{
    PrimitiveType_Lines,
    PrimitiveType_Triangles,
    PrimitiveType_Quads,
};

struct PrimitiveRenderer
{
    PrimitiveType type;
    LineVertex* vertices;
    VertexBuffer* vertex_buffer;
    u32 vertex_count;
};

PrimitiveRenderer PrimitiveRendererCreate(PrimitiveType type, Arena* arena, u32 max_vertices)
{
    PrimitiveRenderer result = {};
    result.vertex_buffer = VertexBufferCreate(nullptr, sizeof(LineVertex) * max_vertices);
    result.vertices = (LineVertex*)ArenaPushSize(arena, sizeof(LineVertex) * max_vertices);
    result.type = type;
    result.vertex_count = 0;
    return result;
}

void PrimitiveRendererDestroy(PrimitiveRenderer* primitive_renderer)
{
    VertexBufferDestroy(primitive_renderer->vertex_buffer);
    primitive_renderer->vertices = nullptr;
}

void PrimitiveRendererReset(PrimitiveRenderer* primitive_renderer)
{
    primitive_renderer->vertex_count = 0;
}

void PrimitiveRendererAddVertex(PrimitiveRenderer* primitive_renderer, Vec3 position, Color color)
{
    primitive_renderer->vertices[primitive_renderer->vertex_count].position = position;
    primitive_renderer->vertices[primitive_renderer->vertex_count].color = ColorToVec4(color);
    primitive_renderer->vertex_count++;
}

void PrimitiveRendererDraw(PrimitiveRenderer* primitive_renderer)
{
    VertexBufferBind(primitive_renderer->vertex_buffer);
    VertexBufferUpdate(primitive_renderer->vertex_buffer, primitive_renderer->vertices, sizeof(LineVertex) * primitive_renderer->vertex_count);
    VertexBufferSetLayout(primitive_renderer->vertex_buffer, &LineVertex::layout);

    if (primitive_renderer->type == PrimitiveType_Lines)
    {
        glDrawArrays(GL_LINES, 0, primitive_renderer->vertex_count);
    }
    else if (primitive_renderer->type == PrimitiveType_Triangles)
    {
        glDrawArrays(GL_TRIANGLES, 0, primitive_renderer->vertex_count);
    }
    else if (primitive_renderer->type == PrimitiveType_Quads)
    {
        glDrawArrays(GL_QUADS, 0, primitive_renderer->vertex_count);
    }
}

struct Renderer
{
    Mat4 view_matrix;           // View matrix for the current frame.
    Mat4 projection_matrix;     // Projection matrix for the current frame.

    Shader* line_shader;
    PrimitiveRenderer line_renderer;
    PrimitiveRenderer rect_renderer;
    PrimitiveRenderer quad_renderer;

    f32 z_order;
    b32 increase_z;
};

static Renderer renderer;

static void IncreaseZOrder()
{
    renderer.z_order += 0.0001f;
}


void RendererInit(Arena* arena, s32 width, s32 height)
{
    gladLoadGL();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_DEPTH_TEST);

    renderer.line_shader = ShaderLoad("shaders/line.vert", "shaders/line.frag");
    renderer.line_renderer = PrimitiveRendererCreate(PrimitiveType_Lines, arena, MAX_LINE_VERTICES);
    renderer.rect_renderer = PrimitiveRendererCreate(PrimitiveType_Quads, arena, MAX_LINE_VERTICES);
    renderer.quad_renderer = PrimitiveRendererCreate(PrimitiveType_Quads, arena, MAX_LINE_VERTICES);
}

void RendererShutdown()
{
    PrimitiveRendererDestroy(&renderer.line_renderer);
    PrimitiveRendererDestroy(&renderer.rect_renderer);
    PrimitiveRendererDestroy(&renderer.quad_renderer);
    ShaderDestroy(renderer.line_shader);
}

void RendererSetCamera(Camera* camera)
{
    renderer.projection_matrix = CameraGetProjectionMatrix(camera);
    renderer.view_matrix = CameraGetViewMatrix(camera);
}

void RendererBegin()
{
    PrimitiveRendererReset(&renderer.line_renderer);
    PrimitiveRendererReset(&renderer.rect_renderer);
    PrimitiveRendererReset(&renderer.quad_renderer);

    renderer.z_order = 0.0f;
    renderer.increase_z = true;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RendererEnd()
{
    glEnable(GL_BLEND);
    ShaderBind(renderer.line_shader);
    ShaderSetMat4(renderer.line_shader, "u_view", renderer.view_matrix);
    ShaderSetMat4(renderer.line_shader, "u_projection", renderer.projection_matrix);

    PrimitiveRendererDraw(&renderer.line_renderer);
    PrimitiveRendererDraw(&renderer.rect_renderer);
    PrimitiveRendererDraw(&renderer.quad_renderer);

    glDisable(GL_BLEND);
}

void RendererDrawLine2D(Vec2 start, Vec2 end, Color color)
{
    PrimitiveRendererAddVertex(&renderer.line_renderer, Vec3{ start.x, start.y, renderer.z_order }, color);
    PrimitiveRendererAddVertex(&renderer.line_renderer, Vec3{ end.x, end.y, renderer.z_order }, color);

    if (renderer.increase_z)
    {
        IncreaseZOrder();
    }
}

void RendererDrawRect2D(Vec2 position, Vec2 size, Color color)
{
    Vec2 p1 = { position.x, position.y };
    Vec2 p2 = { position.x + size.x, position.y };
    Vec2 p3 = { position.x + size.x, position.y + size.y };
    Vec2 p4 = { position.x, position.y + size.y };

    renderer.increase_z = false;
    RendererDrawLine2D(p1, p2, color);
    RendererDrawLine2D(p2, p3, color);
    RendererDrawLine2D(p3, p4, color);
    RendererDrawLine2D(p4, p1, color);
    renderer.increase_z = true;
}

void RendererDrawCircle2D(Vec2 position, f32 radius, Color color)
{
    u32 segments = 32;
    f32 angle = 0.0f;
    f32 angle_step = 2.0f * PI / segments;
    Vec2 p1 = { position.x + radius, position.y };
    Vec2 p2 = { position.x + radius * Cos(angle), position.y + radius * Sin(angle) };
    for (u32 i = 0; i < segments; ++i)
    {
        angle += angle_step;
        Vec2 p3 = { position.x + radius * Cos(angle), position.y + radius * Sin(angle) };
        RendererDrawLine2D(p2, p3, color);
        p2 = p3;
    }
}

void RendererDrawFilleRect2D(Vec2 position, Vec2 size, Color color)
{
    Vec3 v1 = { position.x, position.y, renderer.z_order };
    Vec3 v2 = { position.x + size.x, position.y, renderer.z_order };
    Vec3 v3 = { position.x + size.x, position.y + size.y, renderer.z_order };
    Vec3 v4 = { position.x, position.y + size.y, renderer.z_order };

    PrimitiveRendererAddVertex(&renderer.rect_renderer, v1, color);
    PrimitiveRendererAddVertex(&renderer.rect_renderer, v2, color);
    PrimitiveRendererAddVertex(&renderer.rect_renderer, v3, color);
    PrimitiveRendererAddVertex(&renderer.rect_renderer, v4, color);

    IncreaseZOrder();
}

void RendererDrawQuad(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4, Color color)
{
    PrimitiveRendererAddVertex(&renderer.quad_renderer, v1, color);
    PrimitiveRendererAddVertex(&renderer.quad_renderer, v2, color);
    PrimitiveRendererAddVertex(&renderer.quad_renderer, v3, color);
    PrimitiveRendererAddVertex(&renderer.quad_renderer, v4, color);
}

void RendererDrawQuadLines(Vec3 v1, Vec3 v2, Vec3 v3, Vec3 v4, Color color)
{
}
