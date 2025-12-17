#pragma once
#include "core/memory.h"
#include "renderer/color.h"
#include "renderer/rhi.h"
#include <glm/glm.hpp>

// Initializes the 2D renderer.
void Renderer2D_Init(Arena* arena);

// Shuts down the 2D renderer.
void Renderer2D_Shutdown();

// Sets the size of the rendering viewport.
void Renderer2D_SetSize(f32 width, f32 height);

// Sets the view offset for the 2D renderer.
void Renderer2D_SetViewOffset(f32 offsetX, f32 offsetY);

// Begins a new frame for the 2D renderer, setting up necessary state.
void Renderer2D_BeginFrame();

// Ends the current frame, rendering all queued 2D primitives.
void Renderer2D_EndFrame();

// Renders all queued 2D primitives and clears the queue.
void Renderer2D_Flush();

// Draws a line between two points with a specified color.
void Renderer2D_DrawLine(const glm::vec2& start, const glm::vec2& end, Color color);

// Draws a rectangle with the specified position, size, and color.
void Renderer2D_DrawRect(const glm::vec2& position, const glm::vec2& size, Color color);