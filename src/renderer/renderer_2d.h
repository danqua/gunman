#pragma once
#include "core/memory.h"
#include "renderer/color.h"
#include "renderer/rhi.h"
#include <glm/glm.hpp>

struct Camera2D {
    glm::vec2 center;           // World-space of the viewport center.
    f32 pixelsPerUnit;          // Pixels per world unit (scale). Larger => zoomed in.
    f32 minPixelsPerUnit;       // Minimum pixels per world unit.
    f32 maxPixelsPerUnit;       // Maximum pixels per world unit.
    glm::ivec2 viewportSize;    // Size of the viewport in pixels.
};

// Creates a default 2D camera with given viewport size.
Camera2D CreateDefaultCamera2D(s32 viewportWidth, s32 viewportHeight);

// Sets the viewport size for the 2D camera.
void Camera2D_SetViewport(Camera2D* camera, s32 viewportWidth, s32 viewportHeight);

// Sets the zoom limits for the 2D camera.
void Camera2D_SetZoomLimits(Camera2D* camera, f32 minPixelsPerUnit, f32 maxPixelsPerUnit);

// Converts screen coordinates to world coordinates using the 2D camera.
glm::vec2 Camera2D_ScreenToWorld(const Camera2D* camera, glm::vec2 screenPos);

// Converts world coordinates to screen coordinates using the 2D camera.
glm::vec2 Camera2D_WorldToScreen(const Camera2D* camera, glm::vec2 worldPos);

// Zooms the camera at a specific world point, keeping that point stationary on the screen.
void Camera2D_ZoomAtWorldPoint(Camera2D* camera, f32 zoomFactor, glm::vec2 anchorPoint);

// Returns the view matrix for the 2D camera.
glm::mat4 Camera2D_GetViewMatrix(const Camera2D* camera);


// Initializes the 2D renderer.
void Renderer2D_Init(Arena* arena);

// Shuts down the 2D renderer.
void Renderer2D_Shutdown();

// Sets the size of the rendering viewport.
void Renderer2D_SetSize(f32 width, f32 height);

// Sets the view offset for the 2D renderer.
void Renderer2D_SetViewOffset(f32 offsetX, f32 offsetY);

// Begins a new frame for the 2D renderer, setting up necessary state.
void Renderer2D_BeginFrame(const glm::mat4* projectionMatrix, const glm::mat4* viewMatrix);

// Ends the current frame, rendering all queued 2D primitives.
void Renderer2D_EndFrame();

// Renders all queued 2D primitives and clears the queue.
void Renderer2D_Flush();

// Draws a line between two points with a specified color.
void Renderer2D_DrawLine(const glm::vec2& start, const glm::vec2& end, Color color);

// Draws a rectangle with the specified position, size, and color.
void Renderer2D_DrawRect(const glm::vec2& position, const glm::vec2& size, Color color);