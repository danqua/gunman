#include "render.h"
#include <glad/glad.h>

#define MAX_LINE_VERTICES 1024

struct LineVertex
{
    Vec3 position;
    Vec4 color;

    static VertexLayout layout;
};

VertexLayout LineVertex::layout = {
    2, { 3, 4 }
};
struct Renderer
{
    Mat4 view_matrix;           // View matrix for the current frame.
    Mat4 projection_matrix;     // Projection matrix for the current frame.

    Shader* line_shader;
    VertexBuffer* line_vertex_buffer;
    LineVertex* line_vertices;
    LineVertex* filled_rect_vertices;
    u32 line_vertex_count;
    u32 filled_rect_vertex_count;


    f32 z_order;
    b32 increase_z;
};

static Renderer renderer;

static void IncreaseZOrder()
{
    renderer.z_order += 0.0001f;
}

static Vec4 ColorToVec4(Color color)
{
    Vec4 result;
    result.x = color.r / 255.0f;
    result.y = color.g / 255.0f;
    result.z = color.b / 255.0f;
    result.w = color.a / 255.0f;
    return result;
}

void RendererInit(Arena* arena, s32 width, s32 height)
{
    gladLoadGL();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    renderer.line_shader = ShaderLoad("shaders/line.vert", "shaders/line.frag");
    renderer.line_vertex_buffer = VertexBufferCreate(nullptr, sizeof(LineVertex) * MAX_LINE_VERTICES);
    renderer.line_vertices = (LineVertex*)ArenaPushSize(arena, sizeof(LineVertex) * MAX_LINE_VERTICES);
    renderer.filled_rect_vertices = (LineVertex*)ArenaPushSize(arena, sizeof(LineVertex) * MAX_LINE_VERTICES);
}

void RendererShutdown()
{
    VertexBufferDestroy(renderer.line_vertex_buffer);
    ShaderDestroy(renderer.line_shader);
}

void RendererSetCamera(Camera* camera)
{
    renderer.projection_matrix = CameraGetProjectionMatrix(camera);
    renderer.view_matrix = CameraGetViewMatrix(camera);
}

void RendererBegin()
{
    renderer.line_vertex_count = 0;
    renderer.filled_rect_vertex_count = 0;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void RendererEnd()
{
    glEnable(GL_BLEND);
    ShaderBind(renderer.line_shader);
    ShaderSetMat4(renderer.line_shader, "u_view", renderer.view_matrix);
    ShaderSetMat4(renderer.line_shader, "u_projection", renderer.projection_matrix);
    VertexBufferBind(renderer.line_vertex_buffer);
    VertexBufferUpdate(renderer.line_vertex_buffer,
                       renderer.line_vertices,
                       sizeof(LineVertex) * renderer.line_vertex_count);

    VertexBufferSetLayout(renderer.line_vertex_buffer, &LineVertex::layout);
    glDrawArrays(GL_LINES, 0, renderer.line_vertex_count);

    VertexBufferUpdate(renderer.line_vertex_buffer,
                       renderer.filled_rect_vertices,
                       sizeof(LineVertex) * renderer.filled_rect_vertex_count);

    glDrawArrays(GL_QUADS, 0, renderer.filled_rect_vertex_count);

    glDisable(GL_BLEND);
}

void RendererDrawLine2D(Vec2 start, Vec2 end, Color color)
{
    LineVertex* v1 = &renderer.line_vertices[renderer.line_vertex_count++];
    v1->position = Vec3{ start.x, start.y, renderer.z_order };
    v1->color = ColorToVec4(color);

    LineVertex* v2 = &renderer.line_vertices[renderer.line_vertex_count++];
    v2->position = Vec3{ end.x, end.y, renderer.z_order };
    v2->color = ColorToVec4(color);

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

    renderer.filled_rect_vertices[renderer.filled_rect_vertex_count++] = { v1, ColorToVec4(color) };
    renderer.filled_rect_vertices[renderer.filled_rect_vertex_count++] = { v2, ColorToVec4(color) };
    renderer.filled_rect_vertices[renderer.filled_rect_vertex_count++] = { v3, ColorToVec4(color) };
    renderer.filled_rect_vertices[renderer.filled_rect_vertex_count++] = { v4, ColorToVec4(color) };

    IncreaseZOrder();
}